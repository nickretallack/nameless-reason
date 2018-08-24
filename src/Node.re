open Utils;
open Types;

let component = ReasonReact.statelessComponent("Node");

let make =
    (
      ~node_id: node_id,
      ~definition: definition,
      ~position: point,
      ~emit: ActionTypes.graph_action => unit,
      _children,
    ) => {
  ...component,
  render: _self =>
    switch (definition) {
    | Graph(graph_definition) =>
      <GraphNode definition=graph_definition node_id position emit />
    | Constant(constant_definition) =>
      <ConstantNode definition=constant_definition node_id position emit />
    },
};
