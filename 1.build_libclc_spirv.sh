#!/bin/bash
set -ex

LLVM_SRC_ROOT=$HOME/llvm
LLVM_BUILD_ROOT=$HOME/llvm/build
INSTALL_PREFIX=$HOME/opt

export LLVM_CONFIG="llvm-config"

$LLVM_CONFIG --version

[[ -d $LLVM_SRC_ROOT ]] || git clone \
    https://github.com/llvm/llvm-project \
    --depth 1 \
    -b release/13.x \
    $LLVM_SRC_ROOT || exit 1

[[ -d $LLVM_BUILD_ROOT ]] || mkdir $LLVM_BUILD_ROOT || exit 1

[[ -d $LLVM_BUILD_ROOT/libclc ]] || mkdir $LLVM_BUILD_ROOT/libclc || exit 1

cmake -S $LLVM_SRC_ROOT/libclc -B $LLVM_BUILD_ROOT/libclc -G Ninja -DLLVM_CONFIG=$LLVM_CONFIG -DLIBCLC_TARGETS_TO_BUILD="spirv-mesa3d-;spirv64-mesa3d-" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX/usr -DLLVM_SPIRV=/usr/bin/llvm-spirv || exit 1

ninja -C $LLVM_BUILD_ROOT/libclc install || exit 1


# workaroud cmake vs debian packaging.
#mkdir -p /usr/lib/clc
sudo ln -s $INSTALL_PREFIX/usr/share/clc/spirv64-mesa3d-.spv /usr/lib/clc/ || exit 1
sudo ln -s $INSTALL_PREFIX/usr/share/clc/spirv-mesa3d-.spv /usr/lib/clc/ || exit 1
