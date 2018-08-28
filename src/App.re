[%%debugger.chrome];
open Types;
open ActionTypes;

let component = ReasonReact.reducerComponent("App");

let make = (~size, ~definitions, _children) => {
  ...component,
  initialState: () => {definitions, definition_id: "example"},
  subscriptions: self => [
    Sub(
      () => ReasonReact.Router.watchUrl(url => self.send(ChangeRoute(url))),
      ReasonReact.Router.unwatchUrl,
    ),
  ],
  reducer: (action: app_action, state: app_state) =>
    switch (action) {
    | ChangeRoute(url) =>
      ReasonReact.Update({
        ...state,
        definition_id:
          switch (url.hash) {
          | "" => "example"
          | definition_id => definition_id
          },
      })
    | CreateConnection({definition_id, source, sink}) =>
      let definition = Belt.Map.getExn(state.definitions, definition_id);
      switch (definition) {
      | Graph(graph_definition) =>
        ReasonReact.Update({
          ...state,
          definitions:
            Belt.Map.set(
              state.definitions,
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
        })
      | _ => ReasonReact.NoUpdate /* TODO: Error? */
      };
    | ChangeName({definition_id, name}) =>
      let definition = Belt.Map.getExn(state.definitions, definition_id);
      switch (definition) {
      | Graph(definition) =>
        let documentation = Belt.Map.getExn(definition.documentation, "en");
        ReasonReact.Update({
          ...state,
          definitions:
            Belt.Map.set(
              state.definitions,
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
        });
      | Constant(definition) =>
        let documentation = Belt.Map.getExn(definition.documentation, "en");
        ReasonReact.Update({
          ...state,
          definitions:
            Belt.Map.set(
              state.definitions,
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
        });
      | Code(definition) =>
        let documentation = Belt.Map.getExn(definition.documentation, "en");
        ReasonReact.Update({
          ...state,
          definitions:
            Belt.Map.set(
              state.definitions,
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
        });
      };
    },
  render: self =>
    switch (Belt.Map.get(self.state.definitions, self.state.definition_id)) {
    | None => ReasonReact.string("Not found")
    | Some(definition) =>
      switch (definition) {
      | Graph(definition) =>
        <Graph
          definition
          definition_id=self.state.definition_id
          definitions=self.state.definitions
          size
          emit=(action => self.send(action))
        />
      | Constant(_) => ReasonReact.string("A Constant")
      | Code(_) => ReasonReact.string("Some Code")
      }
    },
};
