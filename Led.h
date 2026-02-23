#pragma once
#include "raylib_raygui.h"
#include "enums.h"

class Led {
public:
	Led(Vector2 center,  float radius, Color color);

	void draw();

	Vector2 get_pos() { return center; }
	LedState get_state() const { return state; }

	void set_forced_state(bool forced_state) { this->forced_state = forced_state; }
	void set_color(Color color) { this->color = color; }
	void set_state(LedState state) { this->state = state; }

	bool is_updated() { return state != old_state; }
	void update() { old_state = state; }

	float get_radius();

	static void init_colors();
private:
	Vector2 center;
	Color color;

	bool forced_state = false;
	LedState state = LedState::NORMAL;
	LedState old_state = LedState::NORMAL;

	float radius;
	static Color line_color;
};