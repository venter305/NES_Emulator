#ifndef TEXTINPUT_H_
#define TEXTINPUT_H_

#include "panel.h"
#include "textObject.h"

class TextInput: public Panel{
	public:

		Text *text;
		std::string currText;
	
		TextInput(int,int,int,int,std::string="./GraphicsEngine/GUI/panelVertShader",std::string="./GraphicsEngine/GUI/panelFragShader");
	
		TextInput(int,int,int,int,GLFWwindow*,std::string="./GraphicsEngine/GUI/panelVertShader",std::string="./GraphicsEngine/GUI/panelFragShader");
		void drawText();
};

#endif
