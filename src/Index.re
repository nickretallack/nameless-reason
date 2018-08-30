open Types;
open Utils;

let example =
  makeGraph(
    ~name="Example Definition",
    ~description="An example function",
    ~inputs=[|("in1", "In 1"), ("in2", "In 2")|],
    ~outputs=[|("out1", "Out 1"), ("out2", "Out 2")|],
    ~nodes=[|
      ("node1", Call("example")),
      ("node2", Call("example")),
      ("node3", Call("example")),
    |],
    ~connections=[|
      (
        NodeConnection({node_id: "node1", nib_id: "in1"}),
        NodeConnection({node_id: "node2", nib_id: "out1"}),
      ),
      (
        NodeConnection({node_id: "node1", nib_id: "in2"}),
        NodeConnection({node_id: "node2", nib_id: "out2"}),
      ),
      (
        GraphConnection({nib_id: "out2"}),
        NodeConnection({node_id: "node1", nib_id: "out2"}),
      ),
      (
        NodeConnection({node_id: "node2", nib_id: "in1"}),
        GraphConnection({nib_id: "in1"}),
      ),
    |],
  );

let simple =
  makeGraph(
    ~name="Simple Definition",
    ~description="A simple function",
    ~inputs=[|("in1", "In 1"), ("in2", "In 2")|],
    ~outputs=[|("out1", "Out 1"), ("out2", "Out 2")|],
    ~nodes=[|
      ("node1", Value("one")),
      ("node2", Value("one")),
      ("node3", Call("plus")),
    |],
    ~connections=[|
      (
        GraphConnection({nib_id: "out1"}),
        NodeConnection({node_id: "node3", nib_id: "result"}),
      ),
      (
        NodeConnection({node_id: "node3", nib_id: "left"}),
        NodeConnection({node_id: "node1", nib_id: "value"}),
      ),
      (
        NodeConnection({node_id: "node3", nib_id: "right"}),
        NodeConnection({node_id: "node2", nib_id: "value"}),
      ),
    |],
  );

let one =
  Constant({
    documentation:
      Belt.Map.fromArray(
        [|("en", {name: "", description: ""})|],
        ~id=(module LanguageComparator),
      ),
    value: "1",
  });

let point =
  Shape({
    documentation:
      Belt.Map.fromArray(
        [|
          (
            "en",
            {
              name: "2D Point",
              description: "X and Y coordinates",
              fieldNames: makeNibMap([|("x", "X"), ("y", "Y")|]),
            },
          ),
        |],
        ~id=(module LanguageComparator),
      ),
    display: {
      fieldOrder: ["x", "y"],
    },
  });

let plus =
  Code({
    documentation:
      Belt.Map.fromArray(
        [|
          (
            "en",
            {
              name: "+",
              description: "Addition",
              inputNames:
                Belt.Map.fromArray(
                  [|("left", "Left"), ("right", "Right")|],
                  ~id=(module NibComparator),
                ),
              outputNames:
                Belt.Map.fromArray(
                  [|("result", "Result")|],
                  ~id=(module NibComparator),
                ),
            },
          ),
        |],
        ~id=(module LanguageComparator),
      ),
    implementation:
      Belt.Map.fromArray(
        [|(JavaScript, "function(left, right){return left + right}")|],
        ~id=(module ProgrammingLanguageComparator),
      ),
    display: {
      inputOrder: ["left", "right"],
      outputOrder: ["result"],
    },
  });

let pointExample =
  makeGraph(
    ~name="Point Example",
    ~description="",
    ~inputs=[||],
    ~outputs=[|("x", "X"), ("y", "Y")|],
    ~nodes=[|
      ("constructor", ShapeConstruct("point")),
      ("destructure", ShapeDestructure("point")),
      ("x", Value("one")),
      ("y", Value("one")),
    |],
    ~connections=[|
      (
        NodeConnection({node_id: "constructor", nib_id: "x"}),
        NodeConnection({node_id: "x", nib_id: "value"}),
      ),
      (
        NodeConnection({node_id: "constructor", nib_id: "y"}),
        NodeConnection({node_id: "y", nib_id: "value"}),
      ),
      (
        NodeConnection({node_id: "destructure", nib_id: "value"}),
        NodeConnection({node_id: "constructor", nib_id: "value"}),
      ),
      (
        GraphConnection({nib_id: "x"}),
        NodeConnection({node_id: "destructure", nib_id: "x"}),
      ),
      (
        GraphConnection({nib_id: "y"}),
        NodeConnection({node_id: "destructure", nib_id: "y"}),
      ),
    |],
  );

let referenceExample =
  makeGraph(
    ~name="Reference example",
    ~description="",
    ~inputs=[||],
    ~outputs=[|("reference", "Reference")|],
    ~nodes=[|("reference", Reference("Game State"))|],
    ~connections=[|
      (
        GraphConnection({nib_id: "reference"}),
        NodeConnection({node_id: "reference", nib_id: "value"}),
      ),
    |],
  );

let definitions =
  Belt.Map.fromArray(
    [|
      ("example", example),
      ("simple", simple),
      ("one", one),
      ("plus", plus),
      ("point", point),
      ("point-example", pointExample),
      ("reference-example", referenceExample),
    |],
    ~id=(module DefinitionComparator),
  );

ReactDOMRe.renderToElementWithId(
  <WindowSize render=(size => <App definitions size />) />,
  "graph",
);

Js.log(example);
