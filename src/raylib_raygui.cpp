#define RAYGUI_IMPLEMENTATION
#include "raylib_raygui.h"
#include "genesis/style_genesis.h"

int GuiScrollBarW(Rectangle bounds, int value, int minValue, int maxValue) {
	return GuiScrollBar(bounds, value, minValue, maxValue); 
}

void GuiLoadStyleGenesisW() { GuiLoadStyleGenesis(); }