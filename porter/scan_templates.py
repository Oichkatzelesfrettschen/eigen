#!/usr/bin/env python3
"""Scan Eigen headers and build mapping of template instantiations.

This script walks the Eigen/ directory, parses each header with clang's
Python bindings, dumps the AST to porter/asts/<file>.yaml, and records
any ClassTemplateDecl or FunctionTemplateDecl names.

It outputs a YAML mapping file mapping template names to placeholder C
families which can be filled in later.
"""

import os
import sys
import re

try:
    import yaml

    HAVE_YAML = True
except Exception:  # pragma: no cover - optional dependency
    import json

    HAVE_YAML = False

try:
    from clang.cindex import Index, Config, CursorKind
except Exception as exc:  # pragma: no cover - better error message
    raise SystemExit(
        "clang Python bindings not found; run setup.sh to install them"
    ) from exc

# Allow overriding libclang path via env var
LIBCLANG_PATH = os.environ.get("LIBCLANG_PATH")
if LIBCLANG_PATH:
    Config.set_library_file(LIBCLANG_PATH)

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EIGEN_DIR = os.path.join(REPO_ROOT, "Eigen")
AST_DIR = os.path.join(REPO_ROOT, "porter", "asts")
MAPPING_PATH = os.path.join(REPO_ROOT, "porter", "mapping.yaml")

os.makedirs(AST_DIR, exist_ok=True)

index = Index.create()


MATRIX_RE = re.compile(r"\bMatrix<[^>]+>")


def _collect_matrix_instantiations(cursor, out):
    """Recursively collect `Eigen::Matrix<...>` specializations."""
    try:
        tspell = cursor.type.spelling
    except Exception:
        tspell = ""
    if tspell:
        m = MATRIX_RE.search(tspell)
        if m:
            out.add(m.group(0).replace("Eigen::", ""))
    for c in cursor.get_children():
        _collect_matrix_instantiations(c, out)


def _placeholder_name(spec: str) -> str:
    """Convert a Matrix<...> instantiation to a placeholder C family name."""
    m = re.match(r"Matrix<\s*([^,>]+)\s*,\s*([^,>]+)\s*,\s*([^,>]+)", spec)
    if not m:
        return "EC_Matrix"
    dtype, rows, _cols = m.groups()
    base = {
        "float": "f",
        "double": "d",
        "int": "i",
    }.get(dtype.split("::")[-1], "t")
    return f"EC_Matrix{rows}{base}"


def cursor_to_dict(cursor):
    """Recursively convert a clang cursor to a serialisable dictionary."""
    return {
        "kind": str(cursor.kind),
        "spelling": cursor.spelling,
        "children": [cursor_to_dict(c) for c in cursor.get_children()],
    }


def process_header(path: str, tu):
    mapping = {}
    instantiations = set()
    for cursor in tu.cursor.get_children():
        if cursor.kind in (CursorKind.CLASS_TEMPLATE, CursorKind.FUNCTION_TEMPLATE):
            mapping[cursor.spelling] = "TODO"
        _collect_matrix_instantiations(cursor, instantiations)
    for spec in sorted(instantiations):
        mapping.setdefault(spec, _placeholder_name(spec))
    return mapping


def main():
    compile_args = ["-std=c++17", f"-I{EIGEN_DIR}"]
    if os.path.exists(MAPPING_PATH):
        with open(MAPPING_PATH, "r", encoding="utf-8") as f:
            if HAVE_YAML:
                mapping = yaml.safe_load(f) or {}
            else:
                mapping = json.load(f)
            mapping = mapping.get("mappings", mapping)
    else:
        mapping = {}
    for root, _, files in os.walk(EIGEN_DIR):
        for name in files:
            if not name.endswith((".h", ".hpp")):
                continue
            path = os.path.join(root, name)
            rel_path = os.path.relpath(path, EIGEN_DIR)
            print(f"Parsing {rel_path}...", file=sys.stderr)
            try:
                tu = index.parse(path, args=compile_args)
            except Exception as exc:
                print(f"Failed to parse {path}: {exc}", file=sys.stderr)
                continue
            ext = ".yaml" if HAVE_YAML else ".json"
            ast_path = os.path.join(AST_DIR, rel_path.replace(os.sep, "_") + ext)
            with open(ast_path, "w", encoding="utf-8") as f:
                data = cursor_to_dict(tu.cursor)
                if HAVE_YAML:
                    yaml.dump(data, f)
                else:
                    json.dump(data, f, indent=2)
            mapping.update(process_header(path, tu))
            # Persist after each file so the mapping grows incrementally
            with open(MAPPING_PATH, "w", encoding="utf-8") as mf:
                if HAVE_YAML:
                    yaml.dump({"mappings": mapping}, mf)
                else:
                    json.dump({"mappings": mapping}, mf, indent=2)

    with open(MAPPING_PATH, "w", encoding="utf-8") as f:
        if HAVE_YAML:
            yaml.dump({"mappings": mapping}, f)
        else:
            json.dump({"mappings": mapping}, f, indent=2)


if __name__ == "__main__":
    main()
