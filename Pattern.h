#pragma once
#include "raylib_raygui.h"
#include "gui_elements.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

enum class CircleState  { NORMAL, FOCUSED, PRESSED };
enum class PatternState { NORMAL, MIRROR_H, MIRROR_V, MIRROR_HV };
enum class PatternType	{ DEFAULT, ANIMATION };

class Circle {
public:
	Circle(Vector2 center, Color color);

	void draw();

	Vector2 get_pos() { return center; }
	CircleState get_state() { return state; }

	void set_forced_state(bool forced_state) { this->forced_state = forced_state; }
	void set_color(Color color) { this->color = color; }
	void set_state(CircleState state) { this->state = state; }

	static void init_colors();
	static float get_radius();
private:
	Vector2 center;
	Color color;

	bool forced_state = false;
	CircleState state = CircleState::NORMAL;

	static float radius;
	static Color line_color;
};


class TextEx {
public:
	TextEx(Vector2 pos, string text, Color color);

	void draw();
	void set_text(string text) { this->text = text; }

	string get_text() { return text; }

	friend ostream& operator<<(ostream& os, const TextEx& t);

	static void init_font();
private:
	string text;
	Vector2 pos;

	static Font font;
	Color color;
};

class FramesManager {
public:
	void draw(vector<vector <Circle>>& LEDs);
private:
	vector<vector<vector <CircleState>>> frame_states;
	int frame_idx = -1;

	float timer = 0.0f;
	float interval = 1.0f;
	bool play_b = false;
	bool repeat_b = false;

	Button del_frame = { { 250.0f, 390.0f, 20.0f, 20.0f }, "-" };
	Button add_frame = { { 280.0f, 390.0f, 20.0f, 20.0f }, "+" };
	Button frame_back = { { 250.0f, 415.0f, 20.0f, 20.0f }, GuiIconText(ICON_ARROW_LEFT_FILL, nullptr) };
	Button frame_forward = { { 335.0f, 415.0f, 20.0f, 20.0f }, GuiIconText(ICON_ARROW_RIGHT_FILL, nullptr) };

	Toggle repeat_toggle = { { 250.0f, 465.0f, 20.0f, 20.0f }, GuiIconText(ICON_REPEAT, nullptr) };
	Button stop_btn = { { 280.0f, 465.0f, 20.0f, 20.0f }, GuiIconText(ICON_PLAYER_STOP, nullptr) };
	Button play_btn = { { 310.0f, 465.0f, 20.0f, 20.0f }, GuiIconText(ICON_PLAYER_PLAY,nullptr) };

	IntBox wait_input = { { 250.0f, 440.0f, 70.0f, 20.0f }, "wait_us: ", 0, 10000000, 1000000 };

	TextEx total_frames  = { { 250.0f, 368.0f }, "Total Frames: ", GetColor(GuiGetStyle(BUTTON,TEXT_COLOR_NORMAL)) };
	TextEx current_frame = { { 275.0f, 415.0f }, "Frame: ", GetColor(GuiGetStyle(BUTTON,TEXT_COLOR_NORMAL)) };
	TextEx interval_text = { { 320.0f, 440.0f }, "", GetColor(GuiGetStyle(BUTTON,TEXT_COLOR_NORMAL)) };

	vector <vector<CircleState>> get_states(vector<vector <Circle>> LEDs);

	void set_leds_states(vector<vector <Circle>>& LEDs);
	void write_in_states(vector<vector <Circle>> LEDs);
};

class Pattern {
public:
	Pattern();

	void draw();

	vector<string> get_hex() { return hex_v; }
private:
	vector<TextEx> bits_v;
	vector<TextEx> idx_h;
	vector<vector <Circle>> LEDs;
	vector<string> hex_v;

	vector<vector<vector <CircleState>>> frame_states;
	int frame_idx = 0;

	Button reset_btn  = { { 50.0f, 370.0f, 60.0f, 30.0f }, "Reset" };
	Button invert_btn = { { 50.0f, 405.0f, 60.0f, 30.0f }, "Invert" }; //{ 295.0f, 370.0f, 60.0f, 30.0f }

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
};