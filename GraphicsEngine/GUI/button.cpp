#include "button.h"
#include <iostream>

using namespace std;

Button::Button(int x,int y, int w, int h, void (*action_func)(Button*),string vsName, string fsName): Button(x,y,w,h,action_func,NULL){
}

Button::Button(int x,int y, int w, int h, void (*action_func)(Button*),GLFWwindow *window,string vsName, string fsName): Panel(x,y,w,h,window,vsName,fsName){
	color[0] = 1;
	color[1] = 1;
	color[2] = 1;
	
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(context);

	//Init texture
	glGenFramebuffers(1,&textFbo);
	glBindFramebuffer(GL_FRAMEBUFFER,textFbo);
	glGenTextures(1,&textTexture);
	glBindTexture(GL_TEXTURE_2D,textTexture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textTexture,0);
	glBindTexture(GL_TEXTURE_2D,0);

	int currViewport[4];
	glGetIntegerv(GL_VIEWPORT,currViewport);
	
	//Background
	glColor3f(1,1,1);
	glRecti(0,0,width,height);
	
	//Text
	text = new Text(0,0,12,"Button", "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf");
	text->draw();

	glBindFramebuffer(GL_FRAMEBUFFER,0);	
	
	glViewport(currViewport[0],currViewport[1],currViewport[2],currViewport[3]);

	tex = textTexture;
	action = action_func;
	
	glfwMakeContextCurrent(currWindow);
}

//Render the text
void Button::drawText(){
	
	glBindFramebuffer(GL_FRAMEBUFFER,textFbo);
	int currViewport[4];
	glGetIntegerv(GL_VIEWPORT,currViewport);
	
	//Background
	glColor3f(color[0],color[1],color[2]);
	glRecti(0,0,width,height);
	//Text
	text->drawText();
	text->draw();

	glBindFramebuffer(GL_FRAMEBUFFER,0);	
	
	glViewport(currViewport[0],currViewport[1],currViewport[2],currViewport[3]);

	tex = textTexture;
}

//Change Background color
void Button::setBackgroundColor(float c1,float c2,float c3){
	color[0] = c1;
	color[1] = c2;
	color[2] = c3;
	
	drawText();
}
