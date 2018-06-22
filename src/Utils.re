open Types;

let pixels = x => string_of_int(x) ++ "px";

let positionStyle = position =>
  ReactDOMRe.Style.make(
    ~left=pixels(position.x),
    ~top=pixels(position.y),
    (),
  );

let renderMap = (fn, map) =>
  ReasonReact.array(Array.map(fn, Belt.Map.toArray(map)));
let renderList = (fn, list) =>
  ReasonReact.array(Array.of_list(List.mapi(fn, list)));
let renderArray = (fn, array) => ReasonReact.array(Array.map(fn, array));

let getDocumentation = definition =>
  Belt.Map.getExn(definition.documentation, "en");

let rec indexOfW = (needle, haystack, acc) =>
  switch (haystack) {
  | [] => raise(Not_found)
  | [head, ...rest] =>
    if (head == needle) {
      acc;
    } else {
      indexOfW(needle, rest, acc + 1);
    }
  };

let indexOf = (needle, haystack) => indexOfW(needle, haystack, 0);
