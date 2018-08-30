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

let detectCycles =
    (connections: connection_map, nodes: node_map(node))
    : bool =>
  List.exists(
    (node_id: node_id) =>
      dfs(
        connections,
        node_id,
        Belt.Set.add(Belt.Set.make(~id=(module NodeComparator)), node_id),
      ),
    Array.to_list(Belt.Map.keysToArray(nodes)),
  );
