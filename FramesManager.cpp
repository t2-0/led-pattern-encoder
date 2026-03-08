#include "FramesManager.h"

void FramesManagerGui::draw() {
	add_frame.draw();
	del_frame.draw();

	frame_back.draw();
	frame_forward.draw();

	repeat_toggle.draw();

	stop_btn.draw();
	play_btn.draw();

	wait_input.draw();
	interval_text.draw();

	total_frames.draw();
	current_frame.draw();
}

void FramesManagerGui::update(FramesManager& manager) {
	size_t states_size = manager.get_states_size();
	if (states_size == 0) { current_frame.set_text("Frame: -"); }
	else { current_frame.set_text("Frame: " + to_string(manager.get_idx())); }
	total_frames.set_text("Total Frames: " + to_string(states_size));

	if (add_frame.clicked()) { manager.add_frame(); }
	if (del_frame.clicked()) { manager.del_frame(); }

	if (frame_forward.clicked()) { manager.frame_forward(); }
	if (frame_back.clicked())    { manager.frame_back(); }

	if (stop_btn.clicked()) { manager.stop(); repeat_toggle.set_active(false); }
	if (play_btn.clicked()) { manager.play(); }

	if (repeat_toggle.is_updated()) {
		if (repeat_toggle.is_active()) { manager.repeat(); }
		else { manager.stop(); }

		repeat_toggle.update();
	}

	manager.set_interval(wait_input.get_val() / pow(10, 6));
	string interval_str = " = " + to_string(manager.get_interval());
		   interval_str += 's';
	interval_text.set_text(interval_str);
}

vector <vector<LedState>> FramesManager::get_states_from_leds(const vector<vector <Led>>& leds) {
	vector<vector<LedState>> states;
	for (size_t r = 0; r < leds.size(); r++) {
		states.push_back({});
		for (size_t c = 0; c < leds[r].size(); c++) {
			states[r].push_back(leds[r][c].get_state());
		}
	}

	return states;
}

void FramesManager::set_active_states() {
	for (size_t r = 0; r < active_states.size(); r++) {
		for (size_t c = 0; c < active_states[r].size(); c++) {
			active_states[r][c] = frame_states[frame_idx][r][c];
		}
	}
}

void FramesManager::set_frame_states() {
	for (size_t i = 0; i < active_states.size(); i++) {
		for (size_t r = 0; r < active_states[i].size(); r++) {
			for (size_t c = 0; c < active_states[i][r].size(); c++) {
				frame_states[frame_idx][i][r][c] = active_states[i][r][c];
			}
		}
	}
}

void FramesManager::update() {
	if (play_b) {
		float delta_time = GetFrameTime();
		timer += delta_time;

		if (timer >= interval) {
			frame_idx++;
			if (frame_idx >= frame_states.size()) {
				if (!repeat_b) { play_b = false; }
				frame_idx = 0;
			}
			set_active_states();
			timer = 0.0f;
		}
	}

	if (frame_idx >= 0 && !frame_states.empty() && !play_b) { set_frame_states(); }
}

void FramesManager::add_frame() {
	if (frame_states.size() < 64) {
		frame_states.push_back(active_states);
		frame_idx = frame_states.size() - 1;
		updated_b = true;
	}
}

void FramesManager::del_frame() {
	if (!frame_states.empty()) { frame_states.pop_back(); }
	if (frame_idx >= frame_states.size()) {
		if (frame_states.empty()) {
			frame_idx = -1;
		}
		else {
			frame_idx = frame_states.size() - 1;
			set_active_states();
		}
		updated_b = true;
	}
}

void FramesManager::frame_back() {
	if (frame_idx > 0) {
		frame_idx--;
		set_active_states();
	}
}

void FramesManager::frame_forward() {
	if (frame_idx < frame_states.size() - 1 && !frame_states.empty()) {
		frame_idx++;
		set_active_states();
	}
}

void FramesManager::play() {
	if (!frame_states.empty()) {
		play_b = true;
		frame_idx = 0;
		set_active_states();
	}
}

void FramesManager::paste_conf(const vector<array<bitset<8>, 8>>& elements_valb, size_t leds_active_size) {
	frame_states.clear();
	size_t c = 0;
	for (size_t i = 0; i < elements_valb.size() / leds_active_size; i++) {
		for (size_t l = 0; l < leds_active_size; l++, c++) {
			if (frame_states.size() < 64) {
				for (size_t r = 0; r < elements_valb[i].size(); r++) {
					for (size_t b = 0; b < elements_valb[i][r].size(); b++) {
						int b_r = elements_valb[c][r].size() - b - 1;
						if (elements_valb[c][r][b]) { active_states[l][r][b_r] = LedState::ON; }
						else { active_states[l][r][b_r] = LedState::OFF; }
					}
				}
			}
		}
		frame_states.push_back(active_states);
	}

	frame_idx = frame_states.size() - 1;
}