#pragma once

#include <map>
#include "panel.h"

class GUIManager{
	public:
		std::map<int,Panel*> elements;

		GUIManager();
		void drawElements();
		int addElement(Panel*,int=-1);

};

