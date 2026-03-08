#include "CopyPanel.h"
#include <iostream>
#include <bitset>
#include <charconv>
#include <sstream>

using namespace std;

CopyPanel::CopyPanel(Rectangle bounds, float color_factor) {
	this->bounds = bounds;
	this->color_factor = color_factor;
	Color style_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
	line_color = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL));

	color = adjust_brightness(style_color, color_factor);

	Rectangle scrollbar_bounds = bounds;
	scrollbar_bounds.x += bounds.width + 5;
	scrollbar_bounds.width = 10;

	float font_size = GuiGetFont().baseSize;
	scrollbar = new ScrollBar{ scrollbar_bounds, 0, font_size, elements.size() };

	Vector2 container_pos = { bounds.x, bounds.y - 20.0f };
	container_s.set_pos(container_pos);
	container_s.set_color(RAYWHITE);
}

void CopyPanel::draw() {
	DrawRectangleRec(bounds, color);
	DrawRectangleLinesEx(bounds, 1.0f, line_color);

	BeginScissorMode(bounds.x - 20, bounds.y, bounds.width + 20, bounds.height);
	Font font = GuiGetFont();
	int y_offset = 1;

	Rectangle element_bounds = bounds;
	element_bounds.x++;
	element_bounds.y -= 18;

	for (size_t r = 0; r < elements.size(); r++) {
		for (size_t c = 0; c < elements[r].size(); c++) {
			if (elements.size() > 1) {
				element_bounds.y = bounds.y + y_offset + r * font.baseSize - scrollbar->get_scroll_val();
			}
			else {
				element_bounds.y += 19.0f;
			}

			elements[r][c].set_pos({ element_bounds.x, element_bounds.y });
			elements[r][c].draw();
		}
	}
	for (size_t i = 0; i < idx_v.size(); i++) {
		if (elements.size() > 1) {
			Vector2 pos = idx_v[i].get_pos();
			pos.y = bounds.y + y_offset + (i + 1) * font.baseSize - scrollbar->get_scroll_val();
			idx_v[i].set_pos(pos);
		}
		idx_v[i].draw();
	}
	EndScissorMode();


	scrollbar->draw();
	container_s.draw();
}

void CopyPanel::set_elements_text(const vector<vector<string>>& elements_s, PatternType pattern_type, DisplayAmount display_amount) {
	elements.clear();
	idx_v.clear();
	Rectangle idx_bounds = bounds;
	idx_bounds.x -= 12.0f;
	idx_bounds.y += 19.0f;
	idx_bounds.width -= 2.0f;
	idx_bounds.height = 19.0f;

	if (pattern_type == PatternType::DEFAULT) {
		if (display_amount == DisplayAmount::x1) {
			container_s.set_text("array<int, 8> ... =");
		}
		else {
			container_s.set_text("vector<array<int, 8>> ... =");
		}
	}
	else {
		if (display_amount == DisplayAmount::x1) {
			container_s.set_text("vector<array<int, 8>> ... =");
		}
		else {
			container_s.set_text("vector<vector<array<int, 8>>> ... =");
		}
	}

	for (size_t i = 0; i + 2 < elements_s.size(); i++) {
		Vector2 pos = { idx_bounds.x, idx_bounds.y };
		if (i >= 10) { pos.x -= font.baseSize / 2; }
		idx_v.push_back({ pos, to_string(i), GREEN });
		idx_bounds.y += 19.0f;
	}

	Color element_color = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
	for (size_t r = 0; r < elements_s.size(); r++) {
		elements.push_back({});
		for (size_t c = 0; c < elements[r].size() && c < elements_s[r].size(); c++) {
			elements[r][c].set_text(elements_s[r][c]);
			elements[r][c].set_color(element_color);
		}
	}

	float scrollbar_max = y_offset + elements.size() * font.baseSize - bounds.height;
	if (scrollbar_max != scrollbar->get_max_scroll()) {
		scrollbar->set_max_scroll(scrollbar_max);
		scrollbar->set_scroll_val(scrollbar_max);
	}
}

void CopyPanel::paste_elements(PatternGui& pattern_gui, Pattern& pattern) {
	const char* txt = GetClipboardText();
	if (txt == nullptr) {
		cout << "bad txt paste\n";  return;
	}

	stringstream ss { txt };
	string str;
	
	vector<vector<string>> elements_s;
	vector<array<bitset<8>, 8>> elements_valb;
	PatternType pattern_type = PatternType::DEFAULT;
	DisplayAmount display_amount = DisplayAmount::x1;

	bad_paste = false;
	bool animation_4x = false;

	ss >> str;
	if (str == "=") {
		string str_t;
		ss >> str_t;
		str += ' ';
		str += str_t;

		if (str == "= {") {	elements_s.push_back({ str }); }
		else { bad_paste = true; return; }
	}
	else { bad_paste = true; return; }

	ss >> str;
	if (str == "{") {
		string to_push = "\t\t\t" + str + ' ';
		elements_s.push_back({ to_push });
		elements_valb.push_back({});
	}
	else { bad_paste = true; return; }

	streampos pos = ss.tellg();
	ss >> str;
	if (str == "{") {
		cout << "4x animation\n";
		animation_4x = true;

		string to_push = "\t\t\t\t";
			   to_push += str;
			   to_push += ' ';
		elements_s.push_back({ to_push });
	}

	if (!animation_4x) { ss.seekg(pos); }
	while (true) {
		array<bitset<8>, 8>& arr = elements_valb.back();
		string& s = elements_s.back().back();

		for (int i = 0; i < 8; i++) {
			ss >> str;
			string_view str_v = str;

			if (str_v.size() >= 2) { str_v.remove_prefix(2); }
			if (!str_v.empty() && i != 7) { str_v.remove_suffix(1); }

			int val = 0;
			from_chars_result result = from_chars(str_v.data(), str_v.data() + str_v.size(), val, 16);

			if (result.ec == errc() && val >= 0x00 && val <= 0xFF) {
				arr[i] = val;
				s += str;
				s += ' ';
			}
			else { bad_paste = true; return; }
		}

		ss >> str;
		if (str == "}") {
			s += str;
			ss >> str;

			if (animation_4x && str == "}") {
				string to_push = "\t\t\t";
					   to_push += str;
					   to_push += ' ';
				elements_s.push_back({ to_push });
				ss >> str;
			}

			if (str == "};") {
				elements_s.push_back({ str });
				break;
			}
			else if (str == "},") {
				string to_push = "\t\t\t";
					   to_push += str;
					   to_push += ' ';
				elements_s.push_back({ to_push });

				ss >> str;
				string to_push2 = "\t\t\t";
				       to_push2 += str;
				       to_push2 += ' ';

				elements_s.push_back({ to_push2 });
				elements_valb.push_back({});

				if (animation_4x) {
					ss >> str;
					if (str != "{") { bad_paste = true; return;	}

					string to_push3 = "\t\t\t\t";
						   to_push3 += str;
						   to_push3 += ' ';
					elements_s.push_back({ to_push3 });
				}
			}
			else { bad_paste = true; return; }
		}
		else if (str == "},") {
			s += str;
			ss >> str;

			if (str != "{") { bad_paste = true; return;	}

			string to_push = "\t\t\t";
			if (animation_4x) { to_push += "\t"; }
			to_push += str;
			to_push += ' ';

			elements_s.push_back({ to_push });
			elements_valb.push_back({});
		}
		else { bad_paste = true; return; }
	}

	if (!bad_paste) {
		int array_amount = elements_s.size() - 2;

		if (animation_4x) {
			pattern_type = PatternType::ANIMATION;
			display_amount = DisplayAmount::x4;
		}
		else {
			if (array_amount > 1) {
				if (array_amount != 4) { pattern_type = PatternType::ANIMATION; }
				else { display_amount = DisplayAmount::x4; }
			}
		}

		pattern_gui.paste_conf(pattern_type, display_amount);
		pattern.set_type(pattern_type);
		pattern.set_amount(display_amount);
		pattern.set_pattern(elements_valb);

		set_elements_text(elements_s, pattern_type, display_amount);
	}
}

string CopyPanel::format_elements() const {
	string str;

	if (elements.size() == 1) {
		for (size_t r = 0; r < elements.size(); r++) {
			for (size_t c = 0; c < elements[r].size(); c++) {
				str += elements[r][c].get_text() + "\n";
			}
		}
	}
	else {
		for (size_t r = 0; r < elements.size(); r++) {
			for (size_t c = 0; c < elements[r].size(); c++) {
				str += elements[r][c].get_text();
			}
			str += "\n";
		}
	}

	str.pop_back();
	return str;
}

CopyPanelGui::CopyPanelGui(Rectangle panel_bounds) {
	Rectangle buttons_bounds = panel_bounds;
			  buttons_bounds.y += panel_bounds.height + 5.0f;
			  buttons_bounds.width = 60.0f;
			  buttons_bounds.height = 30.0f;

	float x_offset = 65.0f;
	copy_btn = new Button{ buttons_bounds, "Copy" };
	buttons_bounds.x += x_offset;
	paste_btn = new Button{ buttons_bounds, "Paste" };

	buttons_bounds.x = panel_bounds.x + panel_bounds.width - buttons_bounds.width - x_offset;
	load_btn = new Button{ buttons_bounds, "Load" };

	buttons_bounds.x += x_offset;
	save_btn = new Button{ buttons_bounds, "Save" };
}

void CopyPanelGui::draw() {
	copy_btn->draw();
	paste_btn->draw();
	load_btn->draw();
	save_btn->draw();
}

void CopyPanelGui::update(CopyPanel& panel, PatternGui& pattern_gui, Pattern& pattern) {
	if (copy_btn->clicked()) { SetClipboardText(panel.format_elements().c_str()); }
	if (paste_btn->clicked()) { panel.paste_elements(pattern_gui, pattern); };

	if(load_btn->clicked()) {
	}

	if (save_btn->clicked()) {
	}
}