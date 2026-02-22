#include "Pattern.h"

int main() {
	InitWindow(800, 600, "LED Pattern Encoder");

	string file = "style/genesis/style_genesis.rgs";
	GuiLoadStyle(file.c_str());

	Led::init_colors();
	TextEx::init_font();

	PatternGui pattern_gui;
	Pattern pattern;

	CopyPanel panel = { { 385.0f, 40.0f, 150.0f, 192.0f }, 0.75f };

	Color bg_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(bg_color);

		pattern.draw();
		pattern_gui.draw();
		
		pattern.update();
		pattern_gui.update(pattern);

		if (pattern.update_panel()) { panel.set_elements_text(pattern.get_hex()); }
		panel.draw();

		EndDrawing();
	}
}