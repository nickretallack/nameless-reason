type nib_id = string;
type node_id = string;
type definition_id = string;
type language_tag = string;

module NibMap =
  Map.Make({
    type t = nib_id;
    let compare = compare;
  });

module NodeMap =
  Map.Make({
    type t = node_id;
    let compare = compare;
  });

module DefinitionMap =
  Map.Make({
    type t = definition_id;
    let compare = compare;
  });

module LanguageMap =
  Map.Make({
    type t = language_tag;
    let compare = compare;
  });

type point = {
  x: int,
  y: int,
};

type node_connection = {
  node_id,
  nib_id,
};

type graph_connection = {nib_id};

type nib_connection =
  | NodeConnection(node_connection)
  | GraphConnection(graph_connection);

type node_type =
  | Call
  | Value;

type node_implementation = {
  definition_id,
  node_type,
};

module ConnectionMap =
  Map.Make({
    type t = nib_connection;
    let compare = compare;
  });

type graph_implementation = {
  connections: ConnectionMap.t(nib_connection),
  nodes: NodeMap.t(node_implementation),
};

type function_documentation = {
  name: string,
  description: string,
  inputNames: NibMap.t(string),
  outputNames: NibMap.t(string),
};

type graph_definition = {
  documentation: LanguageMap.t(function_documentation),
  implementation: graph_implementation,
};

/* TODO: more types of definitions */
type definition =
  | Graph(graph_definition);

type definition_map = DefinitionMap.t(definition);
