open Types;
let topoSort: (node_map(node), connection_map) => list(node_map(node));
let isRootNode: (node_id, connection_map) => bool;
