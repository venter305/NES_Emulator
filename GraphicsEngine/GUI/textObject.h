#pragma once

#include "panel.h"
#include "GUIElement.h"
#include <freetype2/ft2build.h>
#include <vector>

#include FT_FREETYPE_H

class Text : public GUIElement{
	public:

		int xPos;
		int yPos;

		int width;
		int height;

		std::string text;
		std::string font;

		float fontSize;

		float color[3] = {0.0,0.0,0.0};

		Text(int,int,float,std::string,std::string);
		~Text();

		void draw();
		void setText(std::string);
		void setPos(int,int);
		void setFontSize(float);
		void setTextColor(float,float,float);


private:
	std::string vertShader = "./GraphicsEngine/GUI/panelVertShader";
	std::string fragShader = "./GraphicsEngine/GUI/textFragShader";

	int penX,penY;
	int newLines = 0;

	std::vector<Panel*> characters;
	FT_Library library;
	FT_Face face;

	void addCharacter(char);
};
