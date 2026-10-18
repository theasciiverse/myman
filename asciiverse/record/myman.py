"""MyMan preview: the first maze played by a simple bot.

The legal notice MyMan shows at every start is paged through with Space,
the start screen is left with Enter, and then the bot plays: on
each look at the screen it finds MyMan (the bright yellow sprite inside
the maze), the ghosts (A, or M while they are frightened) and the dots, and steers along the shortest
path to the nearest dot or power pellet that keeps clear of the ghosts,
or towards a frightened ghost. With nowhere safe to
go it runs from the nearest ghost. The maze scrolls to follow MyMan, so
the bot only plans within what is on screen. After about 45 seconds it
quits with q.

    python3 asciiverse/record/myman.py
"""
import os, sys, time
from collections import deque

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rec

LENGTH = 45.0
# what the small tile set draws walls and the ghost house with
WALLS = set("═║╔╗╚╝╓╖╙╜╒╕╘╛┌┐└┘│─├┤┬┴┼╞╡╟╢╤╥╧╨╠╣╦╩╬")
FOOD = set("·o")
SCARED = {"blue", "brightblue", "white"}
MOVES = {(-1, 0): b"k", (1, 0): b"j", (0, -2): b"h", (0, 2): b"l"}


def cells(r):
    for y in range(r.screen.lines):
        row = r.screen.buffer[y]
        for x in range(r.screen.columns):
            c = row[x]
            yield y, x, c.data, c.fg


def look(r):
    """MyMan's tile, the maze's left edge, the ghosts and the food."""
    left = min((x for y, x, ch, fg in cells(r) if ch in WALLS), default=0)
    me, ghosts, food = None, [], set()
    for y, x, ch, fg in cells(r):
        if x < left:
            continue  # score and lives sit left of the maze
        if fg == "brightbrown" and ch not in WALLS and ch not in FOOD and ch != " ":
            me = (y, x)
        elif ch in "AM":
            ghosts.append((y, x, ch == "M" or fg in SCARED))
        elif ch in FOOD and (x - left) % 2 == 0:
            food.add((y, x))
    return me, left, ghosts, food


def passable(r, y, x, left):
    if not (0 <= y < r.screen.lines and left <= x < r.screen.columns):
        return False
    c = r.screen.buffer[y][x]
    return c.data not in WALLS


def plan(r):
    me, left, ghosts, food = look(r)
    if me is None:
        return None
    y, x = me
    if (x - left) % 2:
        x -= 1  # between two tiles: start from the left one
    danger = set()
    for gy, gx, scared in ghosts:
        if scared:
            continue
        gx -= (gx - left) % 2
        for dy in range(-2, 3):
            for dx in range(-4, 5, 2):
                if abs(dy) + abs(dx) // 2 <= 2:
                    danger.add((gy + dy, gx + dx))
    targets = set(food) | {(gy, gx - (gx - left) % 2) for gy, gx, s in ghosts if s}
    seen = {(y, x): None}
    q = deque([(y, x)])
    while q:
        cy, cx = q.popleft()
        if (cy, cx) in targets and (cy, cx) != (y, x):
            while seen[(cy, cx)] != (y, x):
                cy, cx = seen[(cy, cx)]
            return MOVES[(cy - y, cx - x)]
        for dy, dx in MOVES:
            n = (cy + dy, cx + dx)
            if n in seen or n in danger or not passable(r, *n, left):
                continue
            seen[n] = (cy, cx)
            q.append(n)
    # nowhere safe: step away from the nearest ghost
    best, key = -1, None
    for (dy, dx), k in MOVES.items():
        n = (y + dy, x + dx)
        if not passable(r, *n, left):
            continue
        d = min((abs(n[0] - gy) + abs(n[1] - gx) // 2 for gy, gx, s in ghosts if not s), default=99)
        if d > best:
            best, key = d, k
    return key


def main():
    r = rec.Rec("myman", "MyMan")
    r.wait_for("LEGAL NOTICE")
    r.pump(1.0)
    while "P U S H" not in r.text():
        r.key(b" ", 0.6)
    r.pump(1.0)
    r.key(rec.ENTER, 0.3)
    last, sent = None, 0.0
    while r.elapsed() < LENGTH:
        k = plan(r)
        # a new direction at once, the same one again now and then
        if k and (k != last or time.time() - sent > 0.5):
            r.key(k, 0.0)
            last, sent = k, time.time()
        r.pump(0.05)
    r.finish(b"q")


main()
