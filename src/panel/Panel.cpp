#include "Panel.h"
#include <charconv>
#include <sstream>

#ifdef _WIN32
	#include "utils/win32_dialog.h"
#elif __EMSCRIPTEN__
	#include "utils/web.h"

	string web_str;
	bool file_loaded_b = false;

	extern "C"
		void file_loaded(char* text) {
			if (!text) { return; }

			web_str = text;
			file_loaded_b = true;
		}

	string clipboard_str;
	bool clipboard_ready = false;

	extern "C"
		void clipboard_pasted(char* text) {
		if (!text) { return; }

		clipboard_str   = text;
		clipboard_ready = true;
	}

#endif

using std::stringstream;
using std::string_view;
using std::streampos;
using std::from_chars_result;
using std::from_chars;
using std::errc;

Panel::Panel(Rectangle bounds, float color_factor) {
	this->bounds = bounds;
	this->color_factor = color_factor;
	Color style_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
	line_color = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL));

	color = adjust_brightness(style_color, color_factor);

	Rectangle scrollbar_bounds = bounds;
	scrollbar_bounds.x += bounds.width + 5;
	scrollbar_bounds.width = 10;

	float font_size = GuiGetFont().baseSize;
	scrollbar = ScrollBar{ scrollbar_bounds, 0, 10 + elements.size() * (font_size + 10), font_size, };

	Vector2 container_pos = { bounds.x, bounds.y - 20.0f };
	container_s.set_pos(container_pos);
	container_s.set_color(RAYWHITE);

	string invalid_txt = "Invalid data";
	Vector2 invalid_pos = { bounds.x, bounds.y + bounds.height + 10.0f };
			invalid_pos.x += bounds.width / 2;
			invalid_pos.x -= round(MeasureTextEx(font, invalid_txt.c_str(), font.baseSize, 0.0f).x / 2);

	invalid_in.set_text({ invalid_pos, invalid_txt, RED });
}

void Panel::draw() {
	DrawRectangleRec(bounds, color);
	DrawRectangleLinesEx(bounds, 1.0f, line_color);

	BeginScissorMode(bounds.x - 20, bounds.y, bounds.width + 20, bounds.height);

	for (size_t i = 0; i < idx_v.size(); i++) { idx_v[i].draw(-scrollbar.get_scroll_val()); }
	for (size_t r = 0; r < elements.size(); r++) {
		for (size_t c = 0; c < elements[r].size(); c++) {
			elements[r][c].draw(-scrollbar.get_scroll_val());
		}
	}
	EndScissorMode();

	scrollbar.draw();
	container_s.draw(); 
	invalid_in.draw();
}

void Panel::set_elements_text(const vector<vector<string>>& elements_s, PatternType pattern_type, DisplayAmount display_amount) {
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

	if (display_amount == DisplayAmount::x4 && pattern_type == PatternType::ANIMATION) {
		if (elements_s.size() > 2) {
			int c = 0;
			for (size_t i = 0; i < (elements_s.size() - 2) / 6; i++, c += 6) {
				Vector2 pos = { idx_bounds.x, idx_bounds.y };
						pos.y = bounds.y + (c + 1) * font.baseSize;
				if (i >= 10) { pos.x -= font.baseSize / 2; }
				idx_v.push_back({ pos, to_string(i), WHITE });
			}
		}
	}
	else {
		for (size_t i = 0; i + 2 < elements_s.size(); i++) {
			Vector2 pos = { idx_bounds.x, idx_bounds.y };
					pos.y = bounds.y + (i + 1) * font.baseSize;
			if (i >= 10) { pos.x -= font.baseSize / 2; }
			idx_v.push_back({ pos, to_string(i), WHITE });
		}
	}

	Color element_color = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
	Vector2 element_pos = { bounds.x, bounds.y };
			element_pos.x++;

	for (size_t r = 0; r < elements_s.size(); r++) {
		elements.push_back({});
		element_pos.y = bounds.y + r * font.baseSize;
		for (size_t c = 0; c < elements_s[r].size(); c++) {
			elements[r][c].set_text(elements_s[r][c]);
			elements[r][c].set_color(element_color);
			elements[r][c].set_pos(element_pos);
		}
	}

	float scrollbar_max = elements.size() * font.baseSize - bounds.height;
	if (scrollbar_max < 0) { scrollbar_max = 0; }
	if (scrollbar_max != scrollbar.get_max_scroll()) {
		scrollbar.set_max_scroll(scrollbar_max);
	}
}

void Panel::copy() {
	SetClipboardText(format_elements().c_str());
}

void Panel::paste_elements(const char* txt, PatternGui& pattern_gui, Pattern& pattern) {
	if (txt == nullptr) { invalid_in.trigger(); return; }

	stringstream ss{ txt };
	string str;

	vector<vector<string>> elements_s;
	vector<array<bitset<8>, 8>> elements_valb;
	PatternType pattern_type = PatternType::DEFAULT;
	DisplayAmount display_amount = DisplayAmount::x1;

	bool animation_4x = false;

	ss >> str;
	if (str == "=") {
		string str_t;
		ss >> str_t;
		str += ' ';
		str += str_t;

		if (str == "= {") { elements_s.push_back({ str }); }
		else { invalid_in.trigger(); return; }
	}
	else { invalid_in.trigger(); return; }

	ss >> str;
	if (str == "{") {
		string to_push = "\t\t\t" + str + ' ';
		elements_s.push_back({ to_push });
		elements_valb.push_back({});
	}
	else { invalid_in.trigger(); return; }

	streampos pos = ss.tellg();
	ss >> str;
	if (str == "{") {
		animation_4x = true;

		string to_push = "\t\t\t\t";
		to_push += str;
		to_push += ' ';
		elements_s.push_back({ to_push });
	}

	if (!animation_4x) { ss.seekg(pos); }

	int counter_4x = 1;
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
			else { invalid_in.trigger(); return; }
		}

		ss >> str;
		if (str == "}") {
			s += str;
			ss >> str;

			if (animation_4x && str == "}") {
				if (counter_4x != 4) { invalid_in.trigger(); return; }
				counter_4x = 0;
				string to_push = "\t\t\t";
				to_push += str;
				elements_s.push_back({ to_push });
				ss >> str;
			}

			if (str == "};") {
				elements_s.push_back({ str });
				break;
			}
			else if (str == "},") {
				if (animation_4x && counter_4x != 4) { invalid_in.trigger(); return; }
				counter_4x = 0;
				string to_push = "\t\t\t";
				to_push += str;
				elements_s.push_back({ to_push });

				ss >> str;
				string to_push2 = "\t\t\t";
				to_push2 += str;
				to_push2 += ' ';

				elements_s.push_back({ to_push2 });
				elements_valb.push_back({});

				if (animation_4x) {
					ss >> str;
					if (str != "{") { invalid_in.trigger(); return; }

					counter_4x++;
					string to_push3 = "\t\t\t\t";
					to_push3 += str;
					to_push3 += ' ';
					elements_s.push_back({ to_push3 });
				}
			}
			else { invalid_in.trigger(); return; }
		}
		else if (str == "},") {
			s += str;
			ss >> str;

			if (str != "{") { invalid_in.trigger(); return; }

			counter_4x++;
			string to_push = "\t\t\t";
			if (animation_4x) { to_push += "\t"; }
			to_push += str;
			to_push += ' ';

			elements_s.push_back({ to_push });
			elements_valb.push_back({});
		}
		else { invalid_in.trigger(); return; }
	}

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

// Convert current elements into single string.
string Panel::format_elements() const {
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

PanelGui::PanelGui(Rectangle panel_bounds) {
	Rectangle buttons_bounds = panel_bounds;
			  buttons_bounds.y += panel_bounds.height + 5.0f;
			  buttons_bounds.width = 60.0f;
			  buttons_bounds.height = 30.0f;

	float x_offset = 65.0f;
	copy_btn = Button{ buttons_bounds, "Copy" };
	buttons_bounds.x += x_offset;
	paste_btn = Button{ buttons_bounds, "Paste" };

	buttons_bounds.x = panel_bounds.x + panel_bounds.width - buttons_bounds.width - x_offset;
	load_btn = Button{ buttons_bounds, "Load" };

	buttons_bounds.x += x_offset;
	save_btn = Button{ buttons_bounds, "Save" };
}

void PanelGui::draw() {
	copy_btn.draw();
	paste_btn.draw();
	load_btn.draw();
	save_btn.draw();
}

#ifdef _WIN32
	void Panel::save_elements() {
		HWND hwnd = (HWND)GetWindowHandle();

		string str;
		for (size_t i = 0; i < elements.size(); i++) {
			for (size_t r = 0; r < elements[i].size(); r++) {
				str += elements[i][r].get_text();
			}
			str += '\n';
		}

		save_file(hwnd, str);
	}

	void Panel::load_elements(PatternGui& pattern_gui, Pattern& pattern) {
		HWND hwnd = (HWND)GetWindowHandle();
		string str = load_file(hwnd);
		paste_elements(str.c_str(), pattern_gui, pattern);
	}

	void PanelGui::update(Panel& panel, PatternGui& pattern_gui, Pattern& pattern) {
		if (copy_btn.clicked()) { panel.copy(); }

		if (paste_btn.clicked()) {
			const char* txt = GetClipboardText();
			panel.paste_elements(txt, pattern_gui, pattern);
		};

		if (load_btn.clicked()) { panel.load_elements(pattern_gui, pattern); }
		if (save_btn.clicked()) { panel.save_elements(); }
	}
#elif __EMSCRIPTEN__
	void Panel::save_elements() {
		string str;
		for (size_t i = 0; i < elements.size(); i++) {
			for (size_t r = 0; r < elements[i].size(); r++) {
				str += elements[i][r].get_text();
			}
			str += '\n';
		}

		save_file(str.c_str());
	}

	void Panel::load_elements(PatternGui& pattern_gui, Pattern& pattern) {
		paste_elements(web_str.c_str(), pattern_gui, pattern);
		file_loaded_b = false;
	}

	void PanelGui::update(Panel& panel, PatternGui& pattern_gui, Pattern& pattern) {
		if (copy_btn.clicked()) {
			panel.copy();
		}

		if (paste_btn.clicked()) { request_clipboard(); };
		if (clipboard_ready) { clipboard_ready = false; panel.paste_elements(clipboard_str.c_str(), pattern_gui, pattern); }

		if (load_btn.clicked()) { load_file(); }
		if (file_loaded_b) { panel.load_elements(pattern_gui, pattern); }

		if (save_btn.clicked()) { panel.save_elements(); }
	}
#endif 