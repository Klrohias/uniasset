#!/bin/zsh

# Options
OUTPUT_DIR=$(pwd)
SCRIPT_DIR=$(dirname $0)
BINARY_DIR=$1

TRIPLET=arm64-ios-bitcode

# Begin
echo "[iOS-build] ===== Uniasset Unity iOS Post-build Script ====="
echo "[iOS-build] OUTPUT_DIR: $OUTPUT_DIR"
echo "[iOS-build] SCRIPT_DIR: $SCRIPT_DIR"
echo "[iOS-build] BINARY_DIR: $BINARY_DIR"
echo "[iOS-build] TRIPLET: $TRIPLET"


echo "[iOS-build] List Of $OUTPUT_DIR"
ls -al $OUTPUT_DIR

function ExtractVcpkgArchive {
    ar x $BINARY_DIR/vcpkg_installed/$TRIPLET/lib/$1
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
ExtractVcpkgArchive libturbojpeg.a
ExtractVcpkgArchive libwebp.a
ExtractVcpkgArchive libwebpdecoder.a
ExtractVcpkgArchive libwebpdemux.a

# Link all objects
echo "[iOS-build] Link objects"
ld -r *.o -o $OUTPUT_DIR/libuniasset.o -bitcode_bundle -exported_symbols_list $SCRIPT_DIR/export-symbols.txt
rm $OUTPUT_DIR/libuniasset.a
ar -q $OUTPUT_DIR/libuniasset.a $OUTPUT_DIR/libuniasset.o
ranlib $OUTPUT_DIR/libuniasset.a

popd
rm -rf _ARCHIVE
