open Types;
open Utils;

let component = ReasonReact.statelessComponent("Graph");

let rec topoSort = (nodes, connections) => {
  let (availableNodes, unavailableNodes) =
    Belt.Map.partition(nodes, (node_id, _node) =>
      !
        Belt.Map.some(connections, (sink, source) =>
          switch (source) {
          | GraphConnection(_connection) => false
          | NodeConnection(connection) =>
            if (connection.node_id == node_id) {
              switch (sink) {
              | NodeConnection(_connection) => true
              | GraphConnection(_connection) => false
              };
            } else {
              false;
            }
          }
        )
    );
  let remainingConnections =
    Belt.Map.keep(connections, (sink, _source) =>
      switch (sink) {
      | GraphConnection(_connection) => false
      | NodeConnection(connection) =>
        !
          Belt.Map.some(availableNodes, (node_id, _node) =>
            connection.node_id == node_id
          )
      }
    );
  if (Belt.Map.isEmpty(unavailableNodes)) {
    [availableNodes];
  } else {
    [availableNodes, ...topoSort(unavailableNodes, remainingConnections)];
  };
};

let make = (~definition, ~definitions, ~size, _children) => {
  ...component,
  render: _self => {
    let getDefinition = definition_id =>
      Belt.Map.getExn(definitions, definition_id);
    let documentation = getDocumentation(definition);
    let columns =
      topoSort(
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
    </div>;
  },
};
