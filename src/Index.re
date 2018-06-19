open Types;

let example_documentation = {
  name: "Example Definition",
  description: "An example function",
  inputNames: NibMap.(empty |> add("in1", "In 1") |> add("in2", "In 2")),
  outputNames:
    NibMap.(empty |> add("out1", "Out 1") |> add("out2", "Out 2")),
};

let example_implementation = {
  nodes:
    NodeMap.(
      empty
      |> add("node1", {definition_id: "example", node_type: Call})
      |> add("node2", {definition_id: "example", node_type: Call})
    ),
  connections:
    ConnectionMap.(
      empty
      |> add(
           NodeConnection({node_id: "node1", nib_id: "in1"}),
           NodeConnection({node_id: "node2", nib_id: "out1"}),
         )
      |> add(
           NodeConnection({node_id: "node1", nib_id: "in2"}),
           NodeConnection({node_id: "node2", nib_id: "out2"}),
         )
    ),
};

let graph_definition = {
  documentation: LanguageMap.(empty |> add("en", example_documentation)),
  implementation: example_implementation,
};

let definition = Graph(graph_definition);
let definitions = DefinitionMap.(empty |> add("example", definition));

ReactDOMRe.renderToElementWithId(
  <Graph definition=graph_definition definitions />,
  "graph",
);
