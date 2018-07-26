open Types;
let component = ReasonReact.reducerComponent("App");

let make = (~size, ~definitions, ~definition_id, _children) => {
  ...component,
  initialState: () => definitions,
  reducer: (action, state: definition_map) =>
    switch (action) {
    | CreateConnection({definition_id, source, sink}) => ReasonReact.NoUpdate
    /* let definition = Belt.Map.getExn(state, definition_id);
       ReasonReact.Update(
         Belt.Map.set(
           state,
           definition_id,
           Graph({
             ...definition,
             implementation: {
               ...definition.implementation,
               connections:
                 Belt.Map.set(
                   definition.implementation.connections,
                   sink,
                   source,
                 ),
             },
           }),
         ),
       ); */
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
