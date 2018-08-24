open Types;

let example_documentation = {
  name: "Example Definition",
  description: "An example function",
  inputNames:
    Belt.Map.fromArray(
      [|("in1", "In 1"), ("in2", "In 2")|],
      ~id=(module NibComparator),
    ),
  outputNames:
    Belt.Map.fromArray(
      [|("out1", "Out 1"), ("out2", "Out 2")|],
      ~id=(module NibComparator),
    ),
};

let example_implementation = {
  nodes:
    Belt.Map.fromArray(
      [|
        ("node1", {definition_id: "example", node_type: Call}),
        ("node2", {definition_id: "example", node_type: Call}),
        ("node3", {definition_id: "example", node_type: Call}),
      |],
      ~id=(module NodeComparator),
    ),
  connections:
    Belt.Map.fromArray(
      [|
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
      ~id=(module ConnectionComparator),
    ),
};

let example =
  Graph({
    documentation:
      Belt.Map.fromArray(
        [|("en", example_documentation)|],
        ~id=(module LanguageComparator),
      ),
    implementation: example_implementation,
    display: {
      inputOrder: ["in1", "in2"],
      outputOrder: ["out1", "out2"],
    },
  });

let simple =
  Graph({
    documentation:
      Belt.Map.fromArray(
        [|
          (
            "en",
            {
              name: "Simple Definition",
              description: "A simple function",
              inputNames:
                Belt.Map.fromArray(
                  [|("in1", "In 1"), ("in2", "In 2")|],
                  ~id=(module NibComparator),
                ),
              outputNames:
                Belt.Map.fromArray(
                  [|("out1", "Out 1"), ("out2", "Out 2")|],
                  ~id=(module NibComparator),
                ),
            },
          ),
        |],
        ~id=(module LanguageComparator),
      ),
    implementation: {
      nodes:
        Belt.Map.fromArray(
          [|
            ("node1", {definition_id: "one", node_type: Call}),
            ("node2", {definition_id: "one", node_type: Call}),
            ("node3", {definition_id: "plus", node_type: Call}),
          |],
          ~id=(module NodeComparator),
        ),
      connections:
        Belt.Map.fromArray(
          [|
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
          ~id=(module ConnectionComparator),
        ),
    },
    display: {
      inputOrder: ["in1", "in2"],
      outputOrder: ["out1", "out2"],
    },
  });

let one =
  Constant({
    documentation:
      Belt.Map.fromArray(
        [|("en", {name: "", description: ""})|],
        ~id=(module LanguageComparator),
      ),
    value: "1",
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

let definitions =
  Belt.Map.fromArray(
    [|
      ("example", example),
      ("simple", simple),
      ("one", one),
      ("plus", plus),
    |],
    ~id=(module DefinitionComparator),
  );

ReactDOMRe.renderToElementWithId(
  <WindowSize
    render=(size => <App definitions size definition_id="simple" />)
  />,
  "graph",
);
