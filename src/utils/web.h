#pragma once
#ifdef __EMSCRIPTEN__
	void load_file();
	void save_file(const char* content);
	void request_clipboard();
#endif