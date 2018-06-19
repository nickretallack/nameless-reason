open Types;

let renderList = list => ReasonReact.array(Array.of_list(list));
let renderMap = (function_, bindings) =>
  renderList(List.map(function_, bindings));

let getDocumentation = definition =>
  LanguageMap.find("en", definition.documentation);
