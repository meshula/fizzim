//
// Color button for fltk
//

#include <fltk/Color.h>
#include <fltk/Group.h>
#include <fltk/Box.h>
#include <fltk/draw.h>

#include "FL_Color_Button.H"

using namespace fltk;


static fltk::Color myColor;

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
	setcolor(myColor);
	fillrect(x, y, w, h);
}

void ColorButton::draw() {
	myColor = fltk::color(rgba[0] * 255.0f, rgba[1] * 255.0f, rgba[2] * 255.0f);
	Button::draw(0, int(textsize())+2);
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
	rgba[0] = 0.0f;
	rgba[1] = 1.0f;
	rgba[2] = 0.0f;
	rgba[3] = 1.0f;
	style(default_style);
	set_flag(ALIGN_LEFT|ALIGN_INSIDE);
}
