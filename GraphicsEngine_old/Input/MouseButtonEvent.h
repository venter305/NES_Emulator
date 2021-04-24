#pragma once

#include "Event.h"

class MouseButtonEvent : public Event {

public:
  enum ButtonState{
    None = -1,
    Released = 0,
    Pressed = 1
  };

  enum ButtonType {
    Unknown = -1,
    Left = 0,
    Right = 1,
    Middle = 2
  };

  MouseButtonEvent(ButtonType,ButtonState,int,int);
  ~MouseButtonEvent();

  ButtonType GetButtonType() {return btnCode;};
  ButtonState GetButtonState() {return btnState;};

  double GetMouseX() {return mosPosX;};
  double GetMouseY() {return mosPosY;};

private:
  ButtonType btnCode = ButtonType::Unknown;
  ButtonState btnState = ButtonState::None;
  double mosPosX = 0;
  double mosPosY = 0;
};
