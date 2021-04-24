#include "KeyEvent.h"

KeyEvent::KeyEvent(int key, KeyState state) : keyCode(key),keyState(state){
  type = EventType::Key;
}

KeyEvent::~KeyEvent(){

}
