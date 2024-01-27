#!/bin/zsh

# set env
OUTPUT_DIR=$(pwd)
SCRIPT_DIR=$(dirname $0)
BINARY_DIR=$1

TRIPLET=arm64-ios-bitcode

# print info
echo "[iOS-build] Begin of environment variables"
env
echo "[iOS-build] End"

echo "[iOS-build] Begin of files in $OUTPUT_DIR"
ls -al $OUTPUT_DIR
echo "[iOS-build] End"

# merge archives
echo "[iOS-build] Merge archives"

mv $OUTPUT_DIR/libuniasset.a $OUTPUT_DIR/libuniasset_raw.a
libtool -static -o $OUTPUT_DIR/libuniasset_merge.a $OUTPUT_DIR/libuniasset_raw.a \
  $BINARY_DIR/vcpkg_installed/$TRIPLET/lib/libturbojpeg.a \
  $BINARY_DIR/vcpkg_installed/$TRIPLET/lib/libwebpdecoder.a

# link objects
echo "[iOS-build] Link objects"
mkdir _ARCHIVE
pushd _ARCHIVE || exit

ar x $OUTPUT_DIR/libuniasset_merge.a
ld -r *.o -o $OUTPUT_DIR/libuniasset.o -bitcode_bundle -exported_symbols_list $SCRIPT_DIR/export-symbols.txt
ar -q $OUTPUT_DIR/libuniasset.a $OUTPUT_DIR/libuniasset.o
ranlib $OUTPUT_DIR/libuniasset.a

rm $OUTPUT_DIR/libuniasset_raw.a $OUTPUT_DIR/libuniasset_merge.a
rm $OUTPUT_DIR/libuniasset.o

popd || exit
rm -rf _ARCHIVE
