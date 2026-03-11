#pragma once
#include "raylib_raygui.h"
#include "gui_elements.h"
#include "Pattern.h"

class Panel {
public:
	Panel(Rectangle bounds, float color_factor);

	void draw();
	void set_elements_text(const vector<vector<string>>& elements_s, PatternType pattern_type, DisplayAmount display_amount);
	void copy();
	void paste_elements(const char* txt, PatternGui& pattern_gui, Pattern& pattern);
	void save_elements();
	void load_elements(PatternGui& pattern_gui, Pattern& pattern);

	Rectangle get_bounds() { return bounds; }
private:
	Rectangle bounds;
	ScrollBar* scrollbar = nullptr;

	Color color;
	Color line_color;
	vector<array<TextEx, 10>> elements;
	vector<TextEx> idx_v;

	TextEx container_s;
	TimedText invalid_in;

	float color_factor = 1.0f;

	Font font = GuiGetFont();
	bool bad_paste = false;

	string format_elements() const;
};

class PanelGui {
public:
	PanelGui(Rectangle panel_bounds);
	void draw();
	void update(Panel& panel, PatternGui& pattern_gui, Pattern& pattern);
private:
	Button* copy_btn = nullptr;
	Button* paste_btn = nullptr;

	Button* load_btn = nullptr;
	Button* save_btn = nullptr;
};