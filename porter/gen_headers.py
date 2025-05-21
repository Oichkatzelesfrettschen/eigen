#!/usr/bin/env python3
"""Generate C headers from mapping.yaml."""
import os
import re
import sys

try:
    import yaml  # type: ignore
    HAVE_YAML = True
except Exception:  # pragma: no cover - optional dependency
    import json
    HAVE_YAML = False

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MAPPING_PATH = os.path.join(REPO_ROOT, "porter", "mapping.yaml")
OUT_DIR = os.path.join(REPO_ROOT, "eigenc", "include")
OUT_PATH = os.path.join(OUT_DIR, "ec_generated.h")

MATRIX_RE = re.compile(r"Matrix<\s*([^,>]+)\s*,\s*([^,>]+)\s*,\s*([^,>]+)\s*>")

CTYPE_MAP = {
    "float": "float",
    "double": "double",
    "int": "int",
}

def parse_mapping(path):
    with open(path, "r", encoding="utf-8") as f:
        if HAVE_YAML:
            data = yaml.safe_load(f) or {}
        else:
            # very small YAML subset parser
            data = {}
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                if line == 'mappings:':
                    continue
                if ':' in line:
                    k, v = line.split(':', 1)
                    data[k.strip()] = v.strip()
        mappings = data.get("mappings", data)
    return mappings

def parse_spec(spec):
    m = MATRIX_RE.match(spec)
    if not m:
        raise ValueError(f"Unrecognized template specification: {spec}")
    dtype, rows, cols = [s.strip() for s in m.groups()]
    ctype = CTYPE_MAP.get(dtype.split("::")[-1], "double")
    return ctype, rows, cols

def gen_header(mappings):
    lines = []
    lines.append("#ifndef EC_GENERATED_H")
    lines.append("#define EC_GENERATED_H")
    lines.append("#include <stddef.h>")
    lines.append("#include <assert.h>")
    lines.append("")

    fn_add = {}
    fn_mul = {}

    for spec, cname in mappings.items():
        ctype, rows, cols = parse_spec(spec)
        lines.append(f"typedef struct {{")
        lines.append("    size_t rows;")
        lines.append("    size_t cols;")
        lines.append(f"    {ctype} *data;")
        lines.append(f"}} {cname};")
        lines.append("")

        add_fn = f"{cname}_add"
        lines.append(f"static inline void {add_fn}(const {cname} *a, const {cname} *b, {cname} *out) {{")
        lines.append("    assert(a->rows == b->rows && a->cols == b->cols && a->rows == out->rows && a->cols == out->cols);")
        lines.append("    for (size_t i = 0; i < a->rows * a->cols; ++i)")
        lines.append("        out->data[i] = a->data[i] + b->data[i];")
        lines.append("}")
        lines.append("")

        mul_fn = f"{cname}_mul"
        lines.append(f"static inline void {mul_fn}(const {cname} *a, const {cname} *b, {cname} *out) {{")
        lines.append("    assert(a->cols == b->rows && a->rows == out->rows && b->cols == out->cols);")
        lines.append("    for (size_t i = 0; i < a->rows; ++i) {")
        lines.append("        for (size_t j = 0; j < b->cols; ++j) {")
        lines.append(f"            {ctype} sum = 0;")
        lines.append("            for (size_t k = 0; k < a->cols; ++k)")
        lines.append("                sum += a->data[i * a->cols + k] * b->data[k * b->cols + j];")
        lines.append("            out->data[i * out->cols + j] = sum;")
        lines.append("        }")
        lines.append("    }")
        lines.append("}")
        lines.append("")

        fn_add[cname] = add_fn
        fn_mul[cname] = mul_fn

    # Generic dispatch macros
    lines.append("#define ec_add(A,B,OUT) \\")
    lines.append("    _Generic((A), \\")
    pairs = []
    for cname, fn in fn_add.items():
        pairs.append(f"{cname}*: {fn}")
        pairs.append(f"const {cname}*: {fn}")
    for idx, pair in enumerate(pairs):
        comma = "," if idx < len(pairs) - 1 else ""
        lines.append(f"        {pair}{comma} \\")
    lines.append("    )(A,B,OUT)")
    lines.append("")

    lines.append("#define ec_mul(A,B,OUT) \\")
    lines.append("    _Generic((A), \\")
    pairs = []
    for cname, fn in fn_mul.items():
        pairs.append(f"{cname}*: {fn}")
        pairs.append(f"const {cname}*: {fn}")
    for idx, pair in enumerate(pairs):
        comma = "," if idx < len(pairs) - 1 else ""
        lines.append(f"        {pair}{comma} \\")
    lines.append("    )(A,B,OUT)")
    lines.append("")

    lines.append("#endif /* EC_GENERATED_H */")

    return "\n".join(lines)


def main():
    mappings = parse_mapping(MAPPING_PATH)
    os.makedirs(OUT_DIR, exist_ok=True)
    header = gen_header(mappings)
    with open(OUT_PATH, "w", encoding="utf-8") as f:
        f.write(header)
    print(f"Written {OUT_PATH}")

if __name__ == "__main__":
    main()
