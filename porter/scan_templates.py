#!/usr/bin/env python3
"""Scan Eigen headers and build mapping of template instantiations.

This script walks the Eigen/ directory, parses each header with clang's
Python bindings, dumps the AST to porter/asts/<file>.ast, and records
any ClassTemplateDecl or FunctionTemplateDecl names.

It outputs a YAML mapping file mapping template names to placeholder C
families which can be filled in later.
"""

import os
import sys
import yaml
from clang.cindex import Index, Config, CursorKind

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

def process_header(path: str, tu):
    mapping = {}
    for cursor in tu.cursor.get_children():
        if cursor.kind in (CursorKind.CLASS_TEMPLATE, CursorKind.FUNCTION_TEMPLATE):
            mapping[cursor.spelling] = "TODO"
    return mapping

def main():
    compile_args = ["-std=c++17", f"-I{EIGEN_DIR}"]
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
            ast_path = os.path.join(AST_DIR, rel_path.replace(os.sep, "_") + ".ast")
            with open(ast_path, "w", encoding="utf-8") as f:
                f.write(tu.cursor.get_children().__repr__())
            mapping.update(process_header(path, tu))

    with open(MAPPING_PATH, "w", encoding="utf-8") as f:
        yaml.dump({"mappings": mapping}, f)

if __name__ == "__main__":
    main()
