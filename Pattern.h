#pragma once
#include "raylib_raygui.h"
#include "gui_elements.h"
#include "Led.h"
#include "FramesManager.h"
#include "enums.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;

class Pattern {
public:
	Pattern();

	void draw();
	void update();

	void reset();
	void invert();

	bool update_panel() { return update_panel_b; }

	void set_state(PatternState state) { this->state = state; }
	void set_type(PatternType type) { this->type = type; }
	void set_amount(DisplayAmount amount);
	void reset_update_panel() { update_panel_b = false; }
	vector<vector<string>> convert_hex();

	DisplayAmount get_amount() { return display_amount; }
private:
	vector<TextEx> bits_v;
	array<TextEx, 8> idx_h;
	array< array<array <Led, 8>, 8>, 4> leds;

	size_t leds_active_size = 1;

	FramesManagerGui frames_gui;
	FramesManager frames_manager;

	bool update_panel_b = true;

	Line line_h;
	Line line_v;

	PatternState state = PatternState::NORMAL;

	PatternType type   = PatternType::DEFAULT;
	PatternType old_type = PatternType::DEFAULT;

	DisplayAmount display_amount = DisplayAmount::x1;
	DisplayAmount old_amount = DisplayAmount::x1;

	// i++; 0, 3; 4, 7;
	void push_bits_v(int start_idx, int end_idx);

	void draw_leds();
	void draw_lines();

	// force states to set ON on mirroed sides
	void mirror_h(size_t i, size_t row, size_t col);

	void mirror_hv(size_t i, size_t row, size_t col);
	void mirror_hv_4x(size_t i, size_t row, size_t col);

	void mirror_v(size_t i, size_t row, size_t col);

	pair<bool, bool> leds_updated();
	void leds_update();

	void set_leds_states(array<array<LedState, 8>, 8> states);
};

class PatternGui {
public:
	void draw(DisplayAmount amount);
	void update(Pattern& pattern);
private:
	Button reset_btn  = { { 50.0f, 370.0f, 100.0f, 30.0f }, "Reset" };
	Button invert_btn = { { 50.0f, 405.0f, 100.0f, 30.0f }, "Invert" };

	Toggle mirror_h_toggle_1x = { { 160.0f, 405.0f, 30.0f, 30.0f }, GuiIconText(ICON_SYMMETRY_HORIZONTAL, nullptr) };
	Toggle mirror_v_toggle_1x = { { 160.0f, 370.0f, 30.0f, 30.0f }, GuiIconText(ICON_SYMMETRY_VERTICAL, nullptr) };

	Dropdownbox mode_drop = { { 50.0f, 440.0f, 100.0f, 30.0f }, "Default;Animation" };
	ToggleGroup display_amount = { { 160.0f, 440.0f, 30.0f, 30.0f }, "1x;4x" };
};