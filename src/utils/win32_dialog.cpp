#include "win32_dialog.h"
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shobjidl.h>

#include <sstream>
#include <fstream>

using namespace std;

string load_file(HWND owner) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    string str;

    if (SUCCEEDED(hr)) {
        IFileOpenDialog* file_open = nullptr;
        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_open));

        if (SUCCEEDED(hr)) {
            COMDLG_FILTERSPEC types[] = { { L"Text Documents", L"*.txt" }, { L"All Files", L"*.*" } };

            file_open->SetFileTypes(2, types);
            hr = file_open->Show(owner);
            if (SUCCEEDED(hr)) {
                IShellItem* item = nullptr;
                hr = file_open->GetResult(&item);

                if (SUCCEEDED(hr)) {
                    PWSTR path = nullptr;
                    item->GetDisplayName(SIGDN_FILESYSPATH, &path);

                    ifstream ifs{ path };
                    string line;
                    while (getline(ifs, line)) {
                        str += line;
                        str += '\n';
                    }

                    CoTaskMemFree(path);
                    item->Release();
                }
            }
            file_open->Release();
        }
        CoUninitialize();
    }

    return str;
}

void save_file(HWND owner, string str) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr)) {
        IFileSaveDialog* file_save = nullptr;
        hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_save));

        if (SUCCEEDED(hr)) {
            COMDLG_FILTERSPEC types[] = { { L"Text Documents", L"*.txt" }, { L"All Files", L"*.*" } };
            file_save->SetFileTypes(2, types);

            file_save->SetDefaultExtension(L"txt");
            hr = file_save->Show(owner);
            if (SUCCEEDED(hr)) {
                IShellItem* item = nullptr;
                hr = file_save->GetResult(&item);

                if (SUCCEEDED(hr)) {
                    PWSTR path = nullptr;
                    item->GetDisplayName(SIGDN_FILESYSPATH, &path);

                    ofstream ofs{ path };
                    if (ofs) { ofs << str; }

                    CoTaskMemFree(path);
                    item->Release();
                }
            }
            file_save->Release();
        }
        CoUninitialize();
    }
}