open Types;
let topoSort:
  (node_map(node_implementation), connection_map) =>
  list(node_map(node_implementation));
let isRootNode: (node_id, connection_map) => bool;
