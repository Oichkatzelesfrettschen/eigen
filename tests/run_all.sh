#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.."; pwd)"

# --- build the C23 port ---
gcc -std=c2x \
    -I"${ROOT_DIR}/eigenc/include" \
    "${ROOT_DIR}/tests/core/test_core.c" -lm \
    -o /tmp/ec_core

# --- build the original C++ reference (for diff tests) ---
g++ -std=c++20 \
    -I"${ROOT_DIR}" \
    "${ROOT_DIR}/tests/core/test_core.cpp" \
    -o /tmp/eigen_core

# --- run functional tests ---
/tmp/ec_core > /tmp/ec_core_out.txt
/tmp/eigen_core > /tmp/eigen_core_out.txt

# --- python numerical parity checks ---
python3 "${ROOT_DIR}/tests/compare_core.py"

echo "\xe2\x9c\x93  all EigenC vs Eigen checks passed"
