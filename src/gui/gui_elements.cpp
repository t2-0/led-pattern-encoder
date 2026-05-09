#include "gui_elements.h"
#include <iostream>

Font TextEx::font = GuiGetFont();

Color adjust_brightness(Color c, float factor) {
	return { static_cast<unsigned char>(fmin(c.r * factor, 255)),
			 static_cast<unsigned char>(fmin(c.g * factor, 255)),
			 static_cast<unsigned char>(fmin(c.b * factor, 255)),
			 c.a };
}

IntBox::IntBox(Rectangle bounds, string text, int min_val, int max_val, int val = 0) :
	bounds{ bounds }, min_val{ min_val }, max_val{ max_val }, value{ val } {
	this->text = new char[text.size()] {};
	strcpy(this->text, text.c_str());
}

void IntBox::draw() {
	if (value > max_val) {
		value = max_val;
	}
	else if (value < min_val) {
		value = min_val;
	}

	if (GuiValueBox(bounds, text, &value, min_val, max_val, edit_mode)) {
		edit_mode = !edit_mode;
	}
}

void TextEx::init_font() {
	TextEx::font = GuiGetFont();
}

void TimedText::draw() {
	if (triggered) {
		float delta_time = GetFrameTime();
		timer += delta_time;

		if (timer >= lifetime) {
			timer = 0.0f;
			triggered = false;
		}

		text.draw();
	}
}

ScrollBar::ScrollBar(Rectangle bounds, float min_scroll, float max_scroll, float font_size) {
	this->bounds = bounds;
	this->min_scroll = min_scroll;
	this->max_scroll = max_scroll;

	scroll_val = 0;
}

void ScrollBar::draw() {
	scroll_val = GuiScrollBarW(bounds, scroll_val, min_scroll, max_scroll);
}