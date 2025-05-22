#!/usr/bin/env bash
# Build helper to generate headers and run both C and Go tests.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# Generate C headers from mapping
python3 "$ROOT/porter/gen_headers.py"

# Ensure go.mod targets Go 1.23
if command -v go >/dev/null; then
  (cd "$ROOT/go" && go mod edit -go=1.23)
  (cd "$ROOT/go" && go vet ./... && go test ./...)
else
  echo "Go toolchain not found" >&2
fi

# Run C/C++ tests
bash "$ROOT/tests/run_all.sh"
