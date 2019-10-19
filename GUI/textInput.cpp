#include "textInput.h"

using namespace std;

TextInput::TextInput(int x,int y,int w,int h,string vsName,string fsName): Panel(x,y,w,h,vsName,fsName){
	text = new Text(0,0,15,"","/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf");
	drawText();

}

void TextInput::drawText(){
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
	glViewport(0,0,200,50);

	glColor3f(1,1,1);
	glRectf(-1,-1,1,1);

	text->drawText();
	text->flipY();
	text->draw();

	glBindFramebuffer(GL_FRAMEBUFFER,currFramebuffer);
	glViewport(currViewport[0],currViewport[1],currViewport[2],currViewport[3]);
}
