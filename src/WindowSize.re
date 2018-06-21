open Types;

type action =
  | Resize(point);

let component = ReasonReact.reducerComponent("WindowSize");

[@bs.val]
external addEventListener : (string, Js.t({..}) => unit) => unit =
  "window.addEventListener";

[@bs.val] external innerWidth : int = "window.innerWidth";

[@bs.val] external innerHeight : int = "window.innerHeight";

let make = (~render, _children) => {
  ...component,
  initialState: () => {x: 1366, y: 768},
  reducer: (action, _state) =>
    switch (action) {
    | Resize(size) => ReasonReact.Update(size)
    },
  didMount: self => {
    self.send(Resize({x: innerWidth, y: innerHeight}));
    addEventListener("resize", _event =>
      self.send(Resize({x: innerWidth, y: innerHeight}))
    );
  },
  render: self => render(self.state),
};
