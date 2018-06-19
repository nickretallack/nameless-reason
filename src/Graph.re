open Types;
open Utils;

let component = ReasonReact.statelessComponent("Graph");

let make = (~definition, ~definitions, ~size, _children) => {
  ...component,
  render: _self => {
    let getDefinition = definition_id =>
      Belt.Map.getExn(definitions, definition_id);
    let documentation = getDocumentation(definition);
    let inputs = Belt.Map.toArray(documentation.inputNames);
    let outputs = Belt.Map.toArray(documentation.outputNames);
    let nodes = Belt.Map.toArray(definition.implementation.nodes);
    <div>
      (ReasonReact.string(documentation.name))
      (
        renderMap(
          ((nib_id, name)) =>
            <div className="graph input" key=nib_id>
              (ReasonReact.string(name))
              <div className="source nib" />
            </div>,
          inputs,
        )
      )
      (
        renderMap(
          ((nib_id, name)) =>
            <div className="graph output" key=nib_id>
              <div className="sink nib" />
              (ReasonReact.string(name))
            </div>,
          outputs,
        )
      )
      (
        renderMap(
          ((node_id, node)) =>
            <Node
              key=node_id
              definition=(getDefinition(node.definition_id))
            />,
          nodes,
        )
      )
    </div>;
  },
};
