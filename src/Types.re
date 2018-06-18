module StringMap =
  Map.Make({
    type t = string;
    let compare = compare;
  });

type point = {
  x: int,
  y: int,
};

type nib_connection = {
  node_id: string,
  nib_id: string,
};

type node_type =
  | Call
  | Value;

type node_implementation = {
  definition_id: string,
  node_type,
};

module ConnectionMap =
  Map.Make({
    type t = nib_connection;
    let compare = compare;
  });

type graph_implementation = {
  connections: ConnectionMap.t(nib_connection),
  nodes: StringMap.t(node_implementation),
};

type function_documentation = {
  name: string,
  inputNames: StringMap.t(string),
  outputNames: StringMap.t(string),
};

type graph_definition = {documentation: StringMap.t(function_documentation)};

/* TODO: more types of definitions */
type definition =
  | Graph(graph_definition);

type definition_map = StringMap.t(definition);
