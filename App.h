#pragma once
#include "Pattern.h"
#include "gui_elements.h"

class App {
public:
	App();

	void run();
private:
	Pattern* pattern = nullptr;
	CopyPanel* panel = nullptr;
};