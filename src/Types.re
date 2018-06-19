module StringMap =
  Map.Make({
    type t = string;
    let compare = compare;
  });

type point = {
  x: int,
  y: int,
};

type node_connection = {
  node_id: string,
  nib_id: string,
};

type graph_connection = {nib_id: string};

type nib_connection =
  | NodeConnection(node_connection)
  | GraphConnection(graph_connection);

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
  description: string,
  inputNames: StringMap.t(string),
  outputNames: StringMap.t(string),
};

type graph_definition = {
  documentation: StringMap.t(function_documentation),
  implementation: graph_implementation,
};

/* TODO: more types of definitions */
type definition =
  | Graph(graph_definition);

type definition_map = StringMap.t(definition);
