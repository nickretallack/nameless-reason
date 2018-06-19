type nib_id = string;
type node_id = string;
type definition_id = string;
type language_tag = string;

module NibComparator =
  Belt.Id.MakeComparable({
    type t = nib_id;
    let cmp = compare;
  });
type nib_map('a) = Belt.Map.t(NibComparator.t, 'a, NibComparator.identity);

module NodeComparator =
  Belt.Id.MakeComparable({
    type t = node_id;
    let cmp = compare;
  });
type node_map('a) =
  Belt.Map.t(NodeComparator.t, 'a, NodeComparator.identity);

module LanguageComparator =
  Belt.Id.MakeComparable({
    type t = language_tag;
    let cmp = compare;
  });
type language_map('a) =
  Belt.Map.t(LanguageComparator.t, 'a, LanguageComparator.identity);

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

module ConnectionComparator =
  Belt.Id.MakeComparable({
    type t = nib_connection;
    let cmp = compare;
  });
type connection_map =
  Belt.Map.t(nib_connection, nib_connection, ConnectionComparator.identity);

type graph_implementation = {
  connections: connection_map,
  nodes: node_map(node_implementation),
};

type function_documentation = {
  name: string,
  description: string,
  inputNames: nib_map(string),
  outputNames: nib_map(string),
};

type graph_definition = {
  documentation: language_map(function_documentation),
  implementation: graph_implementation,
};

/* TODO: more types of definitions */
type definition =
  | Graph(graph_definition);

module DefinitionComparator =
  Belt.Id.MakeComparable({
    type t = definition_id;
    let cmp = compare;
  });
type definition_map =
  Belt.Map.t(definition_id, definition, DefinitionComparator.identity);
