#pragma once

#include "Event.h"

class KeyEvent : public Event {

public:
  enum KeyState{
    None = -1,
    Released = 0,
    Pressed = 1,
    Repeated = 2
  };

  KeyEvent(int,KeyState);
  ~KeyEvent();

  int GetKeyCode() {return keyCode;};
  KeyState GetKeyState() {return keyState;};

private:
  int keyCode = 0;
  KeyState keyState = KeyState::None;
};
