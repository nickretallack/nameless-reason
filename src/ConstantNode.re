open Utils;
open Types;

let component = ReasonReact.statelessComponent("Node");

let make =
    (
      ~node_id: node_id,
      ~definition: constant_definition,
      ~position: point,
      ~emit: ActionTypes.graph_action => unit,
      _children,
    ) => {
  ...component,
  render: _self => {
    let documentation = Belt.Map.getExn(definition.documentation, "en");
    let name =
      if (documentation.name != "") {
        documentation.name;
      } else {
        definition.value;
      };
    <GenericNode
      node_id
      position
      emit
      inputs=(Belt.Map.fromArray([||], ~id=(module NibComparator)))
      outputs=(
        Belt.Map.fromArray([|("value", name)|], ~id=(module NibComparator))
      )
    />;
  },
};
