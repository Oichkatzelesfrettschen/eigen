#!/usr/bin/env bash
set -euo pipefail

apt-get update -qq

# Core build tools and compilers
apt-get install -y --no-install-recommends \
    build-essential=12.* gcc-14=14.* g++-14=14.* \
    clang-17=1:17.* clang-tools-17=1:17.* libclang-17-dev=1:17.* llvm-17-dev=1:17.* \
    cmake=3.27.* ninja-build=1.11.* make=4.3.* gmake=4.3.* bmake=2020.* \
    git ca-certificates \
    nasm=2.15.* yasm=1.3.* \
    golang=1.20.* \
    qemu-user-static=1:8.* qemu-system-x86=1:8.* \
    nodejs=18.* npm=9.* \
    rustc=1.70.* cargo=1.70.* \
    python3=3.11.* python3-pip=23.* \
    curl=7.*

# Python packages
python3 -m pip install --no-cache-dir \
    numpy==1.26.* \
    pytest==7.* \
    pycparser==2.* \
    clang==17.* \
    pyyaml==6.*

# Optional local OpenBLAS debs if provided
if ls ./vendor/libopenblas*.deb >/dev/null 2>&1; then
    dpkg -i ./vendor/libopenblas*.deb || true
fi

# Install protoc using curl
curl -sSL https://protobuf.dev/install.sh | bash

exit 0
