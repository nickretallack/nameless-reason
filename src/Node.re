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
    let inputs = Belt.Map.toArray(documentation.inputNames);
    let outputs = Belt.Map.toArray(documentation.outputNames);

    <div className="node">
      (ReasonReact.string(documentation.name))
      (
        renderMap(
          ((nib_id, name)) =>
            <div className="input" key=nib_id>
              (ReasonReact.string(name))
              <div className="sink nib" />
            </div>,
          inputs,
        )
      )
      (
        renderMap(
          ((nib_id, name)) =>
            <div className="output" key=nib_id>
              <div className="source nib" />
              (ReasonReact.string(name))
            </div>,
          outputs,
        )
      )
    </div>;
  },
};
