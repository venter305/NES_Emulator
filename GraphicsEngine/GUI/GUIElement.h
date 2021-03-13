#pragma once

class GUIElement{
public:
  virtual void draw() = 0;

  void SetId(int _id){id = _id;}

protected:
  int id;
};
