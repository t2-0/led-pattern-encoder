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
	void set_type(PatternType type)    { this->type = type; }
	void set_amount(DisplayAmount amount) { this->display_amount = amount; }

	vector<vector<string>> get_hex() { return hex_v; }
private:
	vector<TextEx> bits_v;
	vector<TextEx> idx_h;
	vector<vector <Led>> leds;
	vector<vector<string>> hex_v;

	FramesManagerGui frames_gui;
	FramesManager frames_manager;

	bool update_panel_b = true;

	Line* line_h = nullptr;
	Line* line_v = nullptr;

	PatternState state = PatternState::NORMAL;
	PatternType type   = PatternType::DEFAULT;
	PatternType old_type = PatternType::ANIMATION; // force difference to pass very first normal stated pattern to panel
	DisplayAmount display_amount = DisplayAmount::x1;

	// i++; 0, 3; 4, 7;
	void push_bits_v(int start_idx, int end_idx);

	void convert_hex();

	void draw_leds();
	void draw_lines();

	// force states to set PRESSED on mirroed sides
	void mirror_h(size_t row, size_t col);
	void mirror_v(size_t row, size_t col);
	void mirror_hv(size_t row, size_t col);

	bool is_updated();
	void set_leds_states(array<array<LedState, 8>, 8> states);
};

class PatternGui {
public:
	void draw();
	void update(Pattern& pattern);
private:
	Button reset_btn  = { { 50.0f, 370.0f, 60.0f, 30.0f }, "Reset" };
	Button invert_btn = { { 50.0f, 405.0f, 60.0f, 30.0f }, "Invert" };

	Toggle mirror_h_toggle = { { 120.0f, 405.0f, 30.0f, 30.0f }, GuiIconText(ICON_SYMMETRY_HORIZONTAL, nullptr) };
	Toggle mirror_v_toggle = { { 120.0f, 370.0f, 30.0f, 30.0f }, GuiIconText(ICON_SYMMETRY_VERTICAL, nullptr) };

	Dropdownbox mode_drop = { { 50.0f, 440.0f, 100.0f, 30.0f }, "Default;Animation" };
	ToggleGroup display_amount = { { 160.0f, 440.0f, 30.0f, 30.0f }, "1x;4x" };
};