#pragma once

#include <string>

using std::string;

struct HWND__;
typedef HWND__* HWND;

string load_file(HWND owner);
void save_file(HWND owner, string str);