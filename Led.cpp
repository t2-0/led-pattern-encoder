#include "Led.h"

Color Led::line_color = RAYWHITE;
float Led::radius = 15.0f;

void Led::draw() {
	if (!forced_state) {
		Vector2 mouse_pos = GetMousePosition();

		if (CheckCollisionPointCircle(mouse_pos, center, radius)) {
			if (state != LedState::ON) {
				state = LedState::FOCUSED;
			}

			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
				state = LedState::ON;
			}
			else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
				state = LedState::OFF;
			}
		}
		else {
			if (state == LedState::FOCUSED) {
				state = LedState::OFF;
			}
		}
	}

	switch (state) {
	case LedState::OFF:
		color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL));
		break;
	case LedState::FOCUSED:
		color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED));
		break;
	case LedState::ON:
		color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_PRESSED));
		break;
	}
	DrawCircleV(center, radius, color);
	DrawCircleLinesV(center, radius, line_color);
}