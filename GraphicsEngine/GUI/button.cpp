#include "button.h"
#include <iostream>

using namespace std;

Button::Button(int x,int y, int w, int h, void (*action_func)(Button*),string vsName, string fsName): Button(x,y,w,h,action_func,NULL){
}

Button::Button(int x,int y, int w, int h, void (*action_func)(Button*),GLFWwindow *window,string vsName, string fsName): Panel(x,y,w,h,window,vsName,fsName){
	id = -1;

	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(context);

	//Text
	text = new Text(xPos,yPos,12,"", "/usr/share/fonts/TTF/DejaVuSerif.ttf");
	textAlignment = TextAlignment::Center;

	action = action_func;

	glfwMakeContextCurrent(currWindow);
}

Button::~Button(){
	delete text;
}

void Button::draw(){
	Panel::draw();
	text->draw();
}

void Button::setText(std::string txt){
	text->setText(txt);
	setTextPos();
}

void Button::setTextSize(int size){
	text->setFontSize(size);
	setTextPos();
}

void Button::setTextColor(float r,float g, float b){
	text->setTextColor(r,g,b);
}

void Button::setTextAlignment(TextAlignment tAlign){
	textAlignment = tAlign;
	setTextPos();
}

void Button::setTextPos(int xOffset,int yOffset){
	int textPosX = xOffset;
	int textPosY = yPos+(height/2)-(text->height/2)+yOffset;
	switch(textAlignment){
		case TextAlignment::Center:
			textPosX += xPos+(width/2)-(text->width/2);
			break;
		case TextAlignment::Left:
			textPosX += xPos;
			break;
	}
	text->setPos(textPosX,textPosY);
}

//Change Background color
void Button::setBackgroundColor(float c1,float c2,float c3){
	color[0] = c1;
	color[1] = c2;
	color[2] = c3;
	setColor(color[0],color[1],color[2],1.0f);
}

void Button::clickAction(int xPos,int yPos){
	if (checkBoundingBox(xPos,yPos))
		action(this);
}
