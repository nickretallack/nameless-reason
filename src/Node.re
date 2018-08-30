open Utils;
open Types;

let component = ReasonReact.statelessComponent("Node");

let make =
    (
      ~node_id: node_id,
      ~node: node,
      ~definitions: definition_map,
      ~position: point,
      ~emit: ActionTypes.graph_action => unit,
      _children,
    ) => {
  ...component,
  render: _self => {
    let makeNode = (~outputs, ~inputs=makeNibMap([||]), ~name=?, ()) =>
      <GenericNode node_id position emit ?name inputs outputs />;

    switch (node) {
    | Reference(name) =>
      makeNode(~outputs=makeNibMap([|("value", name)|]), ())
    | Value(definition_id) =>
      let definition = Belt.Map.getExn(definitions, definition_id);
      makeNode(
        ~outputs=makeNibMap([|("value", getName(definition))|]),
        (),
      );
    | Call(definition_id) =>
      let definition = Belt.Map.getExn(definitions, definition_id);
      makeNode(
        ~name=getName(definition),
        ~inputs=getInputs(definition),
        ~outputs=getOutputs(definition),
        (),
      );
    | PointerCall(definition_id) =>
      let definition = Belt.Map.getExn(definitions, definition_id);
      makeNode(
        ~name=getName(definition),
        ~inputs=
          Belt.Map.set(
            getInputs(definition),
            "implementation",
            "implementation",
          ),
        ~outputs=getOutputs(definition),
        (),
      );
    | Lambda(definition_id) =>
      let definition = Belt.Map.getExn(definitions, definition_id);
      makeNode(
        ~name=getName(definition),
        ~outputs=makeNibMap([|("implementation", "implementation")|]),
        (),
      );
    | ShapeConstruct(definition_id) =>
      let definition = Belt.Map.getExn(definitions, definition_id);
      switch (definition) {
      | Shape(definition) =>
        let documentation = Belt.Map.getExn(definition.documentation, "en");
        makeNode(
          ~name=documentation.name,
          ~inputs=documentation.fieldNames,
          ~outputs=makeNibMap([|("value", "value")|]),
          (),
        );
      | _ => raise(Not_found)
      };
    | ShapeDestructure(definition_id) =>
      let definition = Belt.Map.getExn(definitions, definition_id);
      switch (definition) {
      | Shape(definition) =>
        let documentation = Belt.Map.getExn(definition.documentation, "en");
        makeNode(
          ~name=documentation.name,
          ~inputs=makeNibMap([|("value", "value")|]),
          ~outputs=documentation.fieldNames,
          (),
        );
      | _ => raise(Not_found)
      };
    };
  },
};
