#pragma once
#include "raylib_raygui.h"
#include "gui_elements.h"
#include "Led.h"
#include "enums.h"

#include <bitset>
#include <vector>
#include <iostream>

using namespace std;

class FramesManager {
public:
	void play_loop();

	void add_frame();
	void del_frame();

	void frame_back();
	void frame_forward();

	void play();
	void stop()   { repeat_b = false; play_b = false; }
	void repeat() { repeat_b = true; }

	void set_interval(float interval) { this->interval = interval; }
	void set_active_states(array<array<array<LedState, 8>, 8>, 4> active_states) { 
		this->active_states = active_states; 
		if (frame_idx != -1) {
			set_frame_states();
		}
	}

	vector<array<array<array <LedState, 8>, 8>, 4>> get_frame_states() { return frame_states; }
	array<array<array<LedState, 8>, 8>, 4> get_active_states() { return active_states; }

	bool is_updated() { return updated_b; }
	void update() { updated_b = false; }

	int get_idx() { return frame_idx; }
	size_t get_states_size() { return frame_states.size(); }
	float get_interval() { return interval; }

	void paste_conf(const vector<array<bitset<8>, 8>>& elements_valb, size_t leds_active_size);
private:
	vector<array<array<array <LedState, 8>, 8>, 4>> frame_states;
	array<array<array<LedState, 8>, 8>, 4> active_states = {};
	int frame_idx = -1;

	float timer = 0.0f;
	float interval = 1.0f;

	bool play_b = false;
	bool repeat_b = false;
	bool updated_b = true;

	void set_active_states();
	void set_frame_states();

	vector <vector<LedState>> get_states_from_leds(const vector<vector <Led>>& leds);
};

class FramesManagerGui {
public:
	void draw();
	void update(FramesManager& manager);
private:
	Button del_frame = { { 240.0f, 390.0f, 20.0f, 20.0f }, "-" };
	Button add_frame = { { 270.0f, 390.0f, 20.0f, 20.0f }, "+" };
	Button frame_back = { { 240.0f, 415.0f, 20.0f, 20.0f }, GuiIconText(ICON_ARROW_LEFT_FILL, nullptr) };
	Button frame_forward = { { 328.0f, 415.0f, 20.0f, 20.0f }, GuiIconText(ICON_ARROW_RIGHT_FILL, nullptr) };

	Toggle repeat_toggle = { { 240.0f, 498.0f, 20.0f, 20.0f }, GuiIconText(ICON_REPEAT, nullptr) };
	Button stop_btn = { { 270.0f, 498.0f, 20.0f, 20.0f }, GuiIconText(ICON_PLAYER_STOP, nullptr) };
	Button play_btn = { { 300.0f, 498.0f, 20.0f, 20.0f }, GuiIconText(ICON_PLAYER_PLAY, nullptr) };

	IntBox wait_input = { { 240.0f, 472.0f, 70.0f, 20.0f }, "wait_us: ", 0, 10000000, 1000000 };
	TextEx interval_text = { { 310.0f, 472.0f }, "", GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)) };

	TextEx total_frames  = { { 240.0f, 368.0f }, "Total Frames: ", GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)) };
	TextEx current_frame = { { 265.0f, 415.0f }, "Frame: ", GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)) };
};