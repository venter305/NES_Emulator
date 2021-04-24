#pragma once

#include "panel.h"
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

class Text : public Panel{
	public:
		std::string text;
		float textScale;
		std::string font;
		float color[3];

		int yOffset = 0;
		int yTmpPos = -1;

		FT_Library library;
		FT_Face face;

		GLuint stringFbo,stringTex;
		
		GLuint tex1;

		Panel *sChar;
		
		Text(int,int,float,std::string,std::string,std::string="./GraphicsEngine/GUI/textVertShader",std::string="./GraphicsEngine/GUI/textFragShader");

		Text(int,int,float,std::string,std::string, GLFWwindow*,std::string="./GraphicsEngine/GUI/textVertShader",std::string="./GraphicsEngine/GUI/textFragShader");
		
		void setText(std::string);
		void setColor(float,float,float);
		void setTextSize(float);
		
		void drawText();
		void drawText(int,int,float,std::string,std::string);
};
