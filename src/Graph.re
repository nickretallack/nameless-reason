open Types;
open Utils;

type pointer_id = string;
module PointerComparator =
  Belt.Id.MakeComparable({
    type t = pointer_id;
    let cmp = compare;
  });

type pointer_map('a) =
  Belt.Map.t(PointerComparator.t, 'a, PointerComparator.identity);

type drawing_connection = {
  nib_connection,
  startIsSource: bool,
  point,
};

type pointer_action =
  | DrawingConnection(drawing_connection)
  | MovingNode(node_id);

type state = {pointers: pointer_map(pointer_action)};

type start_drawing_action = {
  pointer_id: string,
  drawing_connection,
};

type continue_drawing_action = {
  pointer_id: string,
  point,
};

type action =
  | StartDrawing(start_drawing_action)
  | ContinueDrawing(continue_drawing_action);

let component = ReasonReact.reducerComponent("Graph");

let pointFromMouse = event => {
  x: ReactEventRe.Mouse.clientX(event),
  y: ReactEventRe.Mouse.clientY(event),
};

let make = (~definition, ~definitions, ~size, _children) => {
  ...component,
  initialState: () => {
    pointers: Belt.Map.make(~id=(module PointerComparator)),
  },
  reducer: (action, state) =>
    switch (action) {
    | StartDrawing({pointer_id, drawing_connection}) =>
      ReasonReact.Update({
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
    },
  render: self => {
    let getDefinition = definition_id =>
      Belt.Map.getExn(definitions, definition_id);
    let getNode = node_id =>
      Belt.Map.getExn(definition.implementation.nodes, node_id);
    let documentation = getDocumentation(definition);
    let columns =
      TopoSort.topoSort(
        definition.implementation.nodes,
        definition.implementation.connections,
      );
    let columnWidth = size.x / (List.length(columns) + 1);
    let nodeWidth = 80;
    let textHeight = 20;

    let nodeHeight = node => {
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
    let nodePositions =
      Belt.Map.mergeMany(
        Belt.Map.make(~id=(module NodeComparator)),
        Array.of_list(
          List.flatten(
            List.mapi(
              (column, nodes) => {
                let rowHeight = size.y / (Belt.Map.size(nodes) + 1);
                List.mapi(
                  (row, (node_id, node)) => (
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

    let getNibNudge = sink =>
      switch (sink) {
      | NodeConnection({node_id, nib_id}) =>
        let node = getNode(node_id);
        let definition = getDefinition(node.definition_id);
        switch (definition) {
        | Graph({display}) => indexOf(nib_id, display.inputOrder)
        };
      | GraphConnection({nib_id}) =>
        indexOf(nib_id, definition.display.outputOrder)
      };

    let getConnectionKey = sink =>
      switch (sink) {
      | NodeConnection({node_id, nib_id}) => node_id ++ nib_id
      | GraphConnection({nib_id}) => nib_id
      };

    /* let click = (_event, _self) => Js.log("clicked"); */

    <div
      className="graph"
      onMouseMove=(
        event =>
          self.send(
            ContinueDrawing({
              pointer_id: "mouse",
              point: pointFromMouse(event),
            }),
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
              nudge=(getNibNudge(sink))
            />,
          definition.implementation.connections,
        )
      )
      (
        renderMap(
          ((pointer_id, pointer_action)) =>
            switch (pointer_action) {
            | DrawingConnection({nib_connection, startIsSource, point}) =>
              <Connection
                key=pointer_id
                sourcePosition=(
                  startIsSource ?
                    getNibPosition(nib_connection, false) : point
                )
                sinkPosition=(
                  startIsSource ? point : getNibPosition(nib_connection, true)
                )
                nudge=(startIsSource ? 0 : getNibNudge(nib_connection))
              />
            | _ => ReasonReact.null
            },
          Belt.Map.keep(self.state.pointers, (_, pointer_action) =>
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
              <div
                className="source nib"
                onMouseDown=(
                  event =>
                    self.send(
                      StartDrawing({
                        pointer_id: "mouse",
                        drawing_connection: {
                          nib_connection: GraphConnection({nib_id: nib_id}),
                          point: pointFromMouse(event),
                          startIsSource: true,
                        },
                      }),
                    )
                )
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
              <div className="sink nib" />
              (ReasonReact.string(name))
            </div>,
          documentation.outputNames,
        )
      )
      (
        renderMap(
          ((node_id, node)) =>
            <Node
              key=node_id
              definition=(getDefinition(node.definition_id))
              position=(Belt.Map.getExn(nodePositions, node_id))
            />,
          definition.implementation.nodes,
        )
      )
    </div>;
  },
};
