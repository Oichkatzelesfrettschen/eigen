#!/usr/bin/env python3
import sys, pathlib

for path_str in sys.argv[1:]:
    p = pathlib.Path(path_str)
    if not p.exists():
        continue
    data = p.read_bytes()
    if not data.endswith(b"\n"):
        p.write_bytes(data + b"\n")
