[%%debugger.chrome];
open Types;
open ActionTypes;

let component = ReasonReact.reducerComponent("App");

let make = (~size, ~definitions, ~definition_id, _children) => {
  ...component,
  initialState: () => definitions,
  reducer: (action: app_action, state: app_state) =>
    switch (action) {
    | CreateConnection({definition_id, source, sink}) =>
      let definition = Belt.Map.getExn(state, definition_id);
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
      | _ => ReasonReact.NoUpdate /* TODO: Error? */
      };
    | ChangeName({definition_id, name}) =>
      let definition = Belt.Map.getExn(state, definition_id);
      switch (definition) {
      | Graph(definition) =>
        let documentation = Belt.Map.getExn(definition.documentation, "en");
        ReasonReact.Update(
          Belt.Map.set(
            state,
            definition_id,
            Graph({
              ...definition,
              documentation:
                Belt.Map.set(
                  definition.documentation,
                  "en",
                  {...documentation, name},
                ),
            }),
          ),
        );
      | Constant(definition) =>
        let documentation = Belt.Map.getExn(definition.documentation, "en");
        ReasonReact.Update(
          Belt.Map.set(
            state,
            definition_id,
            Constant({
              ...definition,
              documentation:
                Belt.Map.set(
                  definition.documentation,
                  "en",
                  {...documentation, name},
                ),
            }),
          ),
        );
      | Code(definition) =>
        let documentation = Belt.Map.getExn(definition.documentation, "en");
        ReasonReact.Update(
          Belt.Map.set(
            state,
            definition_id,
            Code({
              ...definition,
              documentation:
                Belt.Map.set(
                  definition.documentation,
                  "en",
                  {...documentation, name},
                ),
            }),
          ),
        );
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
        emit=(action => self.send(action))
      />
    },
};
