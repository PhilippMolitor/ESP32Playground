#include <Arduino.h>
#include "EC11.hpp"
#include "InfoDisplay.hpp"
#include "AppState.hpp"

EC11 knob1(17, 16, 23);
InfoDisplay iDisplayA(19, 18);
InfoDisplay iDisplayB(21, 22);

AppState st;

void setup()
{
  // state
  st.counter = 0;
  st.activeDisplay = 0;

  knob1.begin();
  iDisplayA.begin();
  iDisplayB.begin();

  log_d("booted.");
}

void loop()
{
  // read knob input
  EC11Event e;
  if (knob1.getEvents(&e))
  {
    if (e.rotation == EC11Event::CW)
    {
      st.counter += e.amount;
    }
    else if (e.rotation == EC11Event::CCW)
    {
      st.counter -= e.amount;
    }

    if (e.pressed)
    {
      if (st.activeDisplay == 0)
      {
        st.activeDisplay = 1;
        iDisplayA.setActive(false);
        iDisplayB.setActive(true);
      }
      else
      {
        st.activeDisplay = 0;
        iDisplayA.setActive(true);
        iDisplayB.setActive(false);
      }
    }
  }

  // update displays
  iDisplayA.update(&st);
  iDisplayB.update(&st);
}
