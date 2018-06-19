open Types;

let renderMap = (fn, array) => ReasonReact.array(Array.map(fn, array));
let renderList = list => ReasonReact.array(Array.of_list(list));
/* let renderMap = (function_, bindings) =>
   renderList(List.map(function_, bindings)); */

let getDocumentation = definition =>
  Belt.Map.getExn(definition.documentation, "en");
