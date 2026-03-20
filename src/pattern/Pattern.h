#pragma once
#include "raylib_raygui.h"
#include "gui/gui_elements.h"
#include "led/Led.h"
#include "frames/FramesManager.h"

#include "types/enums.h"
#include "types/using_templates.h"

// Holds LED states for both static and animated modes.
// N = number of displays (1 or 4)
template<size_t N>
struct ActiveLedStates {
	DisplayStates<N> default_state = {};
	DisplayStates<N> animation_state = {}; 
};

// Represents an 8x8 LED pattern across 1 or 4 displays.
//
// Responsibilities:
// - Store and manage LED states
// - Handle rendering (LEDs + grid lines)
// - Switch between static and animation modes
// - Synchronize with FramesManager for animations
//
// UI labels:
// - idx_h: horizontal indices (rows)
// - bits_v: vertical bit labels (bitmask values: 8 4 2 1 8 4 2 1)
//   representing columns encoded as 0x00–0xFF
class Pattern {
public:
	Pattern();

	void draw();
	void update();

	void reset();
	void invert();

	bool update_panel() { return update_panel_b; }

	void set_amount(DisplayAmount amount);
	void reset_update_panel() { update_panel_b = false; }
	vector<vector<string>> convert_hex();

	DisplayAmount get_amount() { return display_amount; }
	PatternType get_type()     { return pattern_type; }

	void set_state(PatternState state) { pattern_state = state; }
	void set_type(PatternType type);
	// Set pattern from external source (e.g., clipboard or file)
	void set_pattern(const vector<array<bitset<8>, 8>>& elements_valb);
private:
	vector<TextEx> bits_v;
	array<TextEx, 8> idx_h;
	Display<4> leds;
	// Separate state storage for 1x and 4x display modes.
	// Each configuration maintains its own independent data,
	// preventing state loss when switching between sizes.
	ActiveLedStates<1> states_1x;
	ActiveLedStates<4> states_4x;

	FramesManagerGui frames_gui;
	FramesManager frames_manager_1x;
	FramesManager frames_manager_4x;

	bool update_panel_b = true;

	Line line_h;
	Line line_v;

	PatternState pattern_state = PatternState::NORMAL;

	PatternType pattern_type = PatternType::DEFAULT;
	PatternType old_type = PatternType::ANIMATION;

	DisplayAmount display_amount = DisplayAmount::x1;
	DisplayAmount old_amount = DisplayAmount::x4;

	void push_bits_v(int start_idx, int end_idx);

	void draw_leds();
	void draw_lines();

	void mirror_h(size_t i, size_t row, size_t col);

	void mirror_hv(size_t i, size_t row, size_t col);
	void mirror_hv_4x(size_t i, size_t row, size_t col);

	void mirror_v(size_t i, size_t row, size_t col);

	bool leds_updated();
	void leds_update();

	void set_leds_states(DisplayStates<4> states);
	DisplayStates<4> get_states_from_leds();

	void set_leds_1x();
	void set_leds_4x();
};

class PatternGui {
public:
	void draw();
	void update(Pattern& pattern);

	void paste_conf(PatternType pattern_type, DisplayAmount display_amount);
private:
	Button reset_btn  = { { 50.0f, 370.0f, 100.0f, 30.0f }, "Reset" };
	Button invert_btn = { { 50.0f, 405.0f, 100.0f, 30.0f }, "Invert" };

	Toggle mirror_h_toggle = { { 160.0f, 405.0f, 30.0f, 30.0f }, GuiIconText(ICON_SYMMETRY_HORIZONTAL, nullptr) };
	Toggle mirror_v_toggle = { { 160.0f, 370.0f, 30.0f, 30.0f }, GuiIconText(ICON_SYMMETRY_VERTICAL, nullptr) };

	Dropdownbox mode_drop = { { 50.0f, 440.0f, 100.0f, 30.0f }, "Default;Animation" };
	ToggleGroup display_amount_tg = { { 160.0f, 440.0f, 30.0f, 30.0f }, "1x;4x" };
};