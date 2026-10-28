#!/usr/bin/env python3
"""Converts the assets of the GP2X version of Dynamate into what the Gamebuino AKA build uses.

    python tools/convert_assets.py <ffmpeg.exe>

Reads the GP2X version's original files from assets/ (graphics/, sound/, music/title.mod and
font.ttf). Writes:
  main/assets/assets.bin   every image, font and sound effect, in the LCD's own pixel format
  main/assets/assets.h     where each of them sits in assets.bin
  SD_files/DYNAMATE/       the folder that goes on the SD card: music, meta.json, screen.bmp

Images become 16 bit pixels in the order the AKA's panel takes them (lcd_color_rgb in the
library: red in the low bits), with an 8 bit alpha plane when the image is not fully opaque.
Magenta (255,0,255) is the colour key of the original, it becomes alpha 0. The menu and the
level title were drawn with a surface alpha of 230 on top of their colour key, that is baked
into their alpha plane.

The TrueType font (font.ttf at 12 pixels, what TTF_OpenFont(…, 12) gave) is rendered once
here into an alpha bitmap per character, the AKA draws those in any colour.

Sounds are resampled to 44100 Hz mono 16 bit, the only format the library's WAV player takes;
the music is rendered from the original title.mod by ffmpeg (libopenmpt) and written with the
plain 44 byte WAV header the library's player checks for.
"""
import os
import struct
import subprocess
import sys

from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC_DIR = os.path.join(ROOT, "assets")
OUT_DIR = os.path.join(ROOT, "main", "assets")
SD_DIR = os.path.join(ROOT, "SD_files", "DYNAMATE")

# name in the code, file, alpha for the non magenta pixels when the image is colour keyed
IMAGES = [
    ("PIECES", "pieces.png", 255),
    ("CURSOR", "cursor.png", 255),
    ("BACKGROUND", "background.png", None),
    ("BACKGROUNDLEVELEDITOR", "backgroundleveleditor.png", None),
    ("MENU", "menu.png", 230),
    ("LEVELTITLE", "leveltitle.png", 230),
    ("UNLOCKED", "unlocked.png", None),
    ("LOCKED", "locked.png", None),
    ("PIECEOUTSIDEPLAYFIELD", "pieceoutsideplayfield.png", None),
    ("INFINITELOOP", "infiniteloop.png", None),
    ("DEATHPIECE", "deathpiece.png", None),
    ("OUTOFMOVES", "outofmoves.png", None),
    ("LEVELCOMPLETED", "levelcompleted.png", None),
    ("ALLLEVELSCOMPLETED", "alllevelscompleted.png", None),
    ("TITLESCREEN", "titlescreen.png", None),
    ("INTRO1", "intro1.png", None),
    ("INTRO2", "intro2.png", None),
    ("INTRO3", "intro3.png", None),
    ("INTRO4", "intro4.png", None),
]

SFONTS = [("FONTDARK", "darkfont14pt.png"), ("FONTLIGHT", "lightfont14pt.png")]

# same order as the SND_ defines in defines.h
SOUNDS = ["menu", "select", "error", "explode", "move", "teleport", "passlevel", "fail"]

TTF_SIZE = 12


def lcd_color(r, g, b):
    return (r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11)


class Blob:
    def __init__(self):
        self.data = bytearray()

    def align(self):
        while len(self.data) % 4:
            self.data.append(0)

    def add(self, raw):
        self.align()
        offset = len(self.data)
        self.data += raw
        return offset


def surface_planes(im, key_alpha):
    """pixels and alpha plane (None when opaque) of an image"""
    rgba = im.convert("RGBA")
    w, h = rgba.size
    pixels = bytearray()
    alpha = bytearray()
    opaque = True
    keyed = im.mode != "RGBA" and key_alpha is not None
    for r, g, b, a in rgba.getdata():
        if keyed:
            a = 0 if (r, g, b) == (255, 0, 255) else key_alpha
        if a != 255:
            opaque = False
        pixels += struct.pack("<H", lcd_color(r, g, b) if a else 0)
        alpha.append(a)
    return w, h, bytes(pixels), (None if opaque else bytes(alpha))


def add_surface(blob, w, h, pixels, alpha):
    px = blob.add(pixels)
    a = blob.add(alpha) if alpha else -1
    return "{%d, %d, %d, %d}" % (w, h, px, a)


def sfont_charpos(im):
    """SFont_InitFont: the pink runs in the top row mark where each character is"""
    rgba = im.convert("RGBA")
    w = rgba.size[0]
    pos = []
    x = 0
    while x < w:
        if rgba.getpixel((x, 0))[:3] == (255, 0, 255):
            pos.append(x)
            while x < w and rgba.getpixel((x, 0))[:3] == (255, 0, 255):
                x += 1
            pos.append(x)
        x += 1
    return pos, x - 1


def ttf_glyphs(blob, path):
    font = ImageFont.truetype(path, TTF_SIZE)
    ascent, descent = font.getmetrics()
    height = ascent + descent
    entries = []
    for code in range(32, 127):
        ch = chr(code)
        advance = int(round(font.getlength(ch)))
        left, _, right, _ = font.getbbox(ch, anchor="la")
        x0 = min(0, left)
        gw = max(advance, right) - x0
        if gw <= 0 or ch == " ":
            entries.append("{%d, 0, %d, -1}" % (advance, 0))
            continue
        img = Image.new("L", (gw, height), 0)
        ImageDraw.Draw(img).text((-x0, 0), ch, font=font, fill=255, anchor="la")
        off = blob.add(img.tobytes())
        entries.append("{%d, %d, %d, %d}" % (advance, gw, x0, off))
    return height, font.font.height, entries


def ffmpeg_pcm(ffmpeg, path):
    """the file as 44100 Hz mono signed 16 bit samples"""
    return subprocess.run([ffmpeg, "-hide_banner", "-loglevel", "error", "-i", path,
                           "-ac", "1", "-ar", "44100", "-f", "s16le", "-acodec", "pcm_s16le", "-"],
                          check=True, stdout=subprocess.PIPE).stdout


def write_wav(path, pcm):
    with open(path, "wb") as f:
        f.write(struct.pack("<4sI4s4sIHHIIHH4sI", b"RIFF", 36 + len(pcm), b"WAVE", b"fmt ", 16, 1, 1,
                            44100, 88200, 2, 16, b"data", len(pcm)))
        f.write(pcm)


def write_bmp(path, im):
    """24 bit BMP, what the launcher shows next to the game's name"""
    im = im.convert("RGB")
    w, h = im.size
    row = (w * 3 + 3) & ~3
    with open(path, "wb") as f:
        f.write(struct.pack("<2sIHHI", b"BM", 54 + row * h, 0, 0, 54))
        f.write(struct.pack("<IiiHHIIiiII", 40, w, h, 1, 24, 0, row * h, 2835, 2835, 0, 0))
        for y in range(h - 1, -1, -1):
            line = bytearray()
            for x in range(w):
                r, g, b = im.getpixel((x, y))
                line += bytes((b, g, r))
            line += b"\0" * (row - w * 3)
            f.write(line)


def main():
    if len(sys.argv) != 2:
        print(__doc__)
        sys.exit(1)
    src, ffmpeg = SRC_DIR, sys.argv[1]
    gfx = os.path.join(src, "graphics")
    os.makedirs(OUT_DIR, exist_ok=True)
    blob = Blob()
    h = ["// Generated by tools/convert_assets.py from the GP2X version's graphics and sounds.",
         "// Offsets are into assets.bin (dm_assets), -1 = not there.", "#pragma once", "#include <stdint.h>", "",
         "struct AssetSurface { int16_t w, h; int32_t pixels, alpha; };",
         "struct AssetSFont { AssetSurface surface; int16_t maxPos; int16_t count; const int16_t* charPos; };",
         "struct AssetGlyph { int8_t advance; int8_t w; int8_t x; int32_t alpha; };",
         "struct AssetSound { int32_t samples; uint32_t count; };", ""]

    for name, file, key_alpha in IMAGES:
        w, hh, px, a = surface_planes(Image.open(os.path.join(gfx, file)), key_alpha)
        h.append("static const AssetSurface ASSET_%s = %s;" % (name, add_surface(blob, w, hh, px, a)))
    h.append("")

    for name, file in SFONTS:
        im = Image.open(os.path.join(gfx, file))
        pos, max_pos = sfont_charpos(im)
        # SFont blits from row 1 down, the marker row is never drawn
        body = im.convert("RGBA").crop((0, 1, im.size[0], im.size[1]))
        w, hh, px, a = surface_planes(body, None)
        h.append("static const int16_t %s_CHARPOS[] = {%s};" % (name, ", ".join(map(str, pos))))
        h.append("static const AssetSFont ASSET_%s = {%s, %d, %d, %s_CHARPOS};"
                 % (name, add_surface(blob, w, hh, px, a), max_pos, len(pos), name))
    h.append("")

    height, line_skip, glyphs = ttf_glyphs(blob, os.path.join(src, "font.ttf"))
    h.append("// font.ttf at %d pixels, characters 32 to 126" % TTF_SIZE)
    h.append("static const int TTF_HEIGHT = %d;" % height)
    h.append("static const int TTF_LINESKIP = %d;" % line_skip)
    h.append("static const AssetGlyph TTF_GLYPHS[] = {\n    %s};" % ",\n    ".join(glyphs))
    h.append("")

    sounds = []
    for s in SOUNDS:
        pcm = ffmpeg_pcm(ffmpeg, os.path.join(src, "sound", s + ".wav"))
        sounds.append("{%d, %d}" % (blob.add(pcm), len(pcm) // 2))
    h.append("static const AssetSound ASSET_SOUNDS[] = {%s};" % ", ".join(sounds))
    h.append("")

    with open(os.path.join(OUT_DIR, "assets.bin"), "wb") as f:
        f.write(blob.data)
    with open(os.path.join(OUT_DIR, "assets.h"), "w", newline="\n") as f:
        f.write("\n".join(h))
    print("assets.bin: %d bytes" % len(blob.data))

    os.makedirs(os.path.join(SD_DIR, "music"), exist_ok=True)
    os.makedirs(os.path.join(SD_DIR, "userlevels"), exist_ok=True)
    write_wav(os.path.join(SD_DIR, "music", "title.wav"), ffmpeg_pcm(ffmpeg, os.path.join(src, "music", "title.mod")))
    write_bmp(os.path.join(SD_DIR, "screen.bmp"), Image.open(os.path.join(gfx, "titlescreen.png")).resize((160, 120), Image.LANCZOS))
    print("SD folder written to %s" % SD_DIR)


if __name__ == "__main__":
    main()
