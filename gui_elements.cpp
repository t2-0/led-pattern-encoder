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

TextEx::TextEx(Vector2 pos, string text, Color color) {
	this->text = text;
	this->pos = pos;
	this->color = color;
}

void TextEx::init_font() {
	TextEx::font = GuiGetFont();
}

void TextEx::draw() {
	DrawTextEx(font, text.c_str(), pos, font.baseSize, 0.0f, color);
}

ostream& operator<<(ostream& os, const TextEx& t) {
	os << t.text;
	return os;
}

ScrollBar::ScrollBar(Rectangle bounds, float min_scroll, float font_size, size_t item_count) {
	this->bounds = bounds;
	this->min_scroll = min_scroll;
	this->max_scroll = 10 + item_count * (font_size + 10);

	scroll_val = 0;
}

void ScrollBar::draw() {
	scroll_val = GuiScrollBarW(bounds, scroll_val, min_scroll, max_scroll);
}