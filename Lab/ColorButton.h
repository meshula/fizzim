
#ifndef fltk_ColorButton_h
#define fltk_ColorButton_h

#include <fltk/Button.h>

namespace fltk {

	class FL_API ColorButton : public Button {
	public:
		ColorButton(int x,int y,int w,int h,const char *l = 0);
		static NamedStyle* default_style;

		void color(float r, float g, float b, float a = 1.0f);

		float red() const { return rgba[0]; }
		float green() const { return rgba[1]; }
		float blue() const { return rgba[2]; }
		float alpha() const { return rgba[3]; }

		void red(float r) { rgba[0] = r; }
		void green(float g) { rgba[1] = g; }
		void blue(float b) { rgba[2] = b; }
		void alpha(float a) { rgba[3] = a; }

	protected:
		virtual void draw();
		float rgba[4];
	};

}

#endif
