open Types;
open Utils;

let component = ReasonReact.statelessComponent("Nib");
let make =
    (
      ~isSource: bool,
      ~nib_connection: nib_connection,
      ~emit: graph_action => unit,
      _children,
    ) => {
  ...component,
  render: _ =>
    <div
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
          Array.iter(
            touch =>
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
              ),
            convertToList(ReactEventRe.Touch.changedTouches(event)),
          )
      )
      onMouseUp=(
        _ =>
          emit(FinishDrawing({pointer_id: Mouse, nib_connection, isSource}))
      )
    />,
};
