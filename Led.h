#pragma once
#include "raylib_raygui.h"
#include "enums.h"

class Led {
public:
	Led() { center = { 0.0f, 0.0f }; radius = 0.0f; color = RAYWHITE; }
	Led(Vector2 center, Color color) : center{ center }, color{ color } {}

	void draw();

	Vector2 get_pos() { return center; }
	LedState get_state() const { return state; }

	void set_forced_state(bool forced_state) { this->forced_state = forced_state; }
	void set_state(LedState state) { this->state = state; }
	void set_center(Vector2 center) { this->center = center; }
	void set_color(Color color) { this->color = color; }

	bool is_updated(LedState to_ignore) { if (state == to_ignore) return false; return state != old_state; }
	void update() { old_state = state;  }

	static void set_radius(float r) { radius = r; }
	static float get_radius() { return radius; }
	static void init_colors() { Led::line_color = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL)); }
private:
	Vector2 center;
	Color color;

	bool forced_state = false;
	LedState state = LedState::OFF;
	LedState old_state = LedState::OFF;

	static float radius;
	static Color line_color;
};