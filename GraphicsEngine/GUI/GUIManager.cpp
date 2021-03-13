#include "GUIManager.h"
#include <iostream>

using namespace std;

GUIManager::GUIManager(){

}

void GUIManager::drawElements(){
	map<int,std::shared_ptr<GUIElement>>::iterator it;
	for (it = elements.begin();it != elements.end();it++){
		it->second->draw();
	}
}

int GUIManager::addElement(std::shared_ptr<GUIElement> element,int id){
	if (id == -1)
		id = elements.size();
	element->SetId(id);
	elements.insert({id,element});
	return id;
}
