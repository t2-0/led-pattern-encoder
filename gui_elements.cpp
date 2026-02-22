#include "gui_elements.h"
#include <iostream>

Font TextEx::font = GuiGetFont();

Color adjust_brightness(Color c, float factor) {
	return { static_cast<unsigned char>(fmin(c.r * factor, 255)),
			 static_cast<unsigned char>(fmin(c.g * factor, 255)),
			 static_cast<unsigned char>(fmin(c.b * factor, 255)),
			 c.a };
}

IntBox::IntBox(Rectangle bounds, string text, int min_val, int max_val, int val = 0) {
	this->bounds  = bounds;
	this->min_val = min_val;
	this->max_val = max_val;
	value = val;

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

CopyPanel::CopyPanel(Rectangle bounds, float color_factor) {
	this->bounds = bounds;
	this->color_factor = color_factor;
	Color style_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
	line_color = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL));

	color = adjust_brightness(style_color, color_factor);

	Rectangle button_bounds = bounds;
			  button_bounds.y += bounds.height + 10.0f;
			  button_bounds.width = 60.0f;
			  button_bounds.height = 30.0f;
	copy_btn = new Button{ button_bounds, "Copy" };

	Rectangle scrollbar_bounds = bounds;
			  scrollbar_bounds.x += bounds.width + 5;
			  scrollbar_bounds.width = 10;

	float font_size = GuiGetFont().baseSize;
	scrollbar = new ScrollBar{ scrollbar_bounds, 0, font_size, elements.size() };
}

void CopyPanel::draw() {
	DrawRectangleRec(bounds, color);
	DrawRectangleLinesEx(bounds, 1.0f, line_color);

	BeginScissorMode(bounds.x - 20, bounds.y, bounds.width + 20, bounds.height);
	Font font = GuiGetFont();
	int y_offset = 1;

	Rectangle element_bounds = bounds;
			  element_bounds.x++;
			  element_bounds.y -= 18;

	for (size_t r = 0; r < elements.size(); r++) {
		for (size_t c = 0; c < elements[r].size(); c++) {
			if (elements.size() > 1) {
				element_bounds.y = bounds.y + y_offset + r * font.baseSize - scrollbar->get_scroll_val();
			}
			else {
				element_bounds.y += 19.0f;
			}

			elements[r][c].set_pos({ element_bounds.x, element_bounds.y });
			elements[r][c].draw();
		}
	}
	for (size_t i = 0; i < idx_v.size(); i++) {
		if (elements.size() > 1) {
			Vector2 pos = idx_v[i].get_pos();
			pos.y = bounds.y + y_offset + (i + 1) * font.baseSize - scrollbar->get_scroll_val();
			idx_v[i].set_pos(pos);
		}
		idx_v[i].draw();
	}
	EndScissorMode();

	copy_btn->draw();
	if(copy_btn->clicked()) { SetClipboardText(format_elements().c_str()); }
	if (elements.size() > 1) { scrollbar->draw(); }
}

void CopyPanel::set_elements_text(const vector<vector<string>> &elements_s) {
	cout << "set" << endl;
	elements.clear();
	idx_v.clear();
	Rectangle idx_bounds = bounds;
			  idx_bounds.x -= 12.0f;
			  idx_bounds.y += 19.0f;
			  idx_bounds.width -= 2.0f;
			  idx_bounds.height = 19.0f;
	if (elements_s.size() == 1) {
		bounds = { 385.0f, 40.0f, 150.0f, 192.0f };

		for (size_t i = 0; i < 8; i++) {
			Vector2 pos = { idx_bounds.x, idx_bounds.y };
			idx_v.push_back({ { pos }, to_string(i), BLUE });
			idx_bounds.y += 19.0f;
		}
	}
	else {
		bounds = { 385.0f, 40.0f, 350.0f, 192.0f };
		Rectangle scrollbar_bounds = bounds;
				  scrollbar_bounds.x += bounds.width + 5;
				  scrollbar_bounds.width = 10;
				  scrollbar->set_bounds(scrollbar_bounds);
		for (size_t i = 0; i + 2 < elements_s.size(); i++) {
			Vector2 pos = { idx_bounds.x, idx_bounds.y };
			if (i >= 10) { pos.x -= font.baseSize / 2; }
			idx_v.push_back({ pos, to_string(i), BLUE });
			idx_bounds.y += 19.0f;
		}
	}

	Color element_color = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
	for (size_t r = 0; r < elements_s.size(); r++) {
		elements.push_back({});
		for (size_t c = 0; c < elements[r].size() && c < elements_s[r].size(); c++) {
			elements[r][c].set_text(elements_s[r][c]);
			elements[r][c].set_color(element_color);
		}
	}

	float scrollbar_max = y_offset + elements.size() * font.baseSize - bounds.height;
	if (scrollbar_max != scrollbar->get_max_scroll()) {
		scrollbar->set_max_scroll(scrollbar_max);
		scrollbar->set_scroll_val(scrollbar_max); 
	}
}

string CopyPanel::format_elements() {
	string str;

	if (elements.size() == 1) {
		for (size_t r = 0; r < elements.size(); r++) {
			for (size_t c = 0; c < elements[r].size(); c++) {
				str += elements[r][c].get_text() + "\n";
			}
		}
	}
	else {
		for (size_t r = 0; r < elements.size(); r++) {
			for (size_t c = 0; c < elements[r].size(); c++) {
				str += elements[r][c].get_text();
			}
			str += "\n";
		}
	}

	str.pop_back();
	return str;
}