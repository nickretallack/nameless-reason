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
type node_id_set = Belt.Set.t(NodeComparator.t, NodeComparator.identity);

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

type node =
  /* | Literal(string) */
  | Reference(string)
  | Call(definition_id)
  | Value(definition_id)
  | Lambda(definition_id)
  | PointerCall(definition_id)
  | ShapeConstruct(definition_id)
  | ShapeDestructure(definition_id);

module ConnectionComparator =
  Belt.Id.MakeComparable({
    type t = nib_connection;
    let cmp = compare;
  });
type connection_map =
  Belt.Map.t(nib_connection, nib_connection, ConnectionComparator.identity);

type graph_implementation = {
  connections: connection_map,
  nodes: node_map(node),
};

type programmingLanguage =
  | JavaScript;

module ProgrammingLanguageComparator =
  Belt.Id.MakeComparable({
    type t = programmingLanguage;
    let cmp = compare;
  });
type programmingLanguageMap =
  Belt.Map.t(
    programmingLanguage,
    string,
    ProgrammingLanguageComparator.identity,
  );

type basic_documentation = {
  name: string,
  description: string,
};

type function_documentation = {
  name: string,
  description: string,
  inputNames: nib_map(string),
  outputNames: nib_map(string),
};

type shape_documentation = {
  name: string,
  description: string,
  fieldNames: nib_map(string),
};

type documentation =
  | BasicDocumentation(basic_documentation)
  | ShapeDocumentation(shape_documentation)
  | FunctionDocumentation(function_documentation);

type function_display = {
  inputOrder: list(nib_id),
  outputOrder: list(nib_id),
};

type shape_display = {fieldOrder: list(nib_id)};

type graph_definition = {
  documentation: language_map(function_documentation),
  implementation: graph_implementation,
  display: function_display,
};

type code_definition = {
  documentation: language_map(function_documentation),
  implementation: programmingLanguageMap,
  display: function_display,
};

type constant_definition = {
  documentation: language_map(basic_documentation),
  value: string,
};

type interface_definition = {
  documentation: language_map(function_documentation),
  display: function_display,
};

type shape_definition = {
  documentation: language_map(shape_documentation),
  display: shape_display,
};

/* TODO: more types of definitions */
type definition =
  | Graph(graph_definition)
  | Code(code_definition)
  | Constant(constant_definition)
  | Interface(interface_definition)
  | Shape(shape_definition);

module DefinitionComparator =
  Belt.Id.MakeComparable({
    type t = definition_id;
    let cmp = compare;
  });
type definition_map =
  Belt.Map.t(definition_id, definition, DefinitionComparator.identity);

type pointer_id =
  | Mouse
  | Touch(int);

module PointerComparator =
  Belt.Id.MakeComparable({
    type t = pointer_id;
    let cmp = compare;
  });

type pointer_map('a) =
  Belt.Map.t(PointerComparator.t, 'a, PointerComparator.identity);
