#include "textInput.h"
#include <iostream>

using namespace std;


TextInput::TextInput(int x,int y,int w,int h,string t,string vsName,string fsName): TextInput(x,y,w,h,NULL,t,vsName,fsName){
}

TextInput::TextInput(int x,int y,int w,int h,GLFWwindow *window,string t,string vsName,string fsName): Panel(x,y,w,h,window,vsName,fsName){
	//Init
	text = new Text(0,0,15,t,"/usr/share/fonts/TTF/DejaVuSerif.ttf");
	setEnabled(false);
	drawText();
		
	prevInTime = 0;
	inputTimer = 0;
	inTimeMax = 0.12;
	inputMask = "abcdefghijklmnopqrstuvwxyz.0123456789/";
}

//Render the text
void TextInput::drawText(){
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(context);
	//Set up the Texture
	int currFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currFramebuffer);
	int currTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currTexture);
	GLuint fbo;
	glGenFramebuffers(1,&fbo);
	glBindFramebuffer(GL_FRAMEBUFFER,fbo);
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,tex,0);
	glBindTexture(GL_TEXTURE_2D,currTexture);

	int currViewport[4];
	glGetIntegerv(GL_VIEWPORT,currViewport);
	
	//Background
	glColor3f(1,1,1);
	glRecti(0,0,width,height);

	//Text
	text->drawText();
	text->draw();

	glBindFramebuffer(GL_FRAMEBUFFER,currFramebuffer);
	glViewport(currViewport[0],currViewport[1],currViewport[2],currViewport[3]);
	glfwMakeContextCurrent(currWindow);
}

void TextInput::setText(string newText){
	text->setText(newText);
	drawText();
}

void TextInput::addCharacter(char newChar){
	setText(text->text+newChar);
}

void TextInput::removeCharacter(){
	if (text->text.empty())
		return;
	text->text.pop_back();
	drawText();
} 

void TextInput::setEnabled(bool state){
	enabled = state;
	if (state)
		setColor(enabledColor[0],enabledColor[1],enabledColor[2]);
	else
		setColor(disabledColor[0],disabledColor[1],disabledColor[2]);
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


