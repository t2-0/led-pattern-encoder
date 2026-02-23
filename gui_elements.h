#pragma once
#include "raylib_raygui.h"
#include <string>
#include <vector>
#include <array>

using namespace std;

Color adjust_brightness(Color c, float factor);

class IntBox {
public:
	IntBox(Rectangle bounds, string text, int min_val, int max_val, int val);

	void draw();

	int get_val() { return value; }
private:
	Rectangle bounds;

	char* text = nullptr;
	int value = 0;
	int min_val = 0;
	int max_val = 0;
	bool edit_mode = false;
};

class Dropdownbox {
public:
	Dropdownbox(Rectangle bounds, string text) { this->bounds = bounds; this->text = text; }
	void draw() {if (GuiDropdownBox(bounds, text.c_str(), &active, edit_mode)) { edit_mode = !edit_mode; } }

	int get_active() { return active; }

	bool is_updated() { return active != old_active; }
	void update() { old_active = active; }

private:
	Rectangle bounds;
	string text;
	int active = 0;
	int old_active = 0;
	bool edit_mode = false;
};

class Button {
public:
	Button(Rectangle bounds, string text) { this->bounds = bounds; this->text = text; }
	void draw() { clicked_b = GuiButton(bounds, text.c_str()); }

	bool clicked() {
		if (clicked_b) {
			clicked_b = false;
			return true;
		}
		return false;
	}
private:
	Rectangle bounds;
	string text;

	bool clicked_b = false;
};

class Toggle {
public:
	Toggle(Rectangle bounds, string text) { this->bounds = bounds; this->text = text; }

	void draw() { if (GuiToggle(bounds, text.c_str(), &active)) { active = !active; } }
	bool is_active()  { return active; }
	bool is_updated() { return active != active_old; }
	void update() { active_old = active; }

	void set_active(bool active) { this->active = active; }
private:
	Rectangle bounds;
	string text;

	bool active = false;
	bool active_old = false;
};

class ToggleGroup {
public:
	ToggleGroup(Rectangle bounds, string text) { this->bounds = bounds; this->text = text; }

	void draw() { GuiToggleGroup(bounds, text.c_str(), &active); }
	bool is_updated() { return active != active_old; }
	void update() { active_old = active; }

	int get_active() { return active; }
private:
	Rectangle bounds;
	string text;
	int active = 0;
	int active_old = 0;
};

class Line {
public:
	Line(Vector2 start_pos, Vector2 end_pos, float thick, Color color) {
		this->start_pos = start_pos;
		this->end_pos = end_pos;
		this->thick = thick;
		this->color = color;
	}

	void draw() { DrawLineEx(start_pos, end_pos, thick, color); }
private:
	Vector2 start_pos;
	Vector2 end_pos;

	Color color;

	float thick;
};

class TextEx {
public:
	TextEx() { color = RAYWHITE; pos = { 0.0f, 0.0f }; }
	TextEx(Vector2 pos, string text, Color color);

	void draw();

	void set_text(string text)  { this->text = text; }
	void set_pos(Vector2 pos)   { this->pos = pos; }
	void set_color(Color color) { this->color = color; }

	string get_text() { return text; }
	Vector2 get_pos() { return pos; }

	friend ostream& operator<<(ostream& os, const TextEx& t);

	static void init_font();
private:
	string text;
	Vector2 pos;

	static Font font;
	Color color;
};

class ScrollBar {
public:
	ScrollBar(Rectangle bounds, float min_scroll, float font_size, size_t item_count);

	void draw();

	void set_bounds(Rectangle bounds) { this->bounds = bounds; }
	void set_max_scroll(float max_scroll) { this->max_scroll = max_scroll; }
	void set_scroll_val(float scroll_val) { this->scroll_val = scroll_val; }

	float get_max_scroll() { return max_scroll; }
	float get_scroll_val() { return scroll_val; }
private:
	Rectangle bounds;

	float min_scroll;
	float max_scroll;
	float scroll_val;
};

class CopyPanel {
public:
	CopyPanel(Rectangle bounds, float color_factor);

	void draw();

	void set_elements_text(const vector<vector<string>>& elements_s);

	string format_elements();
private:
	Rectangle bounds;
	ScrollBar* scrollbar = nullptr;

	Color color;
	Color line_color;
	vector<array<TextEx, 10>> elements;
	vector<TextEx> idx_v;

	TextEx container_s;

	Button* copy_btn = nullptr;

	float color_factor = 1.0f;

	Font font = GuiGetFont();
	int y_offset = 1;
};