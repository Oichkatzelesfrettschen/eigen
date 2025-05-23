#!/usr/bin/env bash
set -euo pipefail
clang-tidy --config-file=.clang-tidy-c23 "$@"
