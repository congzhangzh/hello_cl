#!/bin/bash
set -e
#do once if needed
which piglit &> /dev/null || ( sudo apt update && sudo apt install build-essential git llvm spirv-tools libllvmspirvlib-dev piglint -y ) || exit 1
[[ -d ~/mesa ]] || (cd ~; git clone https://github.com/mesa3d/mesa.git ) || exit 1
[[ -d ~/opt ]] || (cd ~; mkdir opt) || exit 1

export MESA_INSTALLDIR=$HOME/opt
export OCL_ICD_VENDORS=$MESA_INSTALLDIR/etc/OpenCL/vendors
export LD_LIBRARY_PATH=$MESA_INSTALLDIR/lib/x86_64-linux-gnu 
export LP_CL=1

export MESA_DEBUG=1
export ST_DEBUG=tgsi
export LP_DEBUG=1
export NIR_PRINT=1

#export LP_NUM_THREADS=1
#export CLOVER_DEVICE_VERSION_OVERRIDE=2.2 
#export CLOVER_DEVICE_CLC_VERSION_OVERRIDE=2.2

cd $HOME/mesa || exit 1
[[ -d build ]] || mkdir build || exit 1

echo --begin-- mesa config
meson build/ --buildtype debug -Dprefix="$MESA_INSTALLDIR" -Ddri-drivers=    -Dgallium-drivers=swrast -Dvulkan-drivers=swrast -Dopengl=false -Degl=disabled -Dgbm=disabled -Dgallium-opencl=icd -Dopencl-spirv=true -Dglx=disabled || \
    meson build/ --reconfigure --buildtype debug -Dprefix="$MESA_INSTALLDIR" -Ddri-drivers=    -Dgallium-drivers=swrast -Dvulkan-drivers=swrast -Dopengl=false -Degl=disabled -Dgbm=disabled -Dgallium-opencl=icd -Dopencl-spirv=true -Dglx=disabled || \
    exit 1
echo --end-- mesa config

echo --begin-- mesa build and install
ninja -C build/ install || exit 1
echo --end-- mesa build and install

clinfo

#ref
#0. https://gitlab.freedesktop.org/mesa/mesa/-/issues/5461
#1. https://gitlab.freedesktop.org/mesa/mesa/-/blob/main/src/gallium/drivers/llvmpipe/lp_screen.c#L1038
#2. https://gitlab.freedesktop.org/mesa/mesa/-/blob/main/src/gallium/frontends/clover/core/device.cpp#L170
