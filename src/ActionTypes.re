open Types;

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
