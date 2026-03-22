#include "pattern/Pattern.h"
#include "panel/Panel.h"

int main() {
	InitWindow(800, 600, "LED Pattern Encoder");
	string file = "style/genesis/style_genesis.rgs";
	GuiLoadStyle(file.c_str());

	Led::init_colors();
	TextEx::init_font();

	PatternGui pattern_gui;
	Pattern pattern;

	Panel panel = { { 410.0f, 365.0f, 370.0f, 192.0f }, 0.75f };
	PanelGui panel_gui = { panel.get_bounds() };

	Color bg_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(bg_color);

		pattern.draw();
		pattern_gui.draw();
		
		pattern.update();
		pattern_gui.update(pattern);

		if (pattern.update_panel()) {
			panel.set_elements_text(pattern.convert_hex(), pattern.get_type(), pattern.get_amount());
			pattern.reset_update_panel(); 
		}
		panel.draw();

		panel_gui.draw();
		panel_gui.update(panel, pattern_gui, pattern);

		EndDrawing();
	}
}