#pragma once

#include "Event.h"

class CharEvent : public Event {

public:
  CharEvent(unsigned int);
  ~CharEvent();

  unsigned int GetCharCode() {return charCode;};
private:
  unsigned int charCode = 0;
};
