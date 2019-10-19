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
		
		Text(int,int,float,std::string,std::string,std::string="./GUI/textVertShader",std::string="./GUI/textFragShader");
		
		void setText(std::string);
		void setColor(float,float,float);
		void setTextSize(float);
		
		void drawText();
		void drawText(int,int,float,std::string,std::string);
};

#endif
