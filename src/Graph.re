open Types;
open Utils;

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
  initialState: () => Belt.Map.make(~id=(module PointerComparator)),
  reducer: (action: graph_action, state: graph_state) =>
    switch (action) {
    | StartDrawing({pointer_id, drawing_connection}) =>
      ReasonReact.Update(
        Belt.Map.set(
          state,
          pointer_id,
          DrawingConnection(drawing_connection),
        ),
      )
    | ContinueDrawing({pointer_id, point}) =>
      switch (Belt.Map.get(state, pointer_id)) {
      | Some(pointer_action) =>
        switch (pointer_action) {
        | DrawingConnection(drawing_connection) =>
          ReasonReact.Update(
            Belt.Map.set(
              state,
              pointer_id,
              DrawingConnection({...drawing_connection, point}),
            ),
          )
        | _ => ReasonReact.NoUpdate
        }
      | None => ReasonReact.NoUpdate
      }
    | FinishDrawing({pointer_id}) =>
      Belt.Map.has(state, pointer_id) ?
        ReasonReact.Update(Belt.Map.remove(state, pointer_id)) :
        ReasonReact.NoUpdate
    | StopDrawing({pointer_id}) =>
      Belt.Map.has(state, pointer_id) ?
        ReasonReact.Update(Belt.Map.remove(state, pointer_id)) :
        ReasonReact.NoUpdate
    },
  render: self => {
    let getDefinition = (definition_id: definition_id) : definition =>
      Belt.Map.getExn(definitions, definition_id);
    let getNode = node_id =>
      Belt.Map.getExn(definition.implementation.nodes, node_id);
    let documentation = getDocumentation(definition);
    let columns: list(node_map(node_implementation)) =
      TopoSort.topoSort(
        definition.implementation.nodes,
        definition.implementation.connections,
      );
    let columnWidth = size.x / (List.length(columns) + 1);
    let nodeWidth = 80;
    let textHeight = 20;

    let nodeHeight = (node: node_implementation) => {
      let definition =
        switch (getDefinition(node.definition_id)) {
        | Graph(definition) => definition
        };
      let documentation = getDocumentation(definition);
      (
        Belt.Map.size(documentation.inputNames)
        + Belt.Map.size(documentation.outputNames)
        + 1
      )
      * textHeight;
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
        };
      | GraphConnection({nib_id}) =>
        indexOf(nib_id, definition.display.inputOrder)
      };

    let getConnectionKey = sink =>
      switch (sink) {
      | NodeConnection({node_id, nib_id}) => node_id ++ nib_id
      | GraphConnection({nib_id}) => nib_id
      };

    let maybeEmit = (action: graph_action, self) : unit =>
      switch (action) {
      | FinishDrawing({pointer_id, nib_connection: end_nib, isSource}) =>
        switch (Belt.Map.get(self.ReasonReact.state, pointer_id)) {
        | Some(pointer_action) =>
          switch (pointer_action) {
          | DrawingConnection({startIsSource, nib_connection: start_nib}) =>
            startIsSource != isSource ?
              emit(
                CreateConnection({
                  definition_id,
                  source: startIsSource ? start_nib : end_nib,
                  sink: startIsSource ? end_nib : start_nib,
                }),
              ) :
              ()
          | _ => ()
          }
        | None => ()
        }
      | _ => ()
      };
    let handleNibAction = (action: graph_action, self) : unit => {
      maybeEmit(action, self);
      self.ReasonReact.send(action);
    };

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
          Array.iter(
            touch =>
              self.send(
                ContinueDrawing({
                  pointer_id: Touch(touch##identifier),
                  point: {
                    x: touch##clientX,
                    y: touch##clientY,
                  },
                }),
              ),
            convertToList(ReactEventRe.Touch.changedTouches(event)),
          )
      )
      onMouseUp=(_ => self.send(StopDrawing({pointer_id: Mouse})))
      onTouchEnd=(
        event =>
          Array.iter(
            touch =>
              self.send(
                StopDrawing({pointer_id: Touch(touch##identifier)}),
              ),
            convertToList(ReactEventRe.Touch.changedTouches(event)),
          )
      )>
      (ReasonReact.string(documentation.name))
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
          Belt.Map.keep(self.state, (_, pointer_action: pointer_action) =>
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
                emit=(self.handle(handleNibAction))
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
                emit=(self.handle(handleNibAction))
              />
              (ReasonReact.string(name))
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
              emit=(self.handle(handleNibAction))
            />,
          definition.implementation.nodes,
        )
      )
    </div>;
  },
};
