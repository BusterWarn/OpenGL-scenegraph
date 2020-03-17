#ifndef __DRAW_TEXT_H__
#define __DRAW_TEXT_H__
#include <glm/vec4.hpp>

namespace vr
{
	class Text
	{
  public:
		static void drawText(float x, float y, const char *s);
		static void setFontSize(int size);
		static void setColor(const glm::vec4& color);
	};

}

#endif
