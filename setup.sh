#!/usr/bin/env bash
set -euo pipefail

apt-get update -qq

# Install build essentials and compilers
apt-get install -y \
    build-essential=12.* \
    g++-12=12.* \
    clang-17=1:17.* \
    clang-tools-17=1:17.* \
    libclang-17-dev=1:17.* \
    llvm-17-dev=1:17.* \
    gcc-multilib=12.* \
    cmake=3.27.* \
    make=4.3.* \
    gmake=4.3.* \
    bmake=2020.* \
    ninja-build=1.11.* \
    nasm=2.15.* \
    yasm=1.3.*

# Go toolchain
apt-get install -y golang=1.20.*

# QEMU for emulation
apt-get install -y \
    qemu-user-static=1:8.* \
    qemu-system-x86=1:8.*

# Node.js and npm
apt-get install -y nodejs=18.* npm=9.*

# Rust toolchain
apt-get install -y rustc=1.70.* cargo=1.70.*

# Python and pip
apt-get install -y python3=3.11.* python3-pip=23.*

pip3 install --no-cache-dir \
    numpy==1.26.* \
    pytest==7.* \
    clang==17.*

# Optional local OpenBLAS archive
dpkg -i ./vendor/libopenblas*.deb || true

# curl for protoc installer
apt-get install -y curl=7.*

# Install protoc via official script
curl -sSL https://protobuf.dev/install.sh | bash

exit 0
