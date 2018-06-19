open Utils;
open Types;

let component = ReasonReact.statelessComponent("Node");

let make = (~definition, _children) => {
  ...component,
  render: _self => {
    let definition =
      switch (definition) {
      | Graph(graph_definition) => graph_definition
      };

    let documentation = getDocumentation(definition);
    let inputs = NibMap.bindings(documentation.inputNames);
    let outputs = NibMap.bindings(documentation.outputNames);

    <div>
      (ReasonReact.string(documentation.name))
      (
        renderMap(
          ((nib_id, name)) =>
            <div key=nib_id> (ReasonReact.string(name)) </div>,
          inputs,
        )
      )
      (
        renderMap(
          ((nib_id, name)) =>
            <div key=nib_id> (ReasonReact.string(name)) </div>,
          outputs,
        )
      )
    </div>;
  },
};
