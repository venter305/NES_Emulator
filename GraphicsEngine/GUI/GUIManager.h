#pragma once

#include <map>
#include <memory>
#include "GUIElement.h"

class GUIManager{
	public:
		std::map<int,std::shared_ptr<GUIElement>> elements;

		GUIManager();
		void drawElements();
		int addElement(std::shared_ptr<GUIElement>,int=-1);

};
