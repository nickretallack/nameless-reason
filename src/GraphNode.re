open Utils;
open Types;

let component = ReasonReact.statelessComponent("Node");

let make =
    (
      ~node_id: node_id,
      ~definition: graph_definition,
      ~position: point,
      ~emit: ActionTypes.graph_action => unit,
      _children,
    ) => {
  ...component,
  render: _self => {
    let documentation = Belt.Map.getExn(definition.documentation, "en");
    <GenericNode
      node_id
      position
      emit
      name=documentation.name
      inputs=documentation.inputNames
      outputs=documentation.outputNames
    />;
  },
};
