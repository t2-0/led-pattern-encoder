#include "Pattern.h"
#include <sstream>

float Led::radius = 15.0f;
Color Led::line_color = RAYWHITE;

Font TextEx::font = GuiGetFont();

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

vector <vector<LedState>> FramesManager::get_states_from_leds(vector<vector <Led>> leds) {
	vector<vector<LedState>> states;
	for (size_t r = 0; r < leds.size(); r++) {
		states.push_back({});
		for (size_t c = 0; c < leds[r].size(); c++) {
			states[r].push_back(leds[r][c].get_state());
		}
	}

	return states;
}

void FramesManager::set_leds_states(vector<vector <Led>>& leds) {
	for (size_t r = 0; r < leds.size(); r++) {
		for (size_t c = 0; c < leds.size(); c++) {
			LedState state_t = frame_states[frame_idx][r][c];
			leds[r][c].set_state(state_t);
		}
	}
}

void FramesManager::write_in_states(const vector<vector <Led>>& leds) {
	for (size_t r = 0; r < leds.size(); r++) {
		for (size_t c = 0; c < leds[r].size(); c++) {
			frame_states[frame_idx][r][c] = leds[r][c].get_state();
		}
	}
}

void FramesManager::draw(vector<vector <Led>>& leds) {
	if (play_b) {
		float delta_time = GetFrameTime();
		timer += delta_time;

		if (timer >= interval) {
			frame_idx++;
			if (frame_idx >= frame_states.size()) {
				if (!repeat_b) { play_b = false; }
				frame_idx = 0;
			}
			set_leds_states(leds);
			timer = 0.0f;
		}
	}

	if (frame_states.empty()) { current_frame.set_text("Frame: -"); }
	else { current_frame.set_text("Frame: " + to_string(frame_idx)); }
	total_frames.set_text("Total Frames: " + to_string(frame_states.size()));

	if (add_frame.draw()) {
		if (frame_states.size() < 64) {
			cout << "add\n";
			frame_states.push_back(get_states_from_leds(leds));
			frame_idx++;
			cout << frame_idx << endl;
		}
		updated_b = true;
	}

	if (del_frame.draw()) {
		cout << "del\n";
		if (!frame_states.empty()) { frame_states.pop_back(); }
		if (frame_idx >= frame_states.size()) {
			if (frame_states.empty()) {
				frame_idx = -1;
			}
			else {
				frame_idx = frame_states.size() - 1;
				set_leds_states(leds);
			}
			cout << frame_idx << endl;
		}
		updated_b = true;
	}

	if (frame_back.draw()) {
		if (frame_idx > 0) {
			frame_idx--;
			set_leds_states(leds);
		}
	}

	if (frame_forward.draw()) {
		if (frame_idx < frame_states.size() - 1 && !frame_states.empty()) {
			frame_idx++;
			set_leds_states(leds);
		}
	}

	if (frame_idx >= 0 && !frame_states.empty() && !play_b) { write_in_states(leds); }

	if (repeat_toggle.draw()) { repeat_b = true; }
	else { repeat_b = false; }
	if (stop_btn.draw()) { play_b = false; repeat_b = false; repeat_toggle.set_active(false); }
	if (play_btn.draw()) { 
		if (!frame_states.empty()) {
			play_b = true;
			frame_idx = 0;
			set_leds_states(leds);
		}
	}

	interval = wait_input.get_val() / pow(10, 6);
	string interval_str = " = " + to_string(interval);
		   interval_str += 's';
	interval_text.set_text(interval_str);

	wait_input.draw();
	interval_text.draw();

	total_frames.draw();
	current_frame.draw();
}

Pattern::Pattern() {
	const Vector2 base_pos = { 60.0f, 60.0f };
	const float x_offset = 40.0f;
	const float y_offset = 40.0f;

	Vector2 led_pos = base_pos;
	Color color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL));
	for (int r = 0; r < 8; r++) {
		leds.push_back({});
		for (int c = 0; c < 8; c++) {
			leds[r].push_back({ led_pos, color });
			led_pos.x += x_offset;
		}
		led_pos.x = base_pos.x;
		led_pos.y += y_offset;
	}

	int idx = 0;
	for (size_t c = 0; c < leds.size(); c++, idx++) {
		string s = to_string(idx);
		Vector2 pos = leds[c][0].get_pos();
				pos.y -= 7.0f;
				pos.x -= 28.0f;

		idx_h.push_back({ pos, s, BLUE });
	}

	float radius = Led::get_radius();
	float x_pos = base_pos.x - radius;
	float y_pos = (base_pos.y - radius) + y_offset * 4 - y_offset / 8;

	Vector2 pos_start = { x_pos, y_pos };
	Vector2 pos_end   = { x_pos * 8, y_pos };
	line_v = new Line{ pos_start, pos_end, 1.0f, RAYWHITE };

	x_pos = (base_pos.x - radius) + x_offset * 4 - x_offset / 8;
	y_pos = base_pos.y - radius;

	pos_start = { x_pos, y_pos };
	pos_end   = { x_pos, y_pos * 8 };
	line_h = new Line{ pos_start, pos_end, 1.0f, RAYWHITE };

	push_bits_v(0, 3);
	push_bits_v(4, 7);
}

void Pattern::draw() {
	for (int i = 0; i < 8; i++) {
		bits_v[i].draw();
		idx_h[i].draw();
	}	
	
	if (reset_btn.draw()) { reset(); }
	if (invert_btn.draw()) { invert(); }

	bool draw_h = mirror_h_toggle.draw();
	bool draw_v = mirror_v_toggle.draw();

	if (draw_h) { state = PatternState::MIRROR_H; }
	if (draw_v) { state = PatternState::MIRROR_V; }
	
	if (!draw_h && !draw_v) { state = PatternState::NORMAL; }
	if (draw_h && draw_v)   { state = PatternState::MIRROR_HV; }

	mode_drop.draw();
	if (mode_drop.get_active() == 0) { type = PatternType::DEFAULT; }
	else { type = PatternType::ANIMATION; }

	if (type == PatternType::ANIMATION) {
		frames_manager.draw(leds);
	}

	draw_leds();
	is_updated(); 
	// if update_panel
	convert_hex();
}

void Pattern::reset() {
	for (size_t r = 0; r < leds.size(); r++) {
		for (size_t c = 0; c < leds[r].size(); c++) {
			leds[r][c].set_state(LedState::NORMAL);
		}
	}
}

void Pattern::invert() {
	for (size_t r = 0; r < leds.size(); r++) {
		for (size_t c = 0; c < leds[r].size(); c++) {
			if (leds[r][c].get_state() == LedState::PRESSED) {
				leds[r][c].set_state(LedState::NORMAL);
			}
			else {
				leds[r][c].set_state(LedState::PRESSED);
			}
		}
	}
}

void Pattern::draw_leds() {
	for (size_t r = 0; r < leds.size(); r++) {
		for (size_t c = 0; c < leds[r].size(); c++) {
			leds[r][c].draw();
			switch (state) {
			case PatternState::MIRROR_H:
				mirror_h(r, c);
				line_h->draw();
				break;
			case PatternState::MIRROR_V:
				mirror_v(r, c);
				line_v->draw();
				break;
			case PatternState::MIRROR_HV:
				mirror_hv(r, c);
				line_h->draw();
				line_v->draw();
				break;
			}
		}
	}
}

void Pattern::push_bits_v(int start_idx, int end_idx) {
	Font font = GuiGetFont();
	int b = 8;

	for (int c = start_idx; c <= end_idx; c++) {
		string s = to_string(b);

		Vector2 pos = leds[0][c].get_pos();
				pos.y -= 35.0f;
				pos.x -= round(MeasureTextEx(font, s.c_str(), font.baseSize, 0.0f).x / 2);

		Color color = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
		bits_v.push_back({ pos, s, color });

		b /= 2;
	}
}

void Pattern::convert_hex() {
	if (type == PatternType::DEFAULT) {
		hex_v.clear();
		hex_v.push_back({});
		hex_v[0].push_back("= {");

		for (size_t r = 0; r < leds.size(); r++) {
			stringstream ss;
			int bits_idx = 0;
			int val = 0;

			for (int i = 0; i < 4; i++, bits_idx++) {
				if (leds[r][i].get_state() == LedState::PRESSED) {
					val += stoi(bits_v[bits_idx].get_text());
				}
			}
			ss << "\t\t\t0x" << hex << uppercase << val;
			val = 0;

			for (int i = 4; i < 8; i++, bits_idx++) {
				if (leds[r][i].get_state() == LedState::PRESSED) {
					val += stoi(bits_v[bits_idx].get_text());
				}
			}
			ss << hex << uppercase << val << ",";
			hex_v[0].push_back(ss.str());
		}
		hex_v[0].back().pop_back();
		hex_v[0].push_back("};");
	}
	else {
		vector<vector<vector <LedState>>> states = frames_manager.get_frame_states();

		hex_v.clear();
		hex_v.push_back({});
		hex_v[0].push_back("= {");
		for (size_t i = 0; i < states.size(); i++) {
			hex_v.push_back({});
			stringstream ss;
			ss << "\t\t\t{ ";
			for (size_t r = 0; r < states[i].size(); r++) {
				int bits_idx = 0;
				int val = 0;

				for (int b = 0; b < 4; b++, bits_idx++) {
					if (states[i][r][b] == LedState::PRESSED) {
						val += stoi(bits_v[bits_idx].get_text());
					}
				}
				ss << "0x" << hex << uppercase << val;
				val = 0;

				for (int b = 4; b < 8; b++, bits_idx++) {
					if (states[i][r][b] == LedState::PRESSED) {
						val += stoi(bits_v[bits_idx].get_text());
					}
				}
				ss << hex << uppercase << val << ", ";
			}
			hex_v[i + 1].push_back(ss.str());
			hex_v[i + 1].back().pop_back();
			hex_v[i + 1].back().pop_back();
			hex_v[i + 1].back() += " },";
		}

		hex_v.push_back({});
		hex_v.back().push_back("};");
		/*
		for (int i = 0; i < hex_v.size(); i++) {
			cout << "i: " << i << endl;
			for (int v = 0; v < hex_v[i].size(); v++) {
				cout << hex_v[i][v];
			}
			cout << endl;
		}
		*/
	}
}

void Pattern::mirror_h(size_t row, size_t col) {
	Vector2 mouse_pos = GetMousePosition();
	size_t col_mirror = 8 - col - 1;

	if (CheckCollisionPointCircle(mouse_pos, leds[row][col].get_pos(), Led::get_radius())) {
		LedState led_state = leds[row][col].get_state();
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			leds[row][col_mirror].set_state(led_state);
		}

		if (leds[row][col].get_state() == LedState::FOCUSED) {
			if (leds[row][col_mirror].get_state() != LedState::PRESSED) {
				leds[row][col_mirror].set_state(led_state);
			}
			leds[row][col_mirror].set_forced_state(true);
		}
	}
	else {
		leds[row][col_mirror].set_forced_state(false);
	}
}

void Pattern::mirror_v(size_t row, size_t col) {
	Vector2 mouse_pos = GetMousePosition();
	size_t row_mirror = 8 - row - 1;

	if (CheckCollisionPointCircle(mouse_pos, leds[row][col].get_pos(), Led::get_radius())) {
		LedState led_state = leds[row][col].get_state();
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			leds[row_mirror][col].set_state(led_state);
		}

		if (leds[row][col].get_state() == LedState::FOCUSED) {
			if (leds[row_mirror][col].get_state() != LedState::PRESSED) {
				leds[row_mirror][col].set_state(led_state);
			}
			leds[row_mirror][col].set_forced_state(true);
		}
	}
	else {
		leds[row_mirror][col].set_forced_state(false);
	}
}

void Pattern::mirror_hv(size_t row, size_t col) {
	Vector2 mouse_pos = GetMousePosition();
	size_t row_mirror = 8 - row - 1;
	size_t col_mirror = 8 - col - 1;

	if (CheckCollisionPointCircle(mouse_pos, leds[row][col].get_pos(), Led::get_radius())) {
		LedState led_state = leds[row][col].get_state();
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			leds[row_mirror][col_mirror].set_state(led_state);
			leds[row_mirror][col].set_state(led_state);
			leds[row][col_mirror].set_state(led_state);
		}

		if (leds[row][col].get_state() == LedState::FOCUSED) {
			if (leds[row_mirror][col_mirror].get_state() != LedState::PRESSED) {
				leds[row_mirror][col_mirror].set_state(led_state);
			}
			if (leds[row_mirror][col].get_state() != LedState::PRESSED) {
				leds[row_mirror][col].set_state(led_state);
			}
			if (leds[row][col_mirror].get_state() != LedState::PRESSED) {
				leds[row][col_mirror].set_state(led_state);
			}

			leds[row_mirror][col_mirror].set_forced_state(true);
			leds[row_mirror][col].set_forced_state(true);
			leds[row][col_mirror].set_forced_state(true);
		}

	}
	else {
		leds[row][col].set_forced_state(false);
	}
}

void Pattern::is_updated() {
	for (size_t r = 0; r < leds.size(); r++) {
		for (size_t c = 0; c < leds[r].size(); c++) {
			if (leds[r][c].is_updated(LedState::FOCUSED)) {
				update_panel_b = true;
				leds[r][c].update();
			}
		}
	}
}