#pragma once
#include "raylib_raygui.h"
#include <string>
#include <vector>
#include <array>

using namespace std;

Color adjust_brightness(Color c, float factor);

class Dropdownbox {
public:
	Dropdownbox(Rectangle bounds, string text) { this->bounds = bounds; this->text = text; }
	void draw();

	int get_active() { return *active; }
private:
	Rectangle bounds;
	string text;
	int* active = new int(0);
	bool edit_mode = false;
};

class Button {
public:
	Button(Rectangle bounds, string text) { this->bounds = bounds; this->text = text; }
	int draw() { return GuiButton(bounds, text.c_str()); }
private:
	Rectangle bounds;
	string text;
};

class Toggle {
public:
	Toggle(Rectangle bounds, string text) { this->bounds = bounds; this->text = text; }
	bool draw();
private:
	Rectangle bounds;
	string text;

	bool* active = new bool(false);
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

class RectangleEx {
public:
	RectangleEx();
	RectangleEx(Rectangle bounds, string text, Color color, float color_factor);

	void draw();

	void set_color(Color color) { this->color = color; }
	void set_text(string text) { this->text = text; }
	void set_bounds(Rectangle bounds) { this->bounds = bounds; }
	void set_color_factor(float color_factor) { this->color_factor = color_factor; }

	bool is_pressed() { return pressed; }

	string get_text() { return text; }
	Rectangle get_bounds() { return bounds; }
private:
	Rectangle bounds;
	string text;

	bool pressed = false;

	Color color;
	Color text_color;

	float color_factor;
};

class CopyPanel {
public:
	CopyPanel(Rectangle bounds, float color_factor);

	void draw();

	void set_elements_text(vector<string> elements_s);

	string format_elements();
private:
	Rectangle bounds;

	Color color;
	Color line_color;
	// use 2D vector?
	array<RectangleEx, 10> elements;
	array<Vector2, 8> idx_pos = {};

	Button* copy_btn = nullptr;
};