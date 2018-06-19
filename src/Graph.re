open Types;

let component = ReasonReact.statelessComponent("Graph");

let make = (~definition, ~definitions, _children) => {
  ...component,
  render: _self => {
    let documentation = LanguageMap.find("en", definition.documentation);
    let inputs = NibMap.bindings(documentation.inputNames);
    let outputs = NibMap.bindings(documentation.outputNames);
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
