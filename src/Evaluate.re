/* open Types;

   let rec evaluateOutput = (definitions, definition_id, output_id) => {
     let definition = Belt.Map.getExn(definitions, definition_id);
     switch (definition) {
     | Constant({value}) => value
     | Graph({implementation}) =>
       let source =
         Belt.Map.get(
           implementation.connections,
           GraphConnection({nib_id: output_id}),
         );
       switch (source) {
       | None => "ERROR.  DISCONNECTED"
       | Some(connection) =>
         switch (connection) {
         | GraphConnection(_) => "Input needed!"
         | NodeConnection({node_id, nib_id}) =>
           let node = Belt.Map.getExn(implementation.nodes, node_id);
           evaluateOutput(definitions, node.definition_id, nib_id);
         }
       };
     };
   }; */
