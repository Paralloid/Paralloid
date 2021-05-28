#!/bin/bash

set -e

BUSYBOX_VERSION=1.33.1
BUSYBOX_SRC=busybox-${BUSYBOX_VERSION}
HOST_ARCH=$(uname -m)

NDK_HOME=${ANDROID_NDK}

if [ -z "${NDK_HOME}" ];then
    NDK_HOME=${ANDROID_NDK_HOME}
fi

if [ -z "${NDK_HOME}" ];then
    echo "Please define ANDROID_NDK or ANDROID_NDK_HOME"
    exit 1
fi

# Make sure we always execute in the script directory
pushd "$(dirname "$(realpath "$0")")"

# Remove the old build directory
rm -rf build
mkdir build

pushd build

echo "Downloading ${BUSYBOX_SRC}"
wget https://busybox.net/downloads/${BUSYBOX_SRC}.tar.bz2
tar xf ${BUSYBOX_SRC}.tar.bz2

pushd ${BUSYBOX_SRC}

echo "Patching busybox source code"
for p in $(find "../../android-busybox-ndk/patches/" -name "*.patch" | sort);do
    # We don't want the EXTRAVERSION from osm0sis
    if [ "$(basename ${p})" == "000-customMakefile.patch" ];then
        continue
    fi
    # These patches are no longer needed as of Android NDK r22
    # Having them will cause conflicts like struct redefinition
    if [ "$(basename ${p})" == "015-zcip.patch" ];then
        continue
    fi
    
    patch -p1 -N < "$p"
done

echo "Building busybox"
# Set extra version to paralloid
sed -i "s/EXTRAVERSION =/EXTRAVERSION = -paralloid/" Makefile
# Copy config from osm0sis
cp "../../android-busybox-ndk/osm0sis-full-unified.config" .config
# We need `mdev` to be present
sed -i "s/# CONFIG_MDEV is not set/CONFIG_MDEV=y/" .config
sed -i "s/# CONFIG_FEATURE_MDEV_DAEMON is not set/CONFIG_FEATURE_MDEV_DAEMON=y/" .config
# Also fallocate
sed -i "s/# CONFIG_FALLOCATE is not set/CONFIG_FALLOCATE=y/" .config
# These syscalls are now present in r22, so no need to redefine them here
sed -i "s/pid_t getsid/pid_t _getsid/" libbb/missing_syscalls.c
sed -i "s/int sethostname/int _sethostname/" libbb/missing_syscalls.c
sed -i "s/int adjtimex/int _adjtimex/" libbb/missing_syscalls.c
# Add NDK to PATH
export PATH="${NDK_HOME}/toolchains/llvm/prebuilt/linux-${HOST_ARCH}/bin:${PATH}"
# CROSS_COMPILE is for tools other than the main clang executable (e.g. ld, ar)
export CROSS_COMPILE="${NDK_HOME}/toolchains/llvm/prebuilt/linux-${HOST_ARCH}/bin/arm-linux-androideabi-"
# clang uses a different prefix
export CC="${NDK_HOME}/toolchains/llvm/prebuilt/linux-${HOST_ARCH}/bin/armv7a-linux-androideabi21-clang"
# Redefine CFLAGS to override what's in osm0sis's config
# because it contains some options not supported by clang, which spams the log
# `-O0` disables all optimizations, as clang breaks some applet's main functions
# annotated with MAIN_EXTERNALLY_VISIBLE when optimizations are enabled
# (ref: https://github.com/termux/termux-packages/blob/master/packages/busybox/0001-clang-fix.patch)
# in addition, add the `-static` flag to force static linking
export CFLAGS="-DANDROID -D__ANDROID__ -D__ANDROID_API__=21 -DSK_RELEASE -O0 -static"
# There seems to be race conditions in building the networking part, causing
# random build errors. Unfortunately, this means we have to build with -j1 right now.
make ARCH=arm CROSS_COMPILE="${CROSS_COMPILE}" CC="${CC}" CFLAGS="${CFLAGS}" -j1

# -- pushd ${BUSYBOX_SRC}
popd

cp ${BUSYBOX_SRC}/busybox ./
echo "Built busybox binary: ${PWD}/busybox"

# -- pushd build
popd
# -- pushd "$(dirname "$(realpath "$0")")"
popd
