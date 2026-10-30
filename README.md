# Dynamate for the Gamebuino AKA

A port of the GP2X version of Dynamate to the Gamebuino AKA. Dynamate is a puzzle game by Björn
Kalzen and Jonas Norberg, first released on the Amiga in 1995: clear each level of its numbered
pieces by pushing other pieces into them, with pieces that redirect, teleport and change colour
the piece you move.

- the 30 levels of the original, unlocked one by one, with a move record for each
- the level editor, saving to the SD card in the same `.lev` format as the GP2X version
- the GP2X graphics, sounds and title music, at the same 320x240 the AKA's screen has

## Controls

| AKA | In the game | In the menus | In the level editor |
|---|---|---|---|
| D-pad / joystick | move the cursor | choose | move the cursor |
| A + direction | move the piece under the cursor | select | place the chosen piece |
| B + direction | move the piece, the cursor follows it | back | erase (C too) |
| MENU | game menu | close the game / editor menu | editor menu |
| L1 / R1 | – / next music track | – | previous / next piece |
| D + up / down | volume | volume | volume |

Entering a level name: A types the chosen key, B deletes the last character, MENU accepts and
C cancels.

**Quit** in the main menu, or **RUN + MENU** held for half a second anywhere in the game, goes back
to the launcher; RUN on its own switches the AKA off as in every game.

## Installing

Copy the `SD_files/DYNAMATE` folder to the root of the SD card and start Dynamate from the
launcher. The folder holds `firmware.bin`, `meta.json`, `screen.bmp`, the music and the
`LICENSE`. The game
writes its progress there (`dynamate.dat`, `high.dat`, `settings.dat`) and your levels go in
`DYNAMATE/userlevels`. Levels made with the GP2X version can be copied in there as they are.

Extra music: any 44100 Hz mono 16-bit WAV file in `DYNAMATE/music` is played during the game,
`title.wav` in the menus.

## Building

ESP-IDF 5.5 and the official [Gamebuino AKA library](https://github.com/jmp42/Gamebuino_AKA_lib),
which is used where it is checked out (`C:/github/Gamebuino_AKA_lib` by default, or set
`AKA_LIB_DIR`):

```
idf.py set-target esp32s3
idf.py build
```

`build/dynamate.bin` is copied to `SD_files/DYNAMATE/firmware.bin` by the build.

The graphics, sounds and music are the GP2X version's original files in `assets/`, converted by
`tools/convert_assets.py` (Pillow, and ffmpeg built with libopenmpt for the title music):

```
python tools/convert_assets.py <ffmpeg>
```

It writes `main/assets/assets.bin` (built into the firmware) and the SD card folder's music and
`screen.bmp`.

## Source layout

| File | What it is |
|---|---|
| `main/main.cpp` | start up and the state loop |
| `main/intro.cpp`, `titlescreen.cpp`, `leveltypemenu.cpp`, `levelselectmenu.cpp`, `game.cpp`, `ingamemenu.cpp`, `winmenu.cpp`, `diedmenu.cpp`, `leveleditor.cpp`, `leveleditormenu.cpp`, `leveleditorlevelselect.cpp` | one file per game state |
| `main/dialogs.cpp` | the level name entry and the level editor's messages |
| `main/gamefuncs.cpp`, `commonvars.cpp`, `savestate.cpp` | what the states share, the globals, the files on the card |
| `main/dynamate_base.cpp`, `levels.cpp` | the original game engine by Jonas Norberg, and the levels |
| `main/gfx.cpp`, `sound.cpp`, `input.cpp`, `platform.cpp` | what SDL did on the GP2X, on the AKA library |

## Credits

Dynamate was created by Björn Kalzen and Jonas Norberg. GP2X version and this port by Willems
Davy (Willems Soft). SFont by Karl Bartel.

## License

LGPL 2.1, see [LICENSE](LICENSE), as the SFont code the font drawing in `main/gfx.cpp` comes
from is LGPL.
