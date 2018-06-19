open Types;

let example_documentation = {
  name: "Example Definition",
  inputNames: StringMap.(empty |> add("in1", "In 1") |> add("in2", "In 2")),
  outputNames:
    StringMap.(empty |> add("out1", "Out 1") |> add("out2", "Out 2")),
};

let example_implementation = {
  nodes:
    StringMap.(
      empty
      |> add("node1", {definition_id: "example", node_type: Call})
      |> add("node2", {definition_id: "example", node_type: Call})
    ),
  connections:
    ConnectionMap.(
      empty
      |> add(
           {node_id: "node1", nib_id: "in1"},
           {node_id: "node2", nib_id: "out1"},
         )
      |> add(
           {node_id: "node1", nib_id: "in2"},
           {node_id: "node2", nib_id: "out2"},
         )
    ),
};

let graph_definition = {
  documentation: StringMap.(empty |> add("en", example_documentation)),
};

let definition = Graph(graph_definition);
let definitions = StringMap.(empty |> add("example", definition));

ReactDOMRe.renderToElementWithId(
  <Graph definition=graph_definition definitions />,
  "graph",
);
