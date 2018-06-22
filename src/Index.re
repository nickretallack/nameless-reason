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

let graph_definition = {
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
};

let definition = Graph(graph_definition);
let definitions =
  Belt.Map.fromArray(
    [|("example", definition)|],
    ~id=(module DefinitionComparator),
  );

ReactDOMRe.renderToElementWithId(
  <WindowSize
    render=(size => <Graph definition=graph_definition definitions size />)
  />,
  "graph",
);
