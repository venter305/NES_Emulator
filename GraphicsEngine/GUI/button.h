#ifndef BUTTON_H_
#define BUTTON_H_

#include "panel.h"
#include "textObject.h"

class Button : public Panel{
	public:
		//Text
		Text *text;
		
		GLuint textFbo;
		GLuint textTexture;
	
		float color[3];
		
		Button(int,int,int,int,void(*)(Button*),std::string="./GraphicsEngine/GUI/panelVertShader",std::string="./GraphicsEngine/GUI/panelFragShader");

		Button(int,int,int,int,void(*)(Button*),GLFWwindow*,std::string="./GraphicsEngine/GUI/panelVertShader",std::string="./GraphicsEngine/GUI/panelFragShader");
		void (*action)(Button*);

		void drawText();
		void setBackgroundColor(float,float,float);
};

#endif
