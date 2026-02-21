#define RAYGUI_IMPLEMENTATION
#include "raylib_raygui.h"

int GuiScrollBarW(Rectangle bounds, int value, int minValue, int maxValue) {
	return GuiScrollBar(bounds, value, minValue, maxValue); 
}