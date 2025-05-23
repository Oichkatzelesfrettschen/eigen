# EigenC: Bridging Eigen to C23

This fork tracks an effort to translate the Eigen C++ library into a set of
headers that can be consumed from pure C23.  The goal is to retain the familiar
API while generating self contained C code that does not depend on C++
features.

The translation process lives in `porter/` and outputs headers under
`eigenc/include`.  These C headers implement basic matrix operations with
`static inline` functions and `_Generic` dispatch.

## Generating Headers

Run `porter/gen_headers.py` from the repository root after editing
`porter/mapping.yaml` with the template instantiations you need:

```bash
python3 porter/gen_headers.py
```

The script produces `eigenc/include/ec_generated.h` which is included from
`ec_core.h`.

## Running Tests

The test suite can be executed via `tests/run_all.sh`.  It builds a small example
both as C and as C++ and compares the numerical results.  Cross-compilation for
PowerPC is exercised if the toolchain and QEMU are available.

```bash
bash tests/run_all.sh
```

## Using `ec_Matrixf32`

Below is a minimal example demonstrating how to use the generated C API:

```c
#include "ec_core.h"

int main(void) {
    float a_data[4] = {1,2,3,4};
    float b_data[4] = {5,6,7,8};
    float c_data[4];
    ec_Matrixf32 A = {2,2,a_data};
    ec_Matrixf32 B = {2,2,b_data};
    ec_Matrixf32 C = {2,2,c_data};
    ec_addf32(&A, &B, &C);
    return 0;
}
```

## Cross Compilation

The repository provides cross-compilation checks for PowerPC using
`powerpc-linux-gnu-gcc` and `qemu-ppc-static`.  The `tests/run_all.sh` script
handles building and executing the tests under emulation when these tools are
present.  If 32-bit development libraries are available, the same script also
attempts a 32-bit x86 build using `gcc -m32`.

## Go Bindings

Bindings for Go are generated in `go/ec`. The module targets **Go 1.23** and
mirrors the C API produced under `eigenc/include`. After running the porter
scripts you can execute the Go tests:

```bash
cd go && go test ./...
```

The convenience script `porter/build_all.sh` will regenerate headers and run the
entire suite including the Go tests.

## Development Environment

Run `./setup.sh` to install the toolchain including GCC, Clang/LLVM, Meson and
pre-commit.  The pre-commit hooks are configured to use local scripts so they
work even when the network is unavailable. After installation you can enable
the git hooks with:

```bash
pre-commit install
pre-commit run --all-files
```

Clang-Tidy configurations for both C23 and C++23 live in `.clang-tidy-c23` and
`.clang-tidy`.  The project builds with either GCC or Clang; the test scripts
use whichever compiler is available.


