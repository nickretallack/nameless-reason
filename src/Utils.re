open Types;

let randomId = () => 

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

let makeNibMap = items =>
  Belt.Map.fromArray(items, ~id=(module NibComparator));

let getName = (definition: definition) : string =>
  switch (definition) {
  | Graph({documentation})
  | Code({documentation})
  | Interface({documentation}) => Belt.Map.getExn(documentation, "en").name
  | Constant({documentation, value}) =>
    let name = Belt.Map.getExn(documentation, "en").name;
    name != "" ? name : value;
  | Shape({documentation}) => Belt.Map.getExn(documentation, "en").name
  };

let setName = (definition: definition, name: string) : definition =>
  switch (definition) {
  | Graph(definition) =>
    Graph({
      ...definition,
      documentation:
        Belt.Map.set(
          definition.documentation,
          "en",
          {...Belt.Map.getExn(definition.documentation, "en"), name},
        ),
    })
  | Constant(definition) =>
    Constant({
      ...definition,
      documentation:
        Belt.Map.set(
          definition.documentation,
          "en",
          {...Belt.Map.getExn(definition.documentation, "en"), name},
        ),
    })
  | Code(definition) =>
    Code({
      ...definition,
      documentation:
        Belt.Map.set(
          definition.documentation,
          "en",
          {...Belt.Map.getExn(definition.documentation, "en"), name},
        ),
    })
  | Interface(definition) =>
    Interface({
      ...definition,
      documentation:
        Belt.Map.set(
          definition.documentation,
          "en",
          {...Belt.Map.getExn(definition.documentation, "en"), name},
        ),
    })
  | Shape(definition) =>
    Shape({
      ...definition,
      documentation:
        Belt.Map.set(
          definition.documentation,
          "en",
          {...Belt.Map.getExn(definition.documentation, "en"), name},
        ),
    })
  };

let setFunctionDocumentationDescription =
    (
      description: string,
      documentation: language_map(function_documentation),
    ) =>
  Belt.Map.set(
    documentation,
    "en",
    {...Belt.Map.getExn(documentation, "en"), description},
  );

let setDescription =
    (definition: definition, description: string)
    : definition =>
  switch (definition) {
  | Graph(definition) =>
    Graph({
      ...definition,
      documentation:
        setFunctionDocumentationDescription(
          description,
          definition.documentation,
        ),
    })
  | Code(definition) =>
    Code({
      ...definition,
      documentation:
        setFunctionDocumentationDescription(
          description,
          definition.documentation,
        ),
    })
  | Interface(definition) =>
    Interface({
      ...definition,
      documentation:
        setFunctionDocumentationDescription(
          description,
          definition.documentation,
        ),
    })
  | Constant(definition) =>
    Constant({
      ...definition,
      documentation:
        Belt.Map.set(
          definition.documentation,
          "en",
          {...Belt.Map.getExn(definition.documentation, "en"), description},
        ),
    })
  | Shape(definition) =>
    Shape({
      ...definition,
      documentation:
        Belt.Map.set(
          definition.documentation,
          "en",
          {...Belt.Map.getExn(definition.documentation, "en"), description},
        ),
    })
  };

let addInput = (definition: definition) : definition => {
  let id = "new"; /*string_of_int(Random.int);*/
  switch (definition) {
  | Graph(definition) =>
    let documentation = Belt.Map.getExn(definition.documentation, "en");
    Graph({
      ...definition,
      documentation:
        Belt.Map.set(
          definition.documentation,
          "en",
          {
            ...documentation,
            inputNames: Belt.Map.set(documentation.inputNames, id, ""),
          },
        ),
      display: {
        ...definition.display,
        inputOrder: List.append(definition.display.inputOrder, [id]),
      },
    });
  };
};

let setDocumentationName =
    (documentation: documentation, name: string)
    : documentation =>
  switch (documentation) {
  | BasicDocumentation({description}) =>
    BasicDocumentation({name, description})
  | FunctionDocumentation({description, inputNames, outputNames}) =>
    FunctionDocumentation({name, description, inputNames, outputNames})
  | ShapeDocumentation({description, fieldNames}) =>
    ShapeDocumentation({name, description, fieldNames})
  };

let getInputs = (definition: definition) =>
  switch (definition) {
  | Graph({documentation})
  | Code({documentation})
  | Interface({documentation}) =>
    Belt.Map.getExn(documentation, "en").inputNames
  | Constant(_) => makeNibMap([||])
  | _ => raise(Not_found)
  };

let getOutputs = (definition: definition) =>
  switch (definition) {
  | Graph({documentation})
  | Code({documentation})
  | Interface({documentation}) =>
    Belt.Map.getExn(documentation, "en").outputNames
  | Constant({documentation}) =>
    makeNibMap([|("value", Belt.Map.getExn(documentation, "en").name)|])
  | _ => raise(Not_found)
  };

let getRowIndex =
    (node: node, nib_id: nib_id, isSink: bool, definitions: definition_map) =>
  switch (node) {
  | Value(_)
  | Reference(_) => (-1)
  | Lambda(_) => 0
  | PointerCall(definition_id) =>
    switch (Belt.Map.getExn(definitions, definition_id)) {
    | Graph({display})
    | Code({display})
    | Interface({display}) =>
      if (isSink) {
        if (nib_id == "implementation") {
          0;
        } else {
          indexOf(nib_id, display.inputOrder) + 1;
        };
      } else {
        1
        + List.length(display.inputOrder)
        + indexOf(nib_id, display.outputOrder);
      }
    | _ => raise(Not_found)
    }
  | ShapeConstruct(definition_id) =>
    switch (Belt.Map.getExn(definitions, definition_id)) {
    | Shape({display}) =>
      if (isSink) {
        indexOf(nib_id, display.fieldOrder);
      } else {
        List.length(display.fieldOrder);
      }
    | _ => raise(Not_found)
    }
  | ShapeDestructure(definition_id) =>
    switch (Belt.Map.getExn(definitions, definition_id)) {
    | Shape({display}) =>
      if (isSink) {
        0;
      } else {
        indexOf(nib_id, display.fieldOrder) + 1;
      }
    | _ => raise(Not_found)
    }
  | Call(definition_id) =>
    switch (Belt.Map.getExn(definitions, definition_id)) {
    | Graph({display})
    | Code({display}) =>
      if (isSink) {
        indexOf(nib_id, display.inputOrder);
      } else {
        indexOf(nib_id, display.outputOrder)
        + List.length(display.inputOrder);
      }
    | _ => raise(Not_found)
    }
  };

let getOutputIndex = (node: node, nib_id: nib_id, definitions: definition_map) =>
  switch (node) {
  | Value(_)
  | ShapeConstruct(_)
  | Reference(_)
  | Lambda(_) => 0
  | ShapeDestructure(definition_id) =>
    let definition = Belt.Map.getExn(definitions, definition_id);
    switch (definition) {
    | Shape({display}) => indexOf(nib_id, display.fieldOrder)
    | _ => raise(Not_found)
    };
  | Call(definition_id) =>
    let definition = Belt.Map.getExn(definitions, definition_id);
    switch (definition) {
    | Graph({display})
    | Code({display}) => indexOf(nib_id, display.outputOrder)
    | _ => raise(Not_found)
    };
  | PointerCall(definition_id) =>
    let definition = Belt.Map.getExn(definitions, definition_id);
    switch (definition) {
    | Graph({display})
    | Code({display})
    | Interface({display}) => indexOf(nib_id, display.outputOrder)
    | _ => raise(Not_found)
    };
  };

let makeGraph = (~name, ~description, ~inputs, ~outputs, ~nodes, ~connections) =>
  Graph({
    documentation:
      Belt.Map.fromArray(
        [|
          (
            "en",
            {
              name,
              description,
              inputNames:
                Belt.Map.fromArray(inputs, ~id=(module NibComparator)),
              outputNames:
                Belt.Map.fromArray(outputs, ~id=(module NibComparator)),
            },
          ),
        |],
        ~id=(module LanguageComparator),
      ),
    display: {
      inputOrder: Array.to_list(Array.map(((id, _name)) => id, inputs)),
      outputOrder: Array.to_list(Array.map(((id, _name)) => id, outputs)),
    },
    implementation: {
      nodes: Belt.Map.fromArray(nodes, ~id=(module NodeComparator)),
      connections:
        Belt.Map.fromArray(connections, ~id=(module ConnectionComparator)),
    },
  });
