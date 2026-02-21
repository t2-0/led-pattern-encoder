#include "Led.h"

float Led::radius = 15.0f;
Color Led::line_color = RAYWHITE;

void Led::init_colors() {
	Led::line_color = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL));
}

float Led::get_radius() {
	return radius;
}

Led::Led(Vector2 center, Color color) {
	this->center = center;
	this->color = color;
}

void Led::draw() {
	if (!forced_state) {
		Vector2 mouse_pos = GetMousePosition();

		if (CheckCollisionPointCircle(mouse_pos, center, radius)) {
			if (state != LedState::PRESSED) {
				state = LedState::FOCUSED;
			}

			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
				state = LedState::PRESSED;
			}
			else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
				state = LedState::NORMAL;
			}
		}
		else {
			if (state == LedState::FOCUSED) {
				state = LedState::NORMAL;
			}
		}
	}

	switch (state) {
	case LedState::NORMAL:
		color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL));
		break;
	case LedState::FOCUSED:
		color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED));
		break;
	case LedState::PRESSED:
		color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_PRESSED));
		break;
	}
	DrawCircleV(center, radius, color);
	DrawCircleLinesV(center, radius, line_color);
}