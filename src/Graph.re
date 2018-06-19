open Types;

let component = ReasonReact.statelessComponent("Graph");

let make = (~definition, ~definitions, _children) => {
  ...component,
  render: _self => {
    let documentation = StringMap.find("en", definition.documentation);
    let inputs = StringMap.bindings(documentation.inputNames);
    let outputs = StringMap.bindings(documentation.outputNames);
    <div>
      (ReasonReact.string(documentation.name))
      (
        ReasonReact.array(
          Array.of_list(
            List.map(
              ((_id, name)) => <div> (ReasonReact.string(name)) </div>,
              inputs,
            ),
          ),
        )
      )
      (
        ReasonReact.array(
          Array.of_list(
            List.map(
              ((_id, name)) => <div> (ReasonReact.string(name)) </div>,
              outputs,
            ),
          ),
        )
      )
    </div>;
  },
};
