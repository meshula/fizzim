
#ifndef fltk_ColorButton_h
#define fltk_ColorButton_h

#include <fltk/Button.h>

namespace fltk {

	class FL_API ColorButton : public Button {
	public:
		ColorButton(int x,int y,int w,int h,const char *l = 0);
		static NamedStyle* default_style;

		void color(float r, float g, float b, float a = 1.0f);

	protected:
		virtual void draw();
		float rgba[4];
	};

}

#endif
