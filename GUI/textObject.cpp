#include "textObject.h"
#include <iostream>

using namespace std;

Text::Text(int x, int y, float scale, string t, string fontPath,string vsName, string fsName) : Panel(x,y,6*scale,scale,vsName,fsName){
	textScale = scale;
	text = t;
	font = fontPath;
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	FT_Init_FreeType(&library);
	drawText();
}

void Text::setText(string t){
	text = t;
	drawText();
}

void Text::setColor(float c1,float c2,float c3){
	color[0] = c1;
	color[1] = c2;
	color[3] = c3;
	drawText();
}

void Text::setTextSize(float scale){
	textScale = scale;
	drawText();
}

void Text::drawText(){
	drawText(xPos,yPos,textScale,text,font);
}

void Text::drawText(int x,int y,float scale,string t,string fontPath){
	FT_New_Face(library,fontPath.c_str(),0,&face);
	FT_Set_Pixel_Sizes(face,0,scale);

	int fontScale = 1;

	int xSize = 1;
	int ySize = (scale)*1.25;
	
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
	glViewport(0,0,xSize,ySize);
	GLuint stringFbo;
	glGenFramebuffers(1,&stringFbo);
	glBindFramebuffer(GL_FRAMEBUFFER,stringFbo);
	GLuint stringTex;
	glGenTextures(1,&stringTex);
	glBindTexture(GL_TEXTURE_2D,stringTex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,xSize,ySize,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,stringTex,0);
	glBindTexture(GL_TEXTURE_2D,0);
	
	glUseProgram(shaderId);
	glUniform3f(glGetUniformLocation(shaderId,"textureColor"),color[0],color[1],color[2]);
	glUseProgram(0);

	glColor3f(0,0,0);
	glRectf(-1,-1,1,1);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint tex1;
	glGenTextures(1,&tex1);
	glBindTexture(GL_TEXTURE_2D,tex1);
	
	int xOffset = 1;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	for (c = t.begin();c != t.end();c++){
		glBindTexture(GL_TEXTURE_2D,tex1);
		FT_Load_Char(face,*c,FT_LOAD_RENDER);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RED,face->glyph->bitmap.width,face->glyph->bitmap.rows,0,GL_RED,GL_UNSIGNED_BYTE,face->glyph->bitmap.buffer);
		int charX = xOffset+(face->glyph->bitmap_left);
		int charY = (scale)-(face->glyph->bitmap_top/fontScale);
		Panel sChar(charX,charY,(face->glyph->bitmap.width)/fontScale,(face->glyph->bitmap.rows)/fontScale);
		xOffset += ((face->glyph->advance.x/fontScale)/64);
		sChar.setTexture(tex1);
		sChar.draw();
	}
	
	glBindTexture(GL_TEXTURE_2D,currTexture);
	glBindFramebuffer(GL_FRAMEBUFFER,currFramebuffer);

	tex = stringTex;

	glViewport(currViewport[0],currViewport[1],currViewport[2],currViewport[3]);
	
	FT_Done_Face(face);
}


