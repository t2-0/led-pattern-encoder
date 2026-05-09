#include "web.h"

#ifdef __EMSCRIPTEN__
	#include <emscripten/emscripten.h>

	EM_JS(void, load_file_impl, (), {
		const input = document.createElement('input');

		input.type = 'file';
		input.accept = '.txt';
		input.onchange = function(event) {
			const file = event.target.files[0];

			file.text().then(function(text) {
				const len = lengthBytesUTF8(text) + 1;
				const ptr = _malloc(len);

				stringToUTF8(text, ptr, len);

				Module.ccall('file_loaded', null,['number'],[ptr]);
			})
		};

		input.click();
	});

	EM_JS(void, save_file_impl, (const char* content), {
		const file_name = "";
		const content_str = UTF8ToString(content);

		async function save_file() {
			const handle = await window.showSaveFilePicker({
				suggestedName: file_name,
				types  : [{ description: 'Text Files',
				accept : {'text/plain': ['.txt'] },}] ,
				});

				const writable = await handle.createWritable();
				await writable.write(content_str);
				await writable.close();
		}

		save_file();
	});


	EM_JS(void, request_clipboard_impl, (), {
		navigator.clipboard.readText().then(function(text) {

			const len = lengthBytesUTF8(text) + 1;
			const ptr = _malloc(len);

			stringToUTF8(text, ptr, len);

			Module.ccall('clipboard_pasted', null, ['number'], [ptr]);
		});
	});

	void load_file() {
		load_file_impl();
	}

	void save_file(const char* content) {
		save_file_impl(content);
	}

	void request_clipboard() {
		request_clipboard_impl();
	}

#endif 