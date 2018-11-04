[%%debugger.chrome];
open Types;
open ActionTypes;
open Utils;

let component = ReasonReact.reducerComponent("App");

let make = (~size, ~definitions, _children) => {
  ...component,
  initialState: () => {
    definitions,
    definition_id: ReasonReact.Router.dangerouslyGetInitialUrl().hash,
  },
  didMount: self => {
    let watcherId =
      ReasonReact.Router.watchUrl(url => self.send(ChangeRoute(url)));
    self.onUnmount(() => ReasonReact.Router.unwatchUrl(watcherId));
  },
  reducer: (action: app_action, state: app_state) =>
    switch (action) {
    | ChangeRoute(url) =>
      ReasonReact.Update({...state, definition_id: url.hash})
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
      ReasonReact.Update({
        ...state,
        definitions:
          Belt.Map.set(
            state.definitions,
            definition_id,
            setName(definition, name),
          ),
      });
    | ChangeDescription({definition_id, description}) =>
      let definition = Belt.Map.getExn(state.definitions, definition_id);
      ReasonReact.Update({
        ...state,
        definitions:
          Belt.Map.set(
            state.definitions,
            definition_id,
            setDescription(definition, description),
          ),
      });
    | AddInput({definition_id}) =>
      let definition = Belt.Map.getExn(state.definitions, definition_id);
      ReasonReact.Update({
        ...state,
        definitions:
          Belt.Map.set(
            state.definitions,
            definition_id,
            addInput(definition),
          ),
      });
    },
  render: self =>
    switch (self.state.definition_id) {
    | "" => <DefinitionList definitions=self.state.definitions />
    | _ =>
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
        | Shape(_) => ReasonReact.string("A shape")
        | Interface(_) => ReasonReact.string("An interface")
        }
      }
    },
};
