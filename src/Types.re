type point = {
  x: int,
  y: int,
};

type nib_connection = {
  node_id: string,
  nib_id: string,
};

module StringMap =
  Map.Make({
    type t = string;
    let compare = compare;
  });

type function_documentation = {
  name: string,
  inputNames: StringMap.t(string),
  outputNames: StringMap.t(string),
};

type graph_definition = {documentation: StringMap.t(function_documentation)};

type definition =
  | Graph(graph_definition);

type definition_map = StringMap.t(definition);
