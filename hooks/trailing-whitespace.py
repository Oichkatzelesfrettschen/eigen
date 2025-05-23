#!/usr/bin/env python3
import sys, pathlib, re

for path_str in sys.argv[1:]:
    p = pathlib.Path(path_str)
    if not p.exists():
        continue
    text = p.read_text()
    lines = [re.sub(r'[ \t]+$', '', line) for line in text.splitlines()]
    if text.endswith('\n'):
        new_text = '\n'.join(lines) + '\n'
    else:
        new_text = '\n'.join(lines)
    p.write_text(new_text)
