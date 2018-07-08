open Types;
open Utils;

type action =
  | Resize(point);

let component = ReasonReact.reducerComponent("WindowSize");

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
