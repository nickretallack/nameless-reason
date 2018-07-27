[%%debugger.chrome];
open Types;

/* Find all source node_ids that are connected to this sink node_id */
let followConnections =
    (connections: connection_map, node_id: node_id)
    : node_id_set =>
  Belt.Map.reduce(
    connections,
    Belt.Set.make(~id=(module NodeComparator)),
    (acc, sink, source) =>
    switch (sink) {
    | NodeConnection({node_id: sink_node_id}) =>
      node_id == sink_node_id ?
        switch (source) {
        | NodeConnection({node_id: source_node_id}) =>
          Belt.Set.add(acc, source_node_id)
        | _ => acc
        } :
        acc
    | _ => acc
    }
  );

let rec dfs = (connections: connection_map, node_id, seen: node_id_set) : bool =>
  List.exists(
    source_node_id =>
      Belt.Set.has(seen, source_node_id)
      || dfs(connections, source_node_id, Belt.Set.add(seen, node_id)),
    Belt.Set.toList(followConnections(connections, node_id)),
  );

let detectCycles = (connections: connection_map) : bool => {
  Js.log(
    Belt.Map.has(
      connections,
      NodeConnection({node_id: "node1", nib_id: "out1"}),
    ),
  );
  Belt.Set.some(
    Belt.Map.reduce(
      connections,
      Belt.Set.make(~id=(module NodeComparator)),
      (acc: node_id_set, sink, source) =>
      switch (sink) {
      | GraphConnection(_) =>
        switch (source) {
        | NodeConnection({node_id}) => Belt.Set.add(acc, node_id)
        | _ => acc
        }
      | _ => acc
      }
    ),
    (node_id: node_id) =>
    dfs(
      connections,
      node_id,
      Belt.Set.add(Belt.Set.make(~id=(module NodeComparator)), node_id),
    )
  );
};
