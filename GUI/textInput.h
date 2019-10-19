#ifndef TEXTINPUT_H_
#define TEXTINPUT_H_

#include "panel.h"
#include "textObject.h"

class TextInput: public Panel{
	public:

		Text *text;
		std::string currText;
	
		TextInput(int,int,int,int,std::string="./GUI/panelVertShader",std::string="./GUI/panelFragShader");
		void drawText();
};

#endif
