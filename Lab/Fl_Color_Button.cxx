//
// Color button for fltk
//

#include <fltk/Color.h>
#include <fltk/Group.h>
#include <fltk/Box.h>
#include <fltk/draw.h>

#include "FL_Color_Button.H"

using namespace fltk;




void ColorButton::color(float r, float g, float b, float a) {
	rgba[0] = r; rgba[1] = g; rgba[2] = b; rgba[3] = a;
}

static void default_glyph(int glyph,
						  int x,int y,int w,int h,
						  const Style* style, Flags flags)
{
	Box* box = style->box();
	box->draw(x, y, w, h, style, flags|OUTPUT);
	box->inset(x, y, w, h);
	if (flags & VALUE) {
//		fltk::Color color = fltk::color(rgba[0] * 255.0f, rgba[1] * 255.0f, rgba[2] * 255.0f);
//		setcolor(color, flags);
//		fillrect(x+w/2-1,y+h/2-1,2,2);
		//setcolor(inactive(style->textcolor(), flags));
		//if (h < 4) {fillrect(x+w/2-1,y+h/2-1,2,2); return;}
		x += 1;
		w = h - 2;
		int d1 = w/3;
		int d2 = w-d1;
		y += (h+d2)/2-d1-2;
		for (int n = 0; n < 3; n++, y++) {
			drawline(x, y, x+d1, y+d1);
			drawline(x+d1, y+d1, x+w-1, y+d1-d2+1);
		}
	}
}

void ColorButton::draw() {
	Button::draw(0, int(textsize())+2);
	fltk::Color color = fltk::color(255,0,0);//rgba[0] * 255.0f, rgba[1] * 255.0f, rgba[2] * 255.0f);
	setcolor(color);
	fillrect(x()+w()/2-1,y()+h()/2-1,2,2);
}

static void revert(Style* s) {
	s->buttonbox_ = UP_BOX;
	//s->box_ = DOWN_BOX;
	s->glyph_ = ::default_glyph;
}
static NamedStyle style("Color_Button", revert, &ColorButton::default_style);
NamedStyle* ColorButton::default_style = &::style;

ColorButton::ColorButton(int x, int y, int w, int h, const char *l)
: Button(x, y, w, h, l)
{
	style(default_style);
	type(TOGGLE);
	set_flag(ALIGN_LEFT|ALIGN_INSIDE);
}
