#include <Arduino.h>
#include "EC11.hpp"
#include "InfoDisplay.hpp"
#include "AppState.hpp"

EC11 knob1(17, 16, 23);
InfoDisplay infoDisplays[2] = {
    InfoDisplay(19, 18),
    InfoDisplay(21, 22)};

AppState st;

void setup()
{
  // state
  st.displaySelectActive = false;
  st.activeDisplay = 0;

  knob1.begin();
  for (int i = 0; i < sizeof(infoDisplays) / sizeof(infoDisplays[0]); i++)
  {
    infoDisplays[i].begin(&st);
  }

  log_d("booted.");
}

void loop()
{
  // read knob input
  EC11Event e;
  if (knob1.getEvents(&e))
  {
    // enable/disable display selection
    if (e.pressed)
    {
      st.displaySelectActive = !st.displaySelectActive;
      infoDisplays[st.activeDisplay].setSelected(st.displaySelectActive);
    }

    // change selection
    if (e.amount > 0 && st.displaySelectActive)
    {
      infoDisplays[st.activeDisplay].setSelected(false);

      // calculate next rotating index
      const int length = sizeof(infoDisplays) / sizeof(infoDisplays[0]);
      const int offset = (st.activeDisplay + (e.amount * (e.rotation == EC11Event::CW ? 1 : -1)));
      st.activeDisplay = (offset % length + length) % length;

      infoDisplays[st.activeDisplay].setSelected(true);
    }
  }

  // update displays
  for (int i = 0; i < sizeof(infoDisplays) / sizeof(infoDisplays[0]); i++)
  {
    infoDisplays[i].update();
  };
}
