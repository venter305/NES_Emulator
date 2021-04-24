#pragma once

class Event{

public:
  enum EventType{
    None = 0,
    Key = 1,
    Character = 2,
    MouseCursor = 3,
    MouseButton = 4,
    Window = 5
  };

  Event();
  ~Event();

  EventType GetType() {return type;};

protected:
  EventType type = EventType::None;


};
