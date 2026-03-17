#include<sstream>

#include "Pattern.h"

using std::stringstream;
using std::hex;
using std::uppercase;

DisplayStates<4> Pattern::get_states_from_leds() {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	DisplayStates<4> states = {};
	for (size_t i = 0; i < leds_active_size; i++) {
		for (size_t r = 0; r < states[i].size(); r++) {
			for (size_t c = 0; c < states[i][r].size(); c++) {
				states[i][r][c] = leds[i][r][c].get_state();
			}
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
	display_amount_tg.draw();
}

void PatternGui::update(Pattern& pattern) {
	if (reset_btn.clicked()) { pattern.reset(); }
	if (invert_btn.clicked()) { pattern.invert(); }

	if (mirror_h_toggle.is_updated() || mirror_v_toggle.is_updated()) {
		bool draw_h = mirror_h_toggle.is_active();
		bool draw_v = mirror_v_toggle.is_active();

		PatternState pattern_state = PatternState::NORMAL;
		if (draw_h) { pattern_state = PatternState::MIRROR_H; }
		if (draw_v) { pattern_state = PatternState::MIRROR_V; }
		if (draw_h && draw_v) { pattern_state = PatternState::MIRROR_HV; }

		mirror_h_toggle.update();
		mirror_v_toggle.update();

		pattern.set_state(pattern_state);
	}

	if (mode_drop.is_updated()) {
		PatternType pattern_type = PatternType::DEFAULT;
		if (mode_drop.get_active() == 1) { pattern_type = PatternType::ANIMATION; }
		pattern.set_type(pattern_type);
		mode_drop.update();
	}

	if (display_amount_tg.is_updated()) {
		// 0  == x1, 1 == x4
		int display_active = display_amount_tg.get_active();

		DisplayAmount display_amount = DisplayAmount::x1;
		if (display_active == 1) { display_amount = DisplayAmount::x4; }

		pattern.set_amount(display_amount);
		display_amount_tg.update();
	}
}

void PatternGui::paste_conf(PatternType pattern_type, DisplayAmount display_amount) {
	int pattern_active = 0;
	if (pattern_type == PatternType::ANIMATION) { pattern_active = 1; }
	mode_drop.set_active(pattern_active);

	int display_active = 0;
	if (display_amount == DisplayAmount::x4) { display_active = 1; }
	display_amount_tg.set_active(display_active);
}

Pattern::Pattern() {
	float x_4x_offset = 190.0f;
	float offset = 21.0f;

	Vector2 base_pos = { 35.0f, 60.0f };
	Vector2 led_pos = base_pos;
	led_pos.x += x_4x_offset;

	Color color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL));
	for (size_t i = 1; i < 4; i++) {
		led_pos.x = base_pos.x + x_4x_offset * i;
		led_pos.y = base_pos.y;
		for (int r = 0; r < 8; r++) {
			for (int c = 0; c < 8; c++) {
				leds[i][r][c].set_center(led_pos);
				led_pos.x += offset;
			}
			led_pos.x = base_pos.x + x_4x_offset * i;
			led_pos.y += offset;
		}
	}

	for (size_t r = 0; r < leds[0].size(); r++) {
		string s = to_string(r);
		idx_h[r].set_text(s);
		idx_h[r].set_color(BLUE);
	}

	set_amount(display_amount);
}

void Pattern::draw() {
	if(pattern_type == PatternType::ANIMATION) { frames_gui.draw(); }
	for(size_t i = 0; i < idx_h.size(); i++) { idx_h[i].draw(); }
	for(size_t i = 0; i < bits_v.size(); i++) { bits_v[i].draw(); }

	draw_leds();
	draw_lines();
}

void Pattern::update() {
	bool are_leds_updated = leds_updated();
	update_panel_b = are_leds_updated;
	bool type_b = old_type != pattern_type;

	if (pattern_type == PatternType::ANIMATION) {
		FramesManager& frames_manager = (display_amount == DisplayAmount::x1)
			? frames_manager_1x
			: frames_manager_4x;

		frames_gui.update(frames_manager);

		if (are_leds_updated) {
			DisplayStates<4> leds_states = get_states_from_leds();
			frames_manager.set_active_states(leds_states);
		}

		update_panel_b = update_panel_b || frames_manager.is_updated();

		if (frames_manager.is_updated()) {
			DisplayStates<4> frame_states = frames_manager.get_active_states();
			set_leds_states(frame_states);
			frames_manager.update();
		}

		frames_manager.play_loop();
	}

	update_panel_b = update_panel_b || type_b;
	update_panel_b = update_panel_b || (old_amount != display_amount);

	old_type = pattern_type;
	old_amount = display_amount;
	if (are_leds_updated) { leds_update(); }
}

void Pattern::reset() {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	for (size_t i = 0; i < leds_active_size; i++) {
		for (size_t r = 0; r < leds[i].size(); r++) {
			for (size_t c = 0; c < leds[i][r].size(); c++) {
				leds[i][r][c].set_state(LedState::OFF);
			}
		}
	}
}

void Pattern::invert() {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	for (size_t i = 0; i < leds_active_size; i++) {
		for (size_t r = 0; r < leds[i].size(); r++) {
			for (size_t c = 0; c < leds[i][r].size(); c++) {
				if (leds[i][r][c].get_state() == LedState::ON) {
					leds[i][r][c].set_state(LedState::OFF);
				}
				else {
					leds[i][r][c].set_state(LedState::ON);
				}
			}
		}
	}
}

void Pattern::draw_leds() {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	for (size_t i = 0; i < leds_active_size; i++) {
		for (size_t r = 0; r < leds[i].size(); r++) {
			for (size_t c = 0; c < leds[i][r].size(); c++) {
				leds[i][r][c].draw();

				switch (pattern_state) {
					case PatternState::MIRROR_H: 
							mirror_h(i, r, c);
						break;
					case PatternState::MIRROR_V: 
							mirror_v(i, r, c);
						break;
					case PatternState::MIRROR_HV: 
						if (display_amount == DisplayAmount::x1) { mirror_hv(i, r, c); }
						else { mirror_hv_4x(i, r, c); }
						break;
				}
			}
		}
	}
}

void Pattern::draw_lines() {
	switch (pattern_state) {
	case PatternState::MIRROR_H:
		line_h.draw();
		break;
	case PatternState::MIRROR_V:
		line_v.draw();
		break;
	case PatternState::MIRROR_HV:
		line_h.draw();
		line_v.draw();
		break;
	}
}

void Pattern::push_bits_v(int start_idx, int end_idx) {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	Font font = GuiGetFont();

	float y_offset;
	int b = 8;

	if (display_amount == DisplayAmount::x1) { y_offset = 35.0f; }
	else { y_offset = 25.0f; }

	Color color = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
	for (int i = 0; i < leds_active_size; i++) {
		b = 8;
		for (int c = start_idx; c <= end_idx; c++) {
			string s = to_string(b);

			Vector2 pos = leds[i][0][c].get_pos();
					pos.x -= round(MeasureTextEx(font, s.c_str(), font.baseSize, 0.0f).x / 2);
					pos.y -= y_offset;

			bits_v.push_back({ pos, s, color });

			b /= 2;
		}
	}
}

vector<vector<string>> Pattern::convert_hex() {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	vector<vector<string>> hex_v;

	hex_v.push_back({});
	hex_v[0].push_back("= {");

	if (pattern_type == PatternType::ANIMATION) {
		vector<DisplayStates<4>> states;

		if (display_amount == DisplayAmount::x1) {
			states = frames_manager_1x.get_frame_states();
		}
		else {
			states = frames_manager_4x.get_frame_states();
		}


		for (size_t l = 0; l < states.size(); l++) {
			if (display_amount == DisplayAmount::x4) {
				hex_v.push_back({});
				hex_v.back().push_back("\t\t\t{");
			}

			for (size_t i = 0; i < leds_active_size; i++) {
				stringstream ss;
				hex_v.push_back({});
				if (display_amount == DisplayAmount::x4) {
					ss << "\t";
				}
				ss << "\t\t\t{ ";

				for (size_t r = 0; r < states[l][i].size(); r++) {
					int bits_idx = 0;
					int val = 0;

					for (int c = 0; c < 4; c++, bits_idx++) {
						if (states[l][i][r][c] == LedState::ON) {
							val += stoi(bits_v[bits_idx].get_text());
						}
					}
					ss << "0x" << hex << uppercase << val;
					val = 0;

					for (int c = 4; c < 8; c++, bits_idx++) {
						if (states[l][i][r][c] == LedState::ON) {
							val += stoi(bits_v[bits_idx].get_text());
						}
					}
					ss << hex << uppercase << val << ", ";
				}
				hex_v.back().push_back(ss.str());
				hex_v.back().back().pop_back(); // pop ' '
				hex_v.back().back().pop_back(); // pop ','
				hex_v.back().back() += " },";
			}

			if (display_amount == DisplayAmount::x4) {
				hex_v.back().back().pop_back(); // pop ','
				hex_v.push_back({});
				hex_v.back().push_back("\t\t\t},");
			}
		}
	}
	else {
		for (size_t i = 0; i < leds_active_size; i++) {
			hex_v.push_back({});
			stringstream ss;
			ss << "\t\t\t{ ";
			for (size_t r = 0; r < leds[i].size(); r++) {
				int bits_idx = 0;
				int val = 0;

				for (int c = 0; c < 4; c++, bits_idx++) {
					if (leds[i][r][c].get_state() == LedState::ON) {
						val += stoi(bits_v[bits_idx].get_text());
					}
				}
				ss << "0x" << hex << uppercase << val;
				val = 0;

				for (int c = 4; c < 8; c++, bits_idx++) {
					if (leds[i][r][c].get_state() == LedState::ON) {
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
	}
	if (hex_v.size() > 1) { hex_v.back().back().pop_back(); } // pop ','
	hex_v.push_back({});
	hex_v.back().push_back("};");

	return hex_v;
}

void Pattern::mirror_h(size_t i, size_t row, size_t col) {
	Vector2 mouse_pos = GetMousePosition();
	size_t col_mirror = 8 - col - 1;
	size_t i_orig = i;
	if (display_amount == DisplayAmount::x4) { i = 4 - i - 1; }

	if (CheckCollisionPointCircle(mouse_pos, leds[i_orig][row][col].get_pos(), leds[i_orig][row][col].get_radius())) {
		LedState led_state = leds[i_orig][row][col].get_state();

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			leds[i][row][col_mirror].set_state(led_state);
		}

		if (leds[i_orig][row][col].get_state() == LedState::FOCUSED) {
			if (leds[i][row][col_mirror].get_state() != LedState::ON) {
				leds[i][row][col_mirror].set_state(led_state);
			}
			leds[i][row][col_mirror].set_state_locked(true);
		}
	}
	else {
		leds[i][row][col_mirror].set_state_locked(false);
	}
}

void Pattern::mirror_hv(size_t i, size_t row, size_t col) {
	Vector2 mouse_pos = GetMousePosition();
	size_t row_mirror = 8 - row - 1;
	size_t col_mirror = 8 - col - 1;

	if (CheckCollisionPointCircle(mouse_pos, leds[i][row][col].get_pos(), leds[i][row][col].get_radius())) {
		LedState led_state = leds[i][row][col].get_state();

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			leds[i][row_mirror][col_mirror].set_state(led_state);
			leds[i][row_mirror][col].set_state(led_state);
			leds[i][row][col_mirror].set_state(led_state);
		}

		if (leds[i][row][col].get_state() == LedState::FOCUSED) {
			if (leds[i][row_mirror][col_mirror].get_state() != LedState::ON) {
				leds[i][row_mirror][col_mirror].set_state(led_state);
			}
			if (leds[i][row_mirror][col].get_state() != LedState::ON) {
				leds[i][row_mirror][col].set_state(led_state);
			}
			if (leds[i][row][col_mirror].get_state() != LedState::ON) {
				leds[i][row][col_mirror].set_state(led_state);
			}

			leds[i][row_mirror][col_mirror].set_state_locked(true);
			leds[i][row_mirror][col].set_state_locked(true);
			leds[i][row][col_mirror].set_state_locked(true);
		}
	}
	else {
		leds[i][row][col].set_state_locked(false);
	}
}

void Pattern::mirror_hv_4x(size_t i, size_t row, size_t col) {
	Vector2 mouse_pos = GetMousePosition();
	size_t row_mirror = 8 - row - 1;
	size_t col_mirror = 8 - col - 1;
	size_t i_orig = i;
	i = 4 - i - 1;

	if (CheckCollisionPointCircle(mouse_pos, leds[i_orig][row][col].get_pos(), leds[i_orig][row][col].get_radius())) {
		LedState led_state = leds[i_orig][row][col].get_state();

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			leds[i][row_mirror][col_mirror].set_state(led_state);
			leds[i_orig][row_mirror][col].set_state(led_state);
			leds[i][row][col_mirror].set_state(led_state);
		}

		if (leds[i_orig][row][col].get_state() == LedState::FOCUSED) {
			if (leds[i][row_mirror][col].get_state() != LedState::ON) {
				leds[i][row_mirror][col].set_state(led_state);
			}
			if (leds[i_orig][row_mirror][col].get_state() != LedState::ON) {
				leds[i_orig][row_mirror][col].set_state(led_state);
				leds[i][row_mirror][col_mirror].set_state(led_state);
			}
			if (leds[i][row][col_mirror].get_state() != LedState::ON) {
				leds[i][row][col_mirror].set_state(led_state);
			}

			leds[i][row_mirror][col_mirror].set_state_locked(true);
			leds[i_orig][row_mirror][col].set_state_locked(true);
			leds[i][row_mirror][col_mirror].set_state_locked(true);
			leds[i][row][col_mirror].set_state_locked(true);
		}
	}
	else {
		leds[i_orig][row][col].set_state_locked(false);
	}
}

void Pattern::mirror_v(size_t i, size_t row, size_t col) {
	Vector2 mouse_pos = GetMousePosition();
	size_t row_mirror = 8 - row - 1;

	if (CheckCollisionPointCircle(mouse_pos, leds[i][row][col].get_pos(), leds[i][row][col].get_radius())) {
		LedState led_state = leds[i][row][col].get_state();

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			leds[i][row_mirror][col].set_state(led_state);
		}

		if (leds[i][row][col].get_state() == LedState::FOCUSED) {
			if (leds[i][row_mirror][col].get_state() != LedState::ON) {
				leds[i][row_mirror][col].set_state(led_state);
			}
			leds[i][row_mirror][col].set_state_locked(true);
		}
	}
	else {
		leds[i][row_mirror][col].set_state_locked(false);
	}
}

void Pattern::set_leds_states(DisplayStates<4> states) {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	for (size_t i = 0; i < leds_active_size; i++) {
		for (size_t r = 0; r < states[i].size(); r++) {
			for (size_t c = 0; c < states[i][r].size(); c++) {
				leds[i][r][c].set_state(states[i][r][c]);
			}
		}
	}
}

void Pattern::set_amount(DisplayAmount display_amount) {
	this->display_amount = display_amount;
	bits_v.clear();

	float offset;
	float radius;
	float x_idx_offset;
	Vector2 base_pos;

	if (display_amount == DisplayAmount::x1) {
		offset = 41.0f;
		radius = 15.0f;
		x_idx_offset = 28.0f;

		base_pos = { 60.0f, 60.0f };

		float x_pos = base_pos.x - radius;
		float y_pos = (base_pos.y - radius) + offset * 4 - offset / 8;

		Vector2 pos_start = { x_pos, y_pos };
		x_pos += offset * 8 - radius;
		Vector2 pos_end = { x_pos, y_pos };
		line_v = { pos_start, pos_end, 1.0f, RAYWHITE };

		x_pos = (base_pos.x - radius) + offset * 4 - offset / 8;
		y_pos = base_pos.y - radius;

		pos_start = { x_pos, y_pos };
		y_pos += offset * 8 - radius / 2;
		pos_end = { x_pos, y_pos };
		line_h = { pos_start, pos_end, 1.0f, RAYWHITE }; 
		
		set_leds_1x();
	}
	else {
		offset = 21.0f;
		radius = 8.0f;
		x_idx_offset = 21.0f;

		base_pos = { 35.0f, 60.0f };

		float x1 = leds[2][0][0].get_pos().x;
		float x2 = leds[1][0][7].get_pos().x;

		float x_pos = (x1 + x2) / 2;
		float y_pos = base_pos.y - radius;

		Vector2 pos_start = { x_pos, y_pos };
		float y = leds[2][7][0].get_pos().y;
		y_pos =  y + radius;
		Vector2 pos_end = { x_pos, y_pos };
		line_h = { pos_start, pos_end, 1.0f, RAYWHITE };


		x1 = leds[3][0][7].get_pos().x;
		y = leds[1][7][0].get_pos().y;
		base_pos = { 35.0f, 60.0f };
		x_pos = base_pos.x - radius;
		y_pos = base_pos.y - (base_pos.y - y) / 2;

		pos_start = { x_pos, y_pos };
		x_pos = x1 + radius;
		pos_end = { x_pos, y_pos };

		line_v = { pos_start, pos_end, 1.0f, RAYWHITE };

		set_leds_4x();
	}

	Vector2 led_pos = base_pos;
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			leds[0][r][c].set_center(led_pos);
			led_pos.x += offset;
		}
		led_pos.x = base_pos.x;
		led_pos.y += offset;
	}

	for (size_t r = 0; r < leds[0].size(); r++) {
		Vector2 pos = leds[0][r][0].get_pos();
				pos.x -= x_idx_offset;
				pos.y -= 7.0f;

		idx_h[r].set_pos(pos);
	}

	push_bits_v(0, 3);
	push_bits_v(4, 7);
	
	Led::set_radius(radius);
}

bool Pattern::leds_updated() {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	for (size_t i = 0; i < leds_active_size; i++) {
		for (size_t r = 0; r < leds[i].size(); r++) {
			for (size_t c = 0; c < leds[i][r].size(); c++) {
				if (leds[i][r][c].is_updated(LedState::FOCUSED)) {
					return true;
				}
			}
		}
	}
	return false;
}

void Pattern::leds_update() {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	for (size_t i = 0; i < leds_active_size; i++) {
		for (size_t r = 0; r < leds[i].size(); r++) {
			for (size_t c = 0; c < leds[i][r].size(); c++) {
				if (display_amount == DisplayAmount::x1) { set_states_1x(); }
				else { set_states_4x();}

				leds[i][r][c].update();
			}
		}
	}
}

void Pattern::set_type(PatternType type) {
	pattern_type = type;
	if (display_amount == DisplayAmount::x1) { set_leds_1x(); }
	else { set_leds_4x(); }
}

void Pattern::set_pattern(const vector<array<bitset<8>, 8>>& elements_valb) {
	if (pattern_type == PatternType::ANIMATION) {
		size_t leds_active_size = static_cast<size_t>(display_amount);

		FramesManager& frames_manager = (display_amount == DisplayAmount::x1)
			? frames_manager_1x
			: frames_manager_4x;

		frames_manager.paste_conf(elements_valb, leds_active_size);
		DisplayStates<4> frame_states = frames_manager.get_active_states();
		set_leds_states(frame_states);
	}
	else {
		for (size_t i = 0; i < elements_valb.size(); i++) {
			for (size_t r = 0; r < elements_valb[i].size(); r++) {
				for (size_t b = 0; b < elements_valb[i][r].size(); b++) {
					int b_r = elements_valb[i][r].size() - b - 1;
					if (elements_valb[i][r][b]) { leds[i][r][b_r].set_state(LedState::ON); }
					else { leds[i][r][b_r].set_state(LedState::OFF); }
				}
			}
		}
	}
}

void Pattern::set_states_1x() {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	for (size_t i = 0; i < leds_active_size; i++) {
		for (size_t r = 0; r < leds[i].size(); r++) {
			for (size_t c = 0; c < leds[i][r].size(); c++) {
				if (pattern_type == PatternType::DEFAULT) {
					states_1x.default_state[i][r][c] = leds[i][r][c].get_state();
				}
				else {
					states_1x.animation_state[i][r][c] = leds[i][r][c].get_state();
				}
			}
		}
	}
}

void Pattern::set_states_4x() {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	for (size_t i = 0; i < leds_active_size; i++) {
		for (size_t r = 0; r < leds[i].size(); r++) {
			for (size_t c = 0; c < leds[i][r].size(); c++) {
				if (pattern_type == PatternType::DEFAULT) {
					states_4x.default_state[i][r][c] = leds[i][r][c].get_state();
				}
				else {
					states_4x.animation_state[i][r][c] = leds[i][r][c].get_state();
				}
			}
		}
	}
}

void Pattern::set_leds_1x() {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	for (size_t i = 0; i < leds_active_size; i++) {
		for (size_t r = 0; r < leds[i].size(); r++) {
			for (size_t c = 0; c < leds[i][r].size(); c++) {
				if (pattern_type == PatternType::DEFAULT) {
					leds[i][r][c].set_state(states_1x.default_state[i][r][c]);
				}
				else {
					leds[i][r][c].set_state(states_1x.animation_state[i][r][c]);
				}
			}
		}
	}
}

void Pattern::set_leds_4x() {
	size_t leds_active_size = static_cast<size_t>(display_amount);
	for (size_t i = 0; i < leds_active_size; i++) {
		for (size_t r = 0; r < leds[i].size(); r++) {
			for (size_t c = 0; c < leds[i][r].size(); c++) {
				if (pattern_type == PatternType::DEFAULT) {
					leds[i][r][c].set_state(states_4x.default_state[i][r][c]);
				}
				else {
					leds[i][r][c].set_state(states_4x.animation_state[i][r][c]);
				}
			}
		}
	}
}