[%%debugger.chrome];
open Types;
open Utils;

type react_ref = ref(option(ReasonReact.reactRef));

let component = ReasonReact.reducerComponent("Nib");
let make =
    (
      ~isSource: bool,
      ~nib_connection: nib_connection,
      ~emit: graph_action => unit,
      _children,
    ) => {
  ...component,
  initialState: () => ref(None),
  reducer: (_: unit, _) => ReasonReact.NoUpdate, /*ReasonReact.Update(action),*/
  didMount: self =>
    switch (self.state^) {
    | Some(element) =>
      ElementRe.addEventListener(
        "finish-drawing",
        event =>
          emit(
            FinishDrawing({
              pointer_id: Touch(getEventDetail(event)##identifier),
              nib_connection,
              isSource,
            }),
          ),
        element,
      )
    | None => ()
    },
  render: self =>
    <div
      ref=(ref => self.state := Js.Nullable.toOption(ref))
      className=(String.concat(" ", [isSource ? "source" : "sink", "nib"]))
      onMouseDown=(
        event =>
          emit(
            StartDrawing({
              pointer_id: Mouse,
              drawing_connection: {
                nib_connection,
                point: pointFromMouse(event),
                startIsSource: isSource,
              },
            }),
          )
      )
      onTouchStart=(
        event =>
          iterateTouches(event, touch =>
            emit(
              StartDrawing({
                pointer_id: Touch(touch##identifier),
                drawing_connection: {
                  nib_connection,
                  point: {
                    x: touch##clientX,
                    y: touch##clientY,
                  },
                  startIsSource: isSource,
                },
              }),
            )
          )
      )
      onMouseUp=(
        _ =>
          emit(FinishDrawing({pointer_id: Mouse, nib_connection, isSource}))
      )
      onTouchEnd=(
        event =>
          iterateTouches(event, touch =>
            dispatchCustomEvent(
              CustomEventRe.makeWithOptions(
                "finish-drawing",
                {
                  "detail": {
                    "identifier": touch##identifier,
                  },
                },
              ),
              DocumentRe.elementFromPoint(
                touch##clientX,
                touch##clientY,
                DomRe.document,
              ),
            )
            |> ignore
          )
      )
    />,
};
