#ifndef TEXTPANEL_H_
#define TEXTPANEL_H_

#include "panel.h"
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

class Text : public Panel{
	public:
		std::string text;
		float textScale;
		std::string font;
		float color[3];

		FT_Library library;
		FT_Face face;

		GLuint stringFbo,stringTex;

		Panel *sChar;
		
		Text(int,int,float,std::string,std::string,std::string="./GraphicsEngine/GUI/textVertShader",std::string="./GraphicsEngine/GUI/textFragShader");

		Text(int,int,float,std::string,std::string, GLFWwindow*,std::string="./GraphicsEngine/GUI/textVertShader",std::string="./GraphicsEngine/GUI/textFragShader");
		
		void setText(std::string);
		void setColor(float,float,float);
		void setTextSize(float);
		
		void drawText();
		void drawText(int,int,float,std::string,std::string);
};

#endif
