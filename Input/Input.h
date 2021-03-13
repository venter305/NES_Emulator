#pragma once

#include <GLFW/glfw3.h>

#include "Event.h"
#include "KeyEvent.h"
#include "CharEvent.h"
#include "MouseButtonEvent.h"

class Input{

public:

  GLFWwindow *window;

  Input();
  ~Input();

  void (*onEvent)(Event&);
  void GetMousePos(double&,double&);

private:
};
