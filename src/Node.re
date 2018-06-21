open Utils;
open Types;

let component = ReasonReact.statelessComponent("Node");

let make = (~definition, ~position, _children) => {
  ...component,
  render: _self => {
    let definition =
      switch (definition) {
      | Graph(graph_definition) => graph_definition
      };

    let documentation = getDocumentation(definition);

    <div
      className="node"
      style=(
        ReactDOMRe.Style.make(
          ~left=pixels(position.x),
          ~top=pixels(position.y),
          (),
        )
      )>
      (ReasonReact.string(documentation.name))
      (
        renderMap(
          ((nib_id, name)) =>
            <div className="input" key=nib_id>
              (ReasonReact.string(name))
              <div className="sink nib" />
            </div>,
          documentation.inputNames,
        )
      )
      (
        renderMap(
          ((nib_id, name)) =>
            <div className="output" key=nib_id>
              <div className="source nib" />
              (ReasonReact.string(name))
            </div>,
          documentation.outputNames,
        )
      )
    </div>;
  },
};
