open Types;

let pixels = x => string_of_int(x) ++ "px";

let renderMap = (fn, map) =>
  ReasonReact.array(Array.map(fn, Belt.Map.toArray(map)));
let renderList = (fn, list) =>
  ReasonReact.array(Array.of_list(List.mapi(fn, list)));
let renderArray = (fn, array) => ReasonReact.array(Array.map(fn, array));

let getDocumentation = definition =>
  Belt.Map.getExn(definition.documentation, "en");
