#include "MouseButtonEvent.h"

MouseButtonEvent::MouseButtonEvent(ButtonType btn, ButtonState state,int mouseX,int mouseY) : btnCode(btn),btnState(state),mosPosX(mouseX),mosPosY(mouseY){
  type = EventType::MouseButton;
}

MouseButtonEvent::~MouseButtonEvent(){

}
