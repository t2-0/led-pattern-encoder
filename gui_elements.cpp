#include "gui_elements.h"
#include <iostream>

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
	*value = val;

	this->text = new char[text.size()] {};
	strcpy(this->text, text.c_str());
}

void IntBox::draw() {
	if (*value > max_val) {
		*value = max_val;
	}
	else if (*value < min_val) {
		*value = min_val;
	}

	if (GuiValueBox(bounds, text, value, min_val, max_val, edit_mode)) {
		edit_mode = !edit_mode;
	}
}

void Dropdownbox::draw() {
	if (GuiDropdownBox(bounds, text.c_str(), active, edit_mode)) {
		edit_mode = !edit_mode;
	}
}

bool Toggle::draw() {
	if (GuiToggle(bounds, text.c_str(), active)) {
		*active = !*active;
	}

	return *active;
}

RectangleEx::RectangleEx() {
	bounds = { 0.0f, 0.0f, 0.0f, 0.0f };
	color_factor = 1.0f;

	Color style_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
	color = adjust_brightness(style_color, color_factor);

	text_color = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
}

RectangleEx::RectangleEx(Rectangle bounds, string text, Color color, float color_factor = 1.0f) {
	this->bounds = bounds;
	this->text = text;
	this->color = color;

	this->color_factor = color_factor;

	text_color = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
}

void RectangleEx::draw() {
	Vector2 mouse_pos = GetMousePosition();

	Color style_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
	if (CheckCollisionPointRec(mouse_pos, bounds)) {
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			color = BLUE;
			pressed = true;
		}
		else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			color = adjust_brightness(style_color, color_factor);
			pressed = false;
		}
		else if (!pressed) {
			color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_PRESSED));
		}
	}
	else {
		if (!pressed) {
			color = adjust_brightness(style_color, color_factor);
		}
	}

	DrawRectangleRec(bounds, color);
	Font font = GuiGetFont();
	DrawTextEx(font, text.c_str(), { bounds.x, bounds.y }, font.baseSize, 0.0f, text_color);
}

CopyPanel::CopyPanel(Rectangle bounds, float color_factor) {
	this->bounds = bounds;

	Color style_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
	line_color = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL));

	color = adjust_brightness(style_color, color_factor);

	Rectangle button_bounds = bounds;
			  button_bounds.y += bounds.height + 10.0f;
			  button_bounds.width = 60.0f;
			  button_bounds.height = 30.0f;
	copy_btn = new Button{ button_bounds, "Copy" };

	Rectangle element_bounds = bounds;
			  element_bounds.x++;
			  element_bounds.y++;
			  element_bounds.width -= 2.0f;
			  element_bounds.height = 19.0f;
	
	for (size_t i = 0; i < elements.size(); i++) {
		elements[i].set_bounds(element_bounds);
		elements[i].set_color_factor(color_factor);

		element_bounds.y += 19.0f;
	}

	for (size_t i = 0, r = 1; i < idx_pos.size(); i++, r++) {
		Rectangle bounds = elements[r].get_bounds();
				  bounds.x -= 12.0f;
		Vector2 pos = { bounds.x, bounds.y };
		idx_pos[i] = pos;
	}
}

void CopyPanel::draw() {
	DrawRectangleRec(bounds, color);
	DrawRectangleLinesEx(bounds, 1.0f, line_color);
	
	for (size_t i = 0; i < elements.size(); i++) { elements[i].draw(); }
	if (copy_btn->draw()) { SetClipboardText(format_elements().c_str()); }

	Font font = GuiGetFont();
	for (size_t i = 0; i < idx_pos.size(); i++) {
		string str = to_string(i);
		DrawTextEx(font, str.c_str(), idx_pos[i], font.baseSize, 0.0f, BLUE);
	}
}

void CopyPanel::set_elements_text(vector<string> elements_s) {
	for (size_t i = 0; i < elements.size() && i < elements_s.size(); i++) {
		elements[i].set_text(elements_s[i]);
	}
}

string CopyPanel::format_elements() {
	string str;

	for (size_t i = 0; i < elements.size(); i++) {
		if (elements[i].is_pressed()) {
			str += elements[i].get_text() + "\n";
		}
	}

	if (str.empty()) {
		for (size_t i = 0; i < elements.size(); i++) {
			str += elements[i].get_text() + "\n";
		}
	}
	
	str.pop_back();
	return str;
}