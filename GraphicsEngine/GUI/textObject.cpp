#include "textObject.h"
#include <iostream>

using namespace std;

Text::Text(int x, int y, float scale, string t, string fontPath, GLFWwindow *window,string vsName, string fsName) : Panel(x,y,6*scale,scale,window,vsName,fsName){
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(context);
	//Init
	textScale = scale;
	text = t;
	font = fontPath;
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	FT_Init_FreeType(&library);
	sChar = new Panel(0,0,10,10);

	int currViewport[4];
	glGetIntegerv(GL_VIEWPORT,currViewport);
	int currFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currFramebuffer);
	int currTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currTexture);

	glGenFramebuffers(1,&stringFbo);
	glBindFramebuffer(GL_FRAMEBUFFER,stringFbo);
	glGenTextures(1,&stringTex);
	glBindTexture(GL_TEXTURE_2D,stringTex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,100,100,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,stringTex,0);
	glBindTexture(GL_TEXTURE_2D,currTexture);
	glBindFramebuffer(GL_FRAMEBUFFER,currFramebuffer);

	drawText();
		
	glfwMakeContextCurrent(currWindow);
}

Text::Text(int x, int y, float scale, string t, string fontPath,string vsName, string fsName) : Text(x,y,scale,t,fontPath,NULL,vsName,fsName){
}

//Set the text
void Text::setText(string t){
	text = t;
	drawText();
}

//Set the color of the text
void Text::setColor(float c1,float c2,float c3){
	color[0] = c1;
	color[1] = c2;
	color[2] = c3;
	drawText();
}

//Set the text size
void Text::setTextSize(float scale){
	textScale = scale;
	drawText();
}

//Draw the text
void Text::drawText(){
	drawText(xPos,yPos,textScale,text,font);
}

//Draw the text
void Text::drawText(int x,int y,float scale,string t,string fontPath){
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(context);
	FT_New_Face(library,fontPath.c_str(),0,&face);
	FT_Set_Pixel_Sizes(face,0,scale);

	int fontScale = 1;

	int xSize = 1;
	int ySize = (scale)*1.25;
	
	//Calculate the X size
	string::const_iterator c;
	for (c = t.begin();c != t.end();c++){
		FT_Load_Char(face,*c,FT_LOAD_RENDER);
		xSize += ((face->glyph->advance.x/fontScale)/64);
		
	}
	setSize(xSize,ySize);
	
	int currViewport[4];
	glGetIntegerv(GL_VIEWPORT,currViewport);
	int currFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currFramebuffer);
	int currTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currTexture);
	
	//Setup texture
	glBindFramebuffer(GL_FRAMEBUFFER,stringFbo);
	glBindTexture(GL_TEXTURE_2D,stringTex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,xSize,ySize,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
	glBindTexture(GL_TEXTURE_2D,0);

	glUseProgram(shaderId);
	glUniform3f(glGetUniformLocation(shaderId,"textureColor"),color[0],color[1],color[2]);
	glUseProgram(0);
	
	//Background
	glColor3f(0,0,0);
	glRecti(0,0,xSize,ySize);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Setup character texture
	GLuint tex1;
	glGenTextures(1,&tex1);
	glBindTexture(GL_TEXTURE_2D,tex1);
	
	int xOffset = 1;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	int charX = 0;
	int charY = 0;
	//Draw Character
	for (c = t.begin();c != t.end();c++){
		FT_Load_Char(face,*c,FT_LOAD_RENDER);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RED,face->glyph->bitmap.width,face->glyph->bitmap.rows,0,GL_RED,GL_UNSIGNED_BYTE,face->glyph->bitmap.buffer);
		charX = xOffset+(face->glyph->bitmap_left);
		charY = (scale)-(face->glyph->bitmap_top/fontScale);
		sChar->setPos(charX,charY);
		sChar->setSize((face->glyph->bitmap.width)/fontScale,(face->glyph->bitmap.rows)/fontScale);
		xOffset += ((face->glyph->advance.x/fontScale)/64);
		sChar->setTexture(tex1);
		sChar->draw();
	}
	glBindTexture(GL_TEXTURE_2D,currTexture);
	glBindFramebuffer(GL_FRAMEBUFFER,currFramebuffer);

	tex = stringTex;

	glViewport(currViewport[0],currViewport[1],currViewport[2],currViewport[3]);
	
	FT_Done_Face(face);
	flipY();
	glfwMakeContextCurrent(currWindow);
}


