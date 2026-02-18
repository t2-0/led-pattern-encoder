#include "App.h"
#include <iostream>

using namespace std;

App::App() {
	InitWindow(800, 600, "LED Pattern Encoder");

	string file = "style/genesis/style_genesis.rgs";
	GuiLoadStyle(file.c_str());

	Circle::init_colors();
	TextEx::init_font();

	pattern = new Pattern();
	panel   = new CopyPanel{ { 420.0f, 40.0f, 150.0f, 192.0f }, 0.75f };
}

void App::run() {
	Color bg_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(bg_color);
		
		pattern->draw();

		panel->set_elements_text(pattern->get_hex());
		panel->draw();
		EndDrawing();
	}
}