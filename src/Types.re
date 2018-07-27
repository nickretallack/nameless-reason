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

type function_display = {
  inputOrder: list(nib_id),
  outputOrder: list(nib_id),
};

type graph_definition = {
  documentation: language_map(function_documentation),
  implementation: graph_implementation,
  display: function_display,
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

type drawing_connection = {
  nib_connection,
  startIsSource: bool,
  point,
};

type pointer_action =
  | DrawingConnection(drawing_connection)
  | MovingNode(node_id);

type pointer_action_map = pointer_map(pointer_action);

type start_drawing_action = {
  pointer_id,
  drawing_connection,
};

type continue_drawing_action = {
  pointer_id,
  point,
};

type finish_drawing_action = {
  pointer_id,
  nib_connection,
  isSource: bool,
};

type stop_drawing_action = {pointer_id};

type graph_action =
  | StartDrawing(start_drawing_action)
  | ContinueDrawing(continue_drawing_action)
  | FinishDrawing(finish_drawing_action)
  | StopDrawing(stop_drawing_action);

type graph_state = {
  pointers: pointer_action_map,
  error: option(string),
};

type create_connection_action = {
  definition_id,
  source: nib_connection,
  sink: nib_connection,
};

type change_name_action = {
  definition_id,
  name: string,
};

type app_action =
  | CreateConnection(create_connection_action)
  | ChangeName(change_name_action);

type app_state = definition_map;
