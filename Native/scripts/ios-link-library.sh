#!/bin/zsh

# Options
VCPKG_DIR=$1
OUTPUT_DIR=$(pwd)
SCRIPT_DIR=$(dirname $0)
PACKAGES_DIR=$VCPKG_DIR/packages

TRIPLET=arm64-ios-bitcode

# Begin
echo "[iOS-build] ===== Uniasset Unity iOS Post-build Script ====="
echo "[iOS-build] VCPKG_DIR: $VCPKG_DIR"
echo "[iOS-build] OUTPUT_DIR: $OUTPUT_DIR"
echo "[iOS-build] SCRIPT_DIR: $SCRIPT_DIR"
echo "[iOS-build] PACKAGES_DIR: $PACKAGES_DIR"
echo "[iOS-build] TRIPLET: $TRIPLET"

echo "[iOS-build] List Of $OUTPUT_DIR"
ls -al $OUTPUT_DIR

function ExtractVcpkgArchive {
    ar x $PACKAGES_DIR/$1_$TRIPLET/lib/$2
}

function ExtractArchive {
    echo "[iOS-build] Extract library: $1"
    ar x $1
}

mkdir _ARCHIVE
pushd _ARCHIVE

echo "[iOS-build] Extract libraries"

# Extract all archives
ExtractArchive $OUTPUT_DIR/libuniasset.a
ExtractVcpkgArchive libjpeg-turbo libturbojpeg.a
ExtractVcpkgArchive libwebp libwebp.a
ExtractVcpkgArchive libwebp libwebpdecoder.a
ExtractVcpkgArchive libwebp libwebpdemux.a

# Link all objects
echo "[iOS-build] Link objects"
ld -r *.o -o $OUTPUT_DIR/libuniasset.o -bitcode_bundle -exported_symbols_list $SCRIPT_DIR/export-symbols.txt
rm $OUTPUT_DIR/libuniasset.a
ar -q $OUTPUT_DIR/libuniasset.a $OUTPUT_DIR/libuniasset.o
ranlib $OUTPUT_DIR/libuniasset.a

popd
rm -rf _ARCHIVE
