#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.."; pwd)"

# Native build

gcc -std=c23 -I"${ROOT_DIR}/eigenc/include" "${ROOT_DIR}/tests/core/test_core.c" -o /tmp/ec_test \
  || gcc -std=c2x -I"${ROOT_DIR}/eigenc/include" "${ROOT_DIR}/tests/core/test_core.c" -o /tmp/ec_test

g++ -std=c++14 -I"${ROOT_DIR}" "${ROOT_DIR}/tests/core/test_core.cpp" -o /tmp/eigen_test

/tmp/ec_test
/tmp/eigen_test

python3 "${ROOT_DIR}/tests/compare_eigen_cpp_vs_c.py"
cp /tmp/c_out.txt /tmp/c_out_host.txt

echo "\xE2\x9C\x93  native EigenC vs Eigen checks passed"

# Cross-compile for PowerPC and run via qemu if toolchain present
if command -v powerpc-linux-gnu-gcc >/dev/null && command -v qemu-ppc-static >/dev/null; then
  powerpc-linux-gnu-gcc -static -std=c23 -I"${ROOT_DIR}/eigenc/include" "${ROOT_DIR}/tests/core/test_core.c" -o /tmp/ec_test_ppc \
    || powerpc-linux-gnu-gcc -static -std=c2x -I"${ROOT_DIR}/eigenc/include" "${ROOT_DIR}/tests/core/test_core.c" -o /tmp/ec_test_ppc

  qemu-ppc-static /tmp/ec_test_ppc
  mv /tmp/c_out.txt /tmp/c_out_ppc.txt

  diff -u /tmp/c_out_host.txt /tmp/c_out_ppc.txt

  echo "\xE2\x9C\x93  cross-compiled EigenC checks passed (powerpc)"
else
  echo "Skipping PowerPC cross-compile test: toolchain or QEMU not found" >&2
fi

# Cross-compile for i686 and run via qemu if available
if command -v i686-linux-gnu-gcc >/dev/null && command -v qemu-i386-static >/dev/null; then
  i686-linux-gnu-gcc -static -std=c23 -I"${ROOT_DIR}/eigenc/include" "${ROOT_DIR}/tests/core/test_core.c" -o /tmp/ec_test_i686 \
    || i686-linux-gnu-gcc -static -std=c2x -I"${ROOT_DIR}/eigenc/include" "${ROOT_DIR}/tests/core/test_core.c" -o /tmp/ec_test_i686

  qemu-i386-static /tmp/ec_test_i686
  mv /tmp/c_out.txt /tmp/c_out_i686.txt

  diff -u /tmp/c_out_host.txt /tmp/c_out_i686.txt

  echo "\xE2\x9C\x93  cross-compiled EigenC checks passed (i686)"
else
  echo "Skipping i686 cross-compile test: toolchain or QEMU not found" >&2
fi

# Run Go tests if Go is available
if command -v go >/dev/null; then
  (cd "${ROOT_DIR}/go" && go test ./...)
  echo "\xE2\x9C\x93  Go bindings tests passed"
else
  echo "Skipping Go tests: go not found" >&2
fi
