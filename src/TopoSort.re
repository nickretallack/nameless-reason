open Types;

let isRootNode = (node_id, connections) =>
  !
    Belt.Map.some(connections, (sink, source) =>
      switch (source) {
      | GraphConnection(_) => false
      | NodeConnection(connection) =>
        if (connection.node_id == node_id) {
          switch (sink) {
          | NodeConnection(_connection) => true
          | GraphConnection(_connection) => false
          };
        } else {
          false;
        }
      }
    );

let rec topoSort = (nodes: node_map(node), connections: connection_map) => {
  let (availableNodes, unavailableNodes) =
    Belt.Map.partition(nodes, (node_id, _node) =>
      isRootNode(node_id, connections)
    );
  let remainingConnections =
    Belt.Map.keep(connections, (sink, _source) =>
      switch (sink) {
      | GraphConnection(_connection) => false
      | NodeConnection(connection) =>
        !
          Belt.Map.some(availableNodes, (node_id, _node) =>
            connection.node_id == node_id
          )
      }
    );
  if (Belt.Map.isEmpty(unavailableNodes)) {
    [availableNodes];
  } else {
    [availableNodes, ...topoSort(unavailableNodes, remainingConnections)];
  };
};
