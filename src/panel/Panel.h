#pragma once
#include "raylib_raygui.h"
#include "gui/gui_elements.h"
#include "pattern/Pattern.h"

// Displays and manages a textual (hexadecimal) representation of the LED pattern.
//
// Responsibilities:
// - Render pattern data as hexadecimal values
// - Allow copying/pasting pattern data (text format)
// - Load/save pattern data from/to external sources
//
// Interaction:
// - Receives data from Pattern (via set_elements_text)
// - Sends modified data back to Pattern (via paste/load)
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
	ScrollBar scrollbar;

	Color color;
	Color line_color;
	// Text representation of pattern rows in hex format:
	// { value0, value1, ..., value7 }
	// Each row consists of:
	// - 1 opening brace '{'
	// - 8 hex values (from Pattern)
	// - 1 closing brace '}'
	vector<array<TextEx, 10>> elements;
	vector<TextEx> idx_v;

	TextEx container_s;
	TimedText invalid_in;

	float color_factor = 1.0f;

	Font font = GuiGetFont();

	string format_elements() const;
};

class PanelGui {
public:
	PanelGui(Rectangle panel_bounds);
	void draw();
	void update(Panel& panel, PatternGui& pattern_gui, Pattern& pattern);
private:
	Button copy_btn;
	Button paste_btn;

	Button load_btn;
	Button save_btn;
};