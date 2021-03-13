#include "textInput.h"
#include <iostream>

using namespace std;


TextInput::TextInput(int x,int y,int w,int h,string t,string vsName,string fsName): TextInput(x,y,w,h,NULL,t,vsName,fsName){
}

TextInput::TextInput(int x,int y,int w,int h,GLFWwindow *window,string t,string vsName,string fsName): Button(x,y,w,h,NULL,window,vsName,fsName){
	//Init
	//text = new Text(0,0,15,t,"/usr/share/fonts/TTF/DejaVuSerif.ttf");
	setEnabled(false);
	//drawText();

	setText(t);


	prevInTime = 0;
	inputTimer = 0;
	inTimeMax = 0.12;
	inputMask = "abcdefghijklmnopqrstuvwxyz.0123456789/";
}

void TextInput::addCharacter(char newChar){
	setText(text->text+newChar);
}

void TextInput::removeCharacter(){
	if (text->text.empty())
		return;
	text->text.pop_back();
	setText(text->text);
}

void TextInput::setEnabled(bool state){
	enabled = state;
	if (state)
		setBackgroundColor(enabledColor[0],enabledColor[1],enabledColor[2]);
	else
		setBackgroundColor(disabledColor[0],disabledColor[1],disabledColor[2]);
}

void TextInput::setEnabledColor(float color[3]){
	enabledColor[0] = color[0];
	enabledColor[1] = color[1];
	enabledColor[2] = color[2];
	setEnabled(enabled);
}

void TextInput::setDisabledColor(float color[3]){
	disabledColor[0] = color[0];
	disabledColor[1] = color[1];
	disabledColor[2] = color[2];
	setEnabled(enabled);
}

void TextInput::setStateColors(float eColor[3], float dColor[3]){
	setEnabledColor(eColor);
	setDisabledColor(dColor);
}

void TextInput::clickAction(int posX,int posY){
	setEnabled(checkBoundingBox(posX,posY));
}
