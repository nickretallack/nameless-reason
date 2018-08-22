open Utils;
open Types;

let component = ReasonReact.statelessComponent("Node");

let make =
    (
      ~node_id: node_id,
      ~name=?,
      ~inputs: nib_map(string),
      ~outputs: nib_map(string),
      ~position: point,
      ~emit: graph_action => unit,
      _children,
    ) => {
  ...component,
  render: _self =>
    <div className="node" style=(positionStyle(position))>
      (
        switch (name) {
        | Some(name) =>
          <div className="name"> (ReasonReact.string(name)) </div>
        | None => ReasonReact.null
        }
      )
      (
        renderMap(
          ((nib_id, name)) =>
            <div className="input" key=nib_id>
              (ReasonReact.string(name))
              <Nib
                isSource=false
                nib_connection=(NodeConnection({nib_id, node_id}))
                emit
              />
            </div>,
          inputs,
        )
      )
      (
        renderMap(
          ((nib_id, name)) =>
            <div className="output" key=nib_id>
              <Nib
                isSource=true
                nib_connection=(NodeConnection({nib_id, node_id}))
                emit
              />
              (ReasonReact.string(name))
            </div>,
          outputs,
        )
      )
    </div>,
};
