open Types;

let rec topoSort = (nodes, connections) => {
  let (availableNodes, unavailableNodes) =
    Belt.Map.partition(nodes, (node_id, _node) =>
      !
        Belt.Map.some(connections, (sink, source) =>
          switch (source) {
          | GraphConnection(_connection) => false
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
        )
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
