#pragma once
#include "raylib_raygui.h"
#include "gui_elements.h"
#include "Pattern.h"

class CopyPanel {
public:
	CopyPanel(Rectangle bounds, float color_factor);

	void draw();
	void set_elements_text(const vector<vector<string>>& elements_s, PatternType pattern_type, DisplayAmount display_amount);
	void paste_elements(PatternGui& pattern_gui, Pattern& pattern);

	string format_elements() const;

	Rectangle get_bounds() { return bounds; }
private:
	Rectangle bounds;
	ScrollBar* scrollbar = nullptr;

	Color color;
	Color line_color;
	vector<array<TextEx, 10>> elements;
	vector<TextEx> idx_v;

	TextEx container_s;

	float color_factor = 1.0f;

	Font font = GuiGetFont();
	int y_offset = 1;
	bool bad_paste = false;
};

class CopyPanelGui {
public:
	CopyPanelGui(Rectangle panel_bounds);
	void draw();
	void update(CopyPanel& panel, PatternGui& pattern_gui, Pattern& pattern);
private:
	Button* copy_btn = nullptr;
	Button* paste_btn = nullptr;

	Button* load_btn = nullptr;
	Button* save_btn = nullptr;
};