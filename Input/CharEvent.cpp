#include "CharEvent.h"

CharEvent::CharEvent(unsigned int key) : charCode(key){
  type = EventType::Character;
}

CharEvent::~CharEvent(){

}
