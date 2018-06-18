open Types;

let example_documentation = {
  name: "example",
  inputNames: StringMap.(empty |> add("in1", "In 1") |> add("in2", "In 2")),
  outputNames:
    StringMap.(empty |> add("out1", "Out 1") |> add("out2", "Out 2")),
};

let graph_definition = {
  documentation: StringMap.(empty |> add("en", example_documentation)),
};

let definition = Graph(graph_definition);
let definitions = StringMap.(empty |> add("example", definition));

ReactDOMRe.renderToElementWithId(
  <Connection sourcePosition={x: 20, y: 20} sinkPosition={x: 100, y: 100} />,
);
