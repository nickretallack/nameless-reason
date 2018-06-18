let inputNames0 = Types.StringMap.empty;
let inputNames1 = Types.StringMap.add("in1", "In 1", inputNames0);
let inputNames2 = Types.StringMap.add("in2", "In 2", inputNames1);

let outputNames0 = Types.StringMap.empty;
let outputNames1 = Types.StringMap.add("out1", "Out 1", outputNames0);
let outputNames2 = Types.StringMap.add("out2", "Out 2", outputNames1);

let example_documentation: Types.function_documentation = {
  name: "example",
  inputNames: inputNames2,
  outputNames: outputNames2,
};

let documentation_map0 = Types.StringMap.empty;
let documentation_map1 =
  Types.StringMap.add("en", example_documentation, documentation_map0);

let graph_definition: Types.graph_definition = {
  documentation: documentation_map1,
};

let definition = Types.Graph(graph_definition);

let definitions0 = Types.StringMap.empty;
let definitions1 = Types.StringMap.add("example", definition);

ReactDOMRe.renderToElementWithId(
  <Connection sourcePosition={x: 20, y: 20} sinkPosition={x: 100, y: 100} />,
);
