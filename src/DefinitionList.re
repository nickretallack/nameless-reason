open Utils;
open Types;

let component = ReasonReact.statelessComponent("DefinitionList");

let make = (~definitions: definition_map, _children) => {
  ...component,
  render: _self =>
    <ul>
      (
        renderMap(
          ((definition_id, definition)) => {
            Js.log(getName(definition));
            <li key=definition_id>
              <a href=("#" ++ definition_id)>
                (ReasonReact.string(getName(definition)))
              </a>
            </li>;
          },
          definitions,
        )
      )
    </ul>,
};
