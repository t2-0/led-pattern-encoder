#pragma once
#include "raylib_raygui.h"
#include "gui_elements.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

enum class LedState { NORMAL, FOCUSED, PRESSED };
enum class PatternState { NORMAL, MIRROR_H, MIRROR_V, MIRROR_HV };
enum class PatternType	{ DEFAULT, ANIMATION };

class Led {
public:
	Led(Vector2 center, Color color);

	void draw();

	Vector2 get_pos() { return center; }
	LedState get_state() const{ return state; }

	void set_forced_state(bool forced_state) { this->forced_state = forced_state; }
	void set_color(Color color) { this->color = color; }
	void set_state(LedState state) { this->state = state; }

	bool is_updated(LedState to_ignore) { if (state == to_ignore) return false; return state != old_state; }
	void update() { old_state = state; }

	static void init_colors();
	static float get_radius();
private:
	Vector2 center;
	Color color;

	bool forced_state = false;
	LedState state = LedState::NORMAL;
	LedState old_state = LedState::NORMAL;

	static float radius;
	static Color line_color;
};


class FramesManager {
public:
	void draw(vector<vector <Led>>& leds);

	vector<vector<vector <LedState>>> get_frame_states() { return frame_states; }
	bool is_updated() { return updated_b; }
	void update() { updated_b = false; }
private:
	vector<vector<vector <LedState>>> frame_states;
	int frame_idx = -1;

	float timer = 0.0f;
	float interval = 1.0f;
	bool play_b = false;
	bool repeat_b = false;
	bool updated_b = false;

	Button del_frame = { { 250.0f, 390.0f, 20.0f, 20.0f }, "-" };
	Button add_frame = { { 280.0f, 390.0f, 20.0f, 20.0f }, "+" };
	Button frame_back = { { 250.0f, 415.0f, 20.0f, 20.0f }, GuiIconText(ICON_ARROW_LEFT_FILL, nullptr) };
	Button frame_forward = { { 338.0f, 415.0f, 20.0f, 20.0f }, GuiIconText(ICON_ARROW_RIGHT_FILL, nullptr) };

	Toggle repeat_toggle = { { 250.0f, 465.0f, 20.0f, 20.0f }, GuiIconText(ICON_REPEAT, nullptr) };
	Button stop_btn = { { 280.0f, 465.0f, 20.0f, 20.0f }, GuiIconText(ICON_PLAYER_STOP, nullptr) };
	Button play_btn = { { 310.0f, 465.0f, 20.0f, 20.0f }, GuiIconText(ICON_PLAYER_PLAY,nullptr) };

	IntBox wait_input = { { 250.0f, 440.0f, 70.0f, 20.0f }, "wait_us: ", 0, 10000000, 1000000 };

	TextEx total_frames  = { { 250.0f, 368.0f }, "Total Frames: ", GetColor(GuiGetStyle(BUTTON,TEXT_COLOR_NORMAL)) };
	TextEx current_frame = { { 275.0f, 415.0f }, "Frame: ", GetColor(GuiGetStyle(BUTTON,TEXT_COLOR_NORMAL)) };
	TextEx interval_text = { { 320.0f, 440.0f }, "", GetColor(GuiGetStyle(BUTTON,TEXT_COLOR_NORMAL)) };

	vector <vector<LedState>> get_states_from_leds(vector<vector <Led>> leds);

	void set_leds_states(vector<vector <Led>>& leds);
	void write_in_states(const vector<vector <Led>>& leds);
};

class Pattern {
public:
	Pattern();

	void draw();

	void set_update_panel_b(bool b) { update_panel_b = b; }

	vector<vector<string>> get_hex() { return hex_v; }
	bool update_panel() { return update_panel_b; }
private:
	vector<TextEx> bits_v;
	vector<TextEx> idx_h;
	vector<vector <Led>> leds;
	vector<vector<string>> hex_v;

	int frame_idx = 0;
	bool update_panel_b = true;

	Button reset_btn  = { { 50.0f, 370.0f, 60.0f, 30.0f }, "Reset" };
	Button invert_btn = { { 50.0f, 405.0f, 60.0f, 30.0f }, "Invert" };

	Toggle mirror_h_toggle = { { 120.0f, 405.0f, 30.0f, 30.0f }, GuiIconText(ICON_SYMMETRY_HORIZONTAL, nullptr) };
	Toggle mirror_v_toggle = { { 120.0f, 370.0f, 30.0f, 30.0f }, GuiIconText(ICON_SYMMETRY_VERTICAL, nullptr) };

	Dropdownbox mode_drop = { { 50.0f, 440.0f, 100.0f, 30.0f }, "Default;Animation" };

	FramesManager frames_manager;

	Line* line_h = nullptr;
	Line* line_v = nullptr;

	PatternState state = PatternState::NORMAL;
	PatternType type   = PatternType::DEFAULT;

	// i++; 0, 3; 4, 7;
	void push_bits_v(int start_idx, int end_idx);

	void convert_hex();
	void reset();
	void invert();

	void draw_leds();

	// force states to set PRESSED on mirroed sides
	void mirror_h(size_t row, size_t col);
	void mirror_v(size_t row, size_t col);
	void mirror_hv(size_t row, size_t col);

	void is_updated();
};