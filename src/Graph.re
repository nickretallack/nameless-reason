open Types;
open Utils;

let component = ReasonReact.statelessComponent("Graph");

let make = (~definition, ~definitions, ~size, _children) => {
  ...component,
  render: _self => {
    let getDefinition = definition_id =>
      Belt.Map.getExn(definitions, definition_id);
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
    let getNibPosition = (nib_connection, _isSink) =>
      switch (nib_connection) {
      | NodeConnection({node_id}) => getNodePosition(node_id)
      | GraphConnection(_) => {x: 0, y: size.y / 2}
      };

    let getConnectionKey = sink =>
      switch (sink) {
      | NodeConnection({node_id, nib_id}) => node_id ++ nib_id
      | GraphConnection({nib_id}) => nib_id
      };

    <div>
      (ReasonReact.string(documentation.name))
      (
        renderMap(
          ((nib_id, name)) =>
            <div className="graph input" key=nib_id>
              (ReasonReact.string(name))
              <div className="source nib" />
            </div>,
          documentation.inputNames,
        )
      )
      (
        renderMap(
          ((nib_id, name)) =>
            <div className="graph output" key=nib_id>
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
      (
        renderMap(
          ((sink, source)) =>
            <Connection
              key=(getConnectionKey(sink))
              sinkPosition=(getNibPosition(sink, true))
              sourcePosition=(getNibPosition(source, false))
            />,
          definition.implementation.connections,
        )
      )
    </div>;
  },
};
