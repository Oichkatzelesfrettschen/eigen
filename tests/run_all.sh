#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.."; pwd)"

gcc -std=c23 -I"${ROOT_DIR}/eigenc/include" "${ROOT_DIR}/tests/core/test_core.c" -o /tmp/ec_test \
  || gcc -std=c2x -I"${ROOT_DIR}/eigenc/include" "${ROOT_DIR}/tests/core/test_core.c" -o /tmp/ec_test

g++ -std=c++17 -I"${ROOT_DIR}" "${ROOT_DIR}/tests/core/test_core.cpp" -o /tmp/eigen_test

/tmp/ec_test
/tmp/eigen_test

python3 "${ROOT_DIR}/tests/compare_eigen_cpp_vs_c.py"

echo "\xe2\x9c\x93  all EigenC vs Eigen checks passed"
