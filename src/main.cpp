#include "pattern/Pattern.h"
#include "panel/Panel.h"

#ifdef __EMSCRIPTEN__
	#include <emscripten/emscripten.h>
#endif

struct App {
	PatternGui pattern_gui;
	Pattern pattern;

	Panel panel = { { 410.0f, 365.0f, 370.0f, 192.0f }, 0.75f };
	PanelGui panel_gui = { panel.get_bounds() };
};

void update_draw(void* arg) {
	App* app = (App*)arg;

	Color bg_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
	
	BeginDrawing();
	ClearBackground(bg_color);

	app->pattern.draw();
	app->pattern_gui.draw();

	app->pattern.update();
	app->pattern_gui.update(app->pattern);

	if (app->pattern.update_panel()) {
		app->panel.set_elements_text(app->pattern.convert_hex(), app->pattern.get_type(), app->pattern.get_amount());
		app->pattern.reset_update_panel();
	}
	app->panel.draw();

	app->panel_gui.draw();
	app->panel_gui.update(app->panel, app->pattern_gui, app->pattern);

	EndDrawing();
}

int main() {
	InitWindow(800, 600, "LED Pattern Encoder");
	GuiLoadStyleGenesisW();

	Led::init_colors();
	TextEx::init_font();

	App app;

	#ifdef _WIN32
		while (!WindowShouldClose()) { update_draw(&app); }
	#elif __EMSCRIPTEN__
		emscripten_set_main_loop_arg(update_draw, &app, 0, 1);
	#endif 
}