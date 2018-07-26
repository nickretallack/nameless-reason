open Types;
let component = ReasonReact.reducerComponent("App");

let make = (~size, ~definitions, ~definition_id, _children) => {
  ...component,
  initialState: () => definitions,
  reducer: (action: app_action, state: app_state) =>
    switch (action) {
    | CreateConnection({definition_id, source, sink}) =>
      let definition = Belt.Map.getExn(state, definition_id);
      Js.log("hello");
      switch (definition) {
      | Graph(graph_definition) =>
        ReasonReact.Update(
          Belt.Map.set(
            state,
            definition_id,
            Graph({
              ...graph_definition,
              implementation: {
                ...graph_definition.implementation,
                connections:
                  Belt.Map.set(
                    graph_definition.implementation.connections,
                    sink,
                    source,
                  ),
              },
            }),
          ),
        )
      };
    },
  render: self =>
    switch (Belt.Map.getExn(self.state, definition_id)) {
    | Graph(definition) =>
      <Graph
        definition
        definition_id
        definitions=self.state
        size
        emit=(
          action => {
            Js.log(action);
            self.send(action);
          }
        )
      />
    },
};
