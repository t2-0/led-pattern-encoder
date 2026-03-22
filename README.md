LED Pattern Encoder

A C++ application for editing and animating 8x8 LED patterns,
with support for multiple displays (1x / 4x).

Built using raylib + raygui.

------------------------------------------------------------

FEATURES

- Edit 8x8 LED patterns
- Animation system with frame management
- Hexadecimal pattern representation (e.g. { 0x00, ... })
- Copy / paste patterns via text
- Save and load patterns from files

------------------------------------------------------------

USAGE

- Left mouse button: set LED ON
- Right mouse button: set LED OFF
- Holding the mouse button allows continuous editing

------------------------------------------------------------

BUILD

Open the project in Visual Studio and build.

------------------------------------------------------------

RUN

Extract the archive and run LEDPatternEncoder.exe from the /dist folder.

------------------------------------------------------------

PLATFORM

Windows-only (uses windows.h).

------------------------------------------------------------

DESIGN NOTES

- Pattern state is separated into:
  - static (default mode)
  - animation (frame-based)
- Frames are managed independently via FramesManager
- GUI logic is co-located with core classes (e.g., Pattern + PatternGui)

------------------------------------------------------------

EXAMPLE FILES

The repository includes .txt files in example/ demonstrating valid pattern formats.

------------------------------------------------------------

THIRD-PARTY LIBRARIES

This project uses raylib and raygui (zlib/libpng license).
Their licenses are included in the third_party/ directory.

------------------------------------------------------------

LICENSE

This project is licensed under the MIT License.
See LICENSE file for details.
