#ifndef BUTTON_H_
#define BUTTON_H_

#include "panel.h"
#include "textObject.h"

class Button : public Panel{
	public:
		Text *text;

		GLuint textFbo;
		GLuint textTexture;
	
		float color[3];
		
		Button(int,int,int,int,void(*)(Button*),std::string="./GUI/panelVertShader",std::string="./GUI/panelFragShader");
		void (*action)(Button*);

		void drawText();
		void setBackgroundColor(float,float,float);
};

#endif
