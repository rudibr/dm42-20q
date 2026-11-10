#!/usr/bin/env python3
"""Renders an animated GIF of a real 20Q game (docs/20q-demo.gif).

The game itself is played by the real engine (tools/demo_game.c); this
script only draws the screens the way src/main.c lays them out on the
DM42's 400x240 display.

  python3 tools/make_gif.py [object] [seed]
"""
import os, subprocess, sys
from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.normpath(os.path.join(os.path.dirname(__file__), ".."))
W, H, SCALE = 400, 240, 2
BG, FG = (214, 219, 208), (24, 26, 24)          # LCD colours
FONT = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
f20 = ImageFont.truetype(FONT, 15)
fsmall = ImageFont.truetype(FONT, 12)
f24 = ImageFont.truetype(FONT, 19)
fbig = ImageFont.truetype(FONT, 30)
fhuge = ImageFont.truetype(FONT, 40)
HDR, MENU = 22, 32                               # title bar / menu heights
CATS = ["Animal", "Plant", "Thing", "Other"]

def tw(d, s, f): return d.textlength(s, font=f)

def center(d, y, s, f): d.text(((W - tw(d, s, f)) / 2, y), s, font=f, fill=FG)

def wrap(d, y, s, f):
    words, line, lh = s.split(), "", f.size + 6
    for w in words:
        t = (line + " " + w).strip()
        if line and tw(d, t, f) > W - 16:
            center(d, y, line, f); y += lh; line = w
        else:
            line = t
    if line: center(d, y, line, f); y += lh
    return y

def header(d, left, right=""):
    d.rectangle([0, 0, W, HDR - 1], fill=FG)
    d.text((4, 2), left, font=f20, fill=BG)
    if right: d.text((W - tw(d, right, f20) - 4, 2), right, font=f20, fill=BG)

def menu(d, labels, pressed=None):
    kw = W / 6
    for i in range(6):
        x0, x1 = int(i * kw) + 1, int((i + 1) * kw) - 2
        y0, y1 = H - MENU + 4, H - 2
        inv = (i == pressed)
        d.rounded_rectangle([x0, y0, x1, y1], 3, fill=BG if inv else FG, outline=FG)
        lab = labels[i] if i < len(labels) else ""
        if lab:
            f = f20 if tw(d, lab, f20) <= x1 - x0 - 4 else fsmall
            d.text(((x0 + x1 - tw(d, lab, f)) / 2, y0 + 5 + (f20.size - f.size) // 2), lab, font=f,
                   fill=FG if inv else BG)

def hint(d, s): center(d, H - MENU - 22, s, f20)

def screen():
    im = Image.new("RGB", (W, H), BG)
    return im, ImageDraw.Draw(im)

def title(nobj, nq):
    im, d = screen()
    header(d, "20Q - Twenty Questions", "v1.0")
    center(d, HDR + 10, "20Q", fbig)
    y = wrap(d, HDR + 52, "Think of an animal, a plant, a thing or something else.", f24)
    center(d, y + 4, f"I know {nobj} things and {nq} questions.", f20)
    hint(d, "Press any key to start")
    menu(d, ["Play", "", "", "", "About", "Setup"])
    return im

def category(pressed=None):
    im, d = screen()
    header(d, "20Q", "Question 1")
    center(d, HDR + 40, "Is it Animal, Vegetable,", f24)
    center(d, HDR + 65, "Thing or Other?", f24)
    hint(d, "Vegetable = any plant.  Other = food, place, ...")
    menu(d, CATS + ["Unsure", ""], pressed)
    return im

ANS = {"Yes": 0, "No": 1, "Maybe": 2, "Unknown": 3}

def question(n, text, pressed=None):
    im, d = screen()
    header(d, "20Q", f"Question {n}")
    lines = 1 if tw(d, text, f24) <= W - 16 else 2
    y = (HDR + 10 + H - MENU - 24) / 2 - lines * 25 / 2 - 6
    wrap(d, y, text, f24)
    menu(d, ["Yes", "No", "Maybe", "Unknown", "Undo", "Guess!"], pressed)
    return im

def guess(n, name, pressed=None):
    im, d = screen()
    header(d, "20Q", f"After {n} questions")
    center(d, HDR + 28, "I am guessing that it is...", f24)
    s = name.upper() + "?"
    f = fhuge if tw(d, s, fhuge) < W - 10 else fbig
    center(d, HDR + 70, s, f)
    menu(d, ["Right", "Wrong", "", "", "", ""], pressed)
    return im

def win(n):
    im, d = screen()
    header(d, "I win!")
    y = wrap(d, HDR + 40, "Got it!", fbig) + 8
    wrap(d, y, f"I guessed it in {n} questions.", f24)
    hint(d, "Press any key")
    menu(d, ["", "", "", "", "", ""])
    return im

def main():
    target = sys.argv[1] if len(sys.argv) > 1 else "giraffe"
    seed = sys.argv[2] if len(sys.argv) > 2 else "1"
    exe = "/tmp/q20demo"
    subprocess.run(["gcc", "-O2", "-I" + ROOT + "/src", ROOT + "/tools/demo_game.c",
                    ROOT + "/src/q20.c", "-lm", "-o", exe], check=True)
    out = subprocess.run([exe, target, seed], capture_output=True, text=True, check=True).stdout

    frames = []                                   # (image, milliseconds)
    for ln in out.splitlines():
        tag, *rest = ln.split(" ", 1)
        rest = rest[0] if rest else ""
        if tag == "INFO":
            nobj, nq = rest.split()
            frames += [(title(nobj, nq), 2600)]
        elif tag == "CAT":
            c = int(rest)
            frames += [(category(), 1600), (category(c), 350)]
        elif tag == "Q":
            n, ans, text = rest.split(" ", 2)
            frames += [(question(n, text), 1250), (question(n, text, ANS[ans]), 300)]
        elif tag == "G":
            n, res, name = rest.split(" ", 2)
            p = 0 if res == "right" else 1
            frames += [(guess(n, name), 2200), (guess(n, name, p), 400)]
        elif tag == "END":
            how, n = rest.split()
            if how == "win": frames += [(win(n), 3200)]

    os.makedirs(os.path.join(ROOT, "docs"), exist_ok=True)
    path = os.path.join(ROOT, "docs", "20q-demo.gif")
    imgs = [f.resize((W * SCALE, H * SCALE), Image.NEAREST).quantize(colors=8) for f, _ in frames]
    imgs[0].save(path, save_all=True, append_images=imgs[1:],
                 duration=[ms for _, ms in frames], loop=0, optimize=True)
    imgs[0].convert("RGB").save(os.path.join(ROOT, "docs", "20q-title.png"))
    print(f"{path}: {len(frames)} frames, {sum(ms for _, ms in frames)/1000:.1f} s, "
          f"{os.path.getsize(path)//1024} KB")

main()
