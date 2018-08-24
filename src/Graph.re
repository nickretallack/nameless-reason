[%%debugger.chrome];
open Types;
open ActionTypes;
open Utils;
open Evaluate;

let document = Webapi.Dom.Document.asEventTarget(Webapi.Dom.document);
let preventDefault = event => EventRe.preventDefault(event);
let component = ReasonReact.reducerComponent("Graph");
let make =
    (
      ~definition: graph_definition,
      ~definition_id: definition_id,
      ~definitions: definition_map,
      ~size: point,
      ~emit: app_action => unit,
      _children,
    ) => {
  ...component,
  /* Prevent iOS from scrolling all over the place */
  didMount: _ =>
    Webapi.Dom.EventTarget.addEventListenerWithOptions(
      "touchmove",
      preventDefault,
      {"passive": false, "capture": true, "once": false},
      document,
    ),
  willUnmount: _ =>
    Webapi.Dom.EventTarget.removeEventListener(
      "touchmove",
      preventDefault,
      document,
    ),
  initialState: () => {
    error: None,
    pointers: Belt.Map.make(~id=(module PointerComparator)),
  },
  reducer: (action: graph_action, state: graph_state) =>
    switch (action) {
    | StartDrawing({pointer_id, drawing_connection}) =>
      ReasonReact.Update({
        ...state,
        pointers:
          Belt.Map.set(
            state.pointers,
            pointer_id,
            DrawingConnection(drawing_connection),
          ),
      })
    | ContinueDrawing({pointer_id, point}) =>
      switch (Belt.Map.get(state.pointers, pointer_id)) {
      | Some(pointer_action) =>
        switch (pointer_action) {
        | DrawingConnection(drawing_connection) =>
          ReasonReact.Update({
            ...state,
            pointers:
              Belt.Map.set(
                state.pointers,
                pointer_id,
                DrawingConnection({...drawing_connection, point}),
              ),
          })
        | _ => ReasonReact.NoUpdate
        }
      | None => ReasonReact.NoUpdate
      }
    | FinishDrawing({pointer_id, nib_connection: end_nib, isSource}) =>
      switch (Belt.Map.get(state.pointers, pointer_id)) {
      | Some(pointer_action) =>
        switch (pointer_action) {
        | DrawingConnection({startIsSource, nib_connection: start_nib}) =>
          startIsSource != isSource ?
            DetectCycles.detectCycles(
              Belt.Map.set(
                definition.implementation.connections,
                startIsSource ? end_nib : start_nib,
                startIsSource ? start_nib : end_nib,
              ),
              definition.implementation.nodes,
            ) ?
              ReasonReact.Update({
                ...state,
                error: Some("Can't create cycles"),
              }) :
              ReasonReact.UpdateWithSideEffects(
                {
                  error: None,
                  pointers: Belt.Map.remove(state.pointers, pointer_id),
                },
                (
                  _ =>
                    emit(
                      CreateConnection({
                        definition_id,
                        source: startIsSource ? start_nib : end_nib,
                        sink: startIsSource ? end_nib : start_nib,
                      }),
                    )
                ),
              ) :
            ReasonReact.Update({
              ...state,
              error:
                Some(
                  startIsSource ?
                    "Can't connect a source to a source" :
                    "Can't connect a sink to a sink",
                ),
            })
        | _ => ReasonReact.NoUpdate
        }
      | None => ReasonReact.NoUpdate
      }
    | StopDrawing({pointer_id}) =>
      Belt.Map.has(state.pointers, pointer_id) ?
        ReasonReact.Update({
          ...state,
          pointers: Belt.Map.remove(state.pointers, pointer_id),
        }) :
        ReasonReact.NoUpdate
    },
  render: self => {
    let getDefinition = (definition_id: definition_id) : definition =>
      Belt.Map.getExn(definitions, definition_id);
    let getNode = node_id =>
      Belt.Map.getExn(definition.implementation.nodes, node_id);
    let documentation = Belt.Map.getExn(definition.documentation, "en");
    let columns: list(node_map(node_implementation)) =
      TopoSort.topoSort(
        definition.implementation.nodes,
        definition.implementation.connections,
      );
    let columnWidth = size.x / (List.length(columns) + 1);
    let nodeWidth = 80;
    let textHeight = 20;

    let nodeHeight = (node: node_implementation) =>
      switch (getDefinition(node.definition_id)) {
      | Graph(definition) =>
        let documentation = Belt.Map.getExn(definition.documentation, "en");
        (
          Belt.Map.size(documentation.inputNames)
          + Belt.Map.size(documentation.outputNames)
          + 1
        )
        * textHeight;
      | Constant(_) => 1
      };
    let nodePositions: node_map(point) =
      Belt.Map.mergeMany(
        Belt.Map.make(~id=(module NodeComparator)),
        Array.of_list(
          List.flatten(
            List.mapi(
              (column, nodes: node_map(node_implementation)) => {
                let rowHeight = size.y / (Belt.Map.size(nodes) + 1);
                List.mapi(
                  (row, (node_id, node: node_implementation)) => (
                    node_id,
                    {
                      x: columnWidth * (column + 1) - nodeWidth / 2,
                      y: rowHeight * (row + 1) - nodeHeight(node) / 2,
                    },
                  ),
                  Belt.Map.toList(nodes),
                );
              },
              columns,
            ),
          ),
        ),
      );
    let getNodePosition = node_id => Belt.Map.getExn(nodePositions, node_id);

    let nibOffset = 10;
    let nibPositions = (nibIds, isInput) => {
      let rowHeight = size.y / (List.length(nibIds) + 1);
      Belt.Map.fromArray(
        Array.of_list(
          List.mapi(
            (index, nib_id) => (
              nib_id,
              {
                x:
                  if (isInput) {
                    size.x - nibOffset;
                  } else {
                    nibOffset;
                  },
                y: (index + 1) * rowHeight,
              },
            ),
            nibIds,
          ),
        ),
        ~id=(module NibComparator),
      );
    };
    let inputPositions = nibPositions(definition.display.inputOrder, true);
    let outputPositions = nibPositions(definition.display.outputOrder, false);

    let getNibPosition = (nib_connection, isSink) =>
      switch (nib_connection) {
      | NodeConnection({node_id, nib_id}) =>
        let nodePosition = getNodePosition(node_id);
        let node = getNode(node_id);
        let definition = getDefinition(node.definition_id);
        {
          x: nodePosition.x + (if (isSink) {80} else {0}),
          y:
            (
              (
                switch (definition) {
                | Graph({display}) =>
                  if (isSink) {
                    indexOf(nib_id, display.inputOrder);
                  } else {
                    indexOf(nib_id, display.outputOrder)
                    + List.length(display.inputOrder);
                  }
                | Constant(_) => (-1)
                }
              )
              + 1
            )
            * textHeight
            + textHeight
            / 2
            + nodePosition.y,
        };
      | GraphConnection({nib_id}) =>
        Belt.Map.getExn(
          if (isSink) {outputPositions} else {inputPositions},
          nib_id,
        )
      };

    let getNibNudge = source =>
      switch (source) {
      | NodeConnection({node_id, nib_id}) =>
        let node = getNode(node_id);
        let definition = getDefinition(node.definition_id);
        switch (definition) {
        | Graph({display}) => indexOf(nib_id, display.outputOrder)
        | Constant(_) => 0
        };
      | GraphConnection({nib_id}) =>
        indexOf(nib_id, definition.display.inputOrder)
      };

    let getConnectionKey = sink =>
      switch (sink) {
      | NodeConnection({node_id, nib_id}) => node_id ++ nib_id
      | GraphConnection({nib_id}) => nib_id
      };

    let changeName = event =>
      emit(ChangeName({definition_id, name: getEventValue(event)}));

    let evaluate = output_id =>
      Js.log(evaluateOutput(definitions, definition_id, output_id));
    <div
      className="graph"
      onMouseMove=(
        event => {
          ReactEventRe.Mouse.preventDefault(event);
          self.send(
            ContinueDrawing({
              pointer_id: Mouse,
              point: pointFromMouse(event),
            }),
          );
        }
      )
      onTouchMove=(
        event =>
          iterateTouches(event, touch =>
            self.send(
              ContinueDrawing({
                pointer_id: Touch(touch##identifier),
                point: {
                  x: touch##clientX,
                  y: touch##clientY,
                },
              }),
            )
          )
      )
      onMouseUp=(_ => self.send(StopDrawing({pointer_id: Mouse})))
      onTouchEnd=(
        event =>
          iterateTouches(event, touch =>
            self.send(StopDrawing({pointer_id: Touch(touch##identifier)}))
          )
      )>
      <input
        type_="text"
        className="graph-name"
        value=documentation.name
        onChange=changeName
      />
      (
        switch (self.state.error) {
        | Some(error) =>
          <div className="error-message"> (ReasonReact.string(error)) </div>
        | None => ReasonReact.null
        }
      )
      (
        renderMap(
          ((sink, source)) =>
            <Connection
              key=(getConnectionKey(sink))
              sinkPosition=(getNibPosition(sink, true))
              sourcePosition=(getNibPosition(source, false))
              nudge=(getNibNudge(source))
            />,
          definition.implementation.connections,
        )
      )
      (
        renderMap(
          ((pointer_id: pointer_id, pointer_action: pointer_action)) =>
            switch (pointer_action) {
            | DrawingConnection({nib_connection, startIsSource, point}) =>
              <Connection
                key=(string_of_pointer_id(pointer_id))
                sourcePosition=(
                  startIsSource ?
                    getNibPosition(nib_connection, false) : point
                )
                sinkPosition=(
                  startIsSource ? point : getNibPosition(nib_connection, true)
                )
                nudge=(startIsSource ? getNibNudge(nib_connection) : 0)
              />
            | _ => ReasonReact.null
            },
          Belt.Map.keep(
            self.state.pointers, (_, pointer_action: pointer_action) =>
            switch (pointer_action) {
            | DrawingConnection(_) => true
            | _ => false
            }
          ),
        )
      )
      (
        renderMap(
          ((nib_id, name)) =>
            <div
              className="graph-input input"
              key=nib_id
              style=(
                ReactDOMRe.Style.make(
                  ~right=pixels(10),
                  ~top=pixels(Belt.Map.getExn(inputPositions, nib_id).y),
                  (),
                )
              )>
              (ReasonReact.string(name))
              <Nib
                isSource=true
                nib_connection=(GraphConnection({nib_id: nib_id}))
                emit=self.send
              />
            </div>,
          documentation.inputNames,
        )
      )
      (
        renderMap(
          ((nib_id, name)) =>
            <div
              className="graph-output output"
              key=nib_id
              style=(positionStyle(Belt.Map.getExn(outputPositions, nib_id)))>
              <Nib
                isSource=false
                nib_connection=(GraphConnection({nib_id: nib_id}))
                emit=self.send
              />
              (ReasonReact.string(name))
              <a className="evaluate" onClick=(_ => evaluate(nib_id))>
                (ReasonReact.string("evaluate"))
              </a>
            </div>,
          documentation.outputNames,
        )
      )
      (
        renderMap(
          ((node_id: node_id, node: node_implementation)) =>
            <Node
              key=node_id
              node_id
              definition=(getDefinition(node.definition_id))
              position=(Belt.Map.getExn(nodePositions, node_id))
              emit=self.send
            />,
          definition.implementation.nodes,
        )
      )
    </div>;
  },
};
