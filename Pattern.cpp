#include "Pattern.h"
#include <sstream>

array<array<LedState, 8>, 8> get_states_from_leds(const vector<vector <Led>>& leds) {
	array<array<LedState, 8>, 8> states;
	for (size_t r = 0; r < states.size(); r++) {
		for (size_t c = 0; c < states[r].size(); c++) {
			states[r][c] = leds[r][c].get_state();
		}
	}

	return states;
}

void PatternGui::draw() {
	reset_btn.draw();
	invert_btn.draw();

	mirror_h_toggle.draw();
	mirror_v_toggle.draw();

	mode_drop.draw();
	display_amount.draw();
}

void PatternGui::update(Pattern& pattern) {
	if (reset_btn.clicked())  { pattern.reset(); }
	if (invert_btn.clicked()) { pattern.invert(); }

	bool draw_h = mirror_h_toggle.is_active();
	bool draw_v = mirror_v_toggle.is_active();

	PatternState pattern_state = PatternState::NORMAL;
	if (draw_h) { pattern_state = PatternState::MIRROR_H; }
	if (draw_v) { pattern_state = PatternState::MIRROR_V; }
	if (draw_h && draw_v) { pattern_state = PatternState::MIRROR_HV; }
	pattern.set_state(pattern_state);

	PatternType pattern_type = PatternType::DEFAULT;
	if (mode_drop.get_active() == 1) { pattern_type = PatternType::ANIMATION; }
	pattern.set_type(pattern_type);

	DisplayAmount display_a = static_cast<DisplayAmount>(display_amount.get_active());
	pattern.set_amount(display_a);
}

Pattern::Pattern() {
	const Vector2 base_pos = { 60.0f, 60.0f };
	const float x_offset = 40.0f;
	const float y_offset = 40.0f;

	Vector2 led_pos = base_pos;
	float radius = 15.0f;
	Color color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL));


	for (int r = 0; r < 8; r++) {
		leds.push_back({});
		for (int c = 0; c < 8; c++) {
			leds[r].push_back({ led_pos, radius, color });
			led_pos.x += x_offset;
		}
		led_pos.x = base_pos.x;
		led_pos.y += y_offset;
	}
	led_pos = base_pos;


	int idx = 0;
	for (size_t c = 0; c < leds.size(); c++, idx++) {
		string s = to_string(idx);
		Vector2 pos = leds[c][0].get_pos();
				pos.y -= 7.0f;
				pos.x -= 28.0f;

		idx_h.push_back({ pos, s, BLUE });
	}

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
	if(type == PatternType::ANIMATION){ frames_gui.draw(); }
	if(display_amount == DisplayAmount::x1){
		for (int i = 0; i < 8; i++) {
			bits_v[i].draw();
			idx_h[i].draw();
		}
		draw_leds(); 
	}
	else{}

	
	draw_lines();
}

void Pattern::update() {
	bool type_b = old_type != type;
	update_panel_b = is_updated();
	update_panel_b = update_panel_b || type_b;

	if (type == PatternType::ANIMATION) {
		frames_gui.update(frames_manager);

		array<array<LedState, 8>, 8> leds_states = get_states_from_leds(leds);
		array<array<LedState, 8>, 8> frame_states = frames_manager.get_active_state();

		// prevent leds overriding manager active states
		if (type_b) { set_leds_states(frame_states); }

		// if leds state updated -> update active states for manager
		// otherwise if active manager state updated -> update leds states
		if (update_panel_b) { frames_manager.set_active_states(leds_states); }
		else if (frame_states != leds_states) { set_leds_states(frame_states); }

		update_panel_b = update_panel_b || frames_manager.is_updated();
		frames_manager.update();
		frames_manager.update_b();
	}
	old_type = type;

	if (update_panel_b) { convert_hex(); }
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
			case PatternState::MIRROR_H: mirror_h(r, c); break;
			case PatternState::MIRROR_V: mirror_v(r, c); break;
			case PatternState::MIRROR_HV: mirror_hv(r, c);break;
			}
		}
	}
}

void Pattern::draw_lines() {
	switch (state) {
	case PatternState::MIRROR_H:
		line_h->draw();
		break;
	case PatternState::MIRROR_V:
		line_v->draw();
		break;
	case PatternState::MIRROR_HV:
		line_h->draw();
		line_v->draw();
		break;
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
		vector<array<array <LedState, 8>, 8>> states = frames_manager.get_frame_states();

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
			hex_v[i + 1].back().pop_back(); // pop ' '
			hex_v[i + 1].back().pop_back(); // pop ','
			hex_v[i + 1].back() += " },";
		}
		if (hex_v.size() > 1) { hex_v.back().back().pop_back(); } // pop ','
		hex_v.push_back({});
		hex_v.back().push_back("};");
	}
}

void Pattern::mirror_h(size_t row, size_t col) {
	Vector2 mouse_pos = GetMousePosition();
	size_t col_mirror = 8 - col - 1;
	
	if (CheckCollisionPointCircle(mouse_pos, leds[row][col].get_pos(), leds[row][col].get_radius())) {
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

	if (CheckCollisionPointCircle(mouse_pos, leds[row][col].get_pos(), leds[row][col].get_radius())) {
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

	if (CheckCollisionPointCircle(mouse_pos, leds[row][col].get_pos(), leds[row][col].get_radius())) {
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

bool Pattern::is_updated() {
	bool b = false;
	for (size_t r = 0; r < leds.size(); r++) {
		for (size_t c = 0; c < leds[r].size(); c++) {
			if (leds[r][c].is_updated()) {
				b = true;
				leds[r][c].update();
			}
		}
	}
	return b;
}

void Pattern::set_leds_states(array<array<LedState, 8>, 8> states) {
	for (size_t r = 0; r < states.size(); r++) {
		for (size_t c = 0; c < states[r].size(); c++) {
			leds[r][c].set_state(states[r][c]);
		}
	}
}