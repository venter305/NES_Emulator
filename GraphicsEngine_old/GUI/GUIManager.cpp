#include "GUIManager.h"
#include <iostream>

using namespace std;

GUIManager::GUIManager(){
	
}

void GUIManager::drawElements(){
	map<int,Panel*>::iterator it;
	for (it = elements.begin();it != elements.end();it++)
		it->second->draw();
}

int GUIManager::addElement(Panel *p,int id){
	if (id == -1)
		id = elements.size();
	p->id = id;
	elements.insert({id,p});
	return id;
}
