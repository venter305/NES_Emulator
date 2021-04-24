#include "Input.h"

Input::Input(){

}

Input::~Input(){

}

void Input::GetMousePos(double &xPos, double &yPos){

  int width,height;
  glfwGetWindowSize(window,&width,&height);

  double tmpYPos;
  glfwGetCursorPos(window,&xPos,&tmpYPos);

  yPos = tmpYPos-height;
  if (yPos < 0) yPos *= -1;
}
