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

[@bs.val]
external addEventListener : (string, Js.t({..}) => unit) => unit =
  "window.addEventListener";

[@bs.val] external innerWidth : int = "window.innerWidth";

[@bs.val] external innerHeight : int = "window.innerHeight";

let pointFromMouse = event => {
  x: ReactEventRe.Mouse.clientX(event),
  y: ReactEventRe.Mouse.clientY(event),
};

external convertToList : Js.t('a) => array('b) = "%identity";

let string_of_pointer_id = pointer_id =>
  switch (pointer_id) {
  | Mouse => "mouse"
  | Touch(id) => "touch-" ++ string_of_int(id)
  };

let iterateTouches = (event, callback) =>
  Array.iter(
    callback,
    convertToList(ReactEventRe.Touch.changedTouches(event)),
  );

[@bs.send.pipe: Dom.element]
external dispatchCustomEvent : Dom.customEvent => bool = "dispatchEvent";

[@bs.get] external getEventDetail : Dom.event => Js.t({..}) = "detail";

let getEventValue = event => ReactDOMRe.domElementToObj(
                               ReactEventRe.Form.target(event),
                             )##value;
