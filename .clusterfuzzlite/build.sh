#!/bin/bash -eu

COMMON_OBJS="qv_util.o qv_table.o qv_manifest.o qv_stream.o qv_archive.o"

$CC $CFLAGS -I"$SRC" -c "$SRC/src/qv_util.c" -o qv_util.o
$CC $CFLAGS -I"$SRC" -c "$SRC/src/qv_table.c" -o qv_table.o
$CC $CFLAGS -I"$SRC" -c "$SRC/src/qv_manifest.c" -o qv_manifest.o
$CC $CFLAGS -I"$SRC" -c "$SRC/src/qv_stream.c" -o qv_stream.o
$CC $CFLAGS -I"$SRC" -c "$SRC/src/qv_archive.c" -o qv_archive.o

$CXX $CXXFLAGS -I"$SRC" "$SRC/fuzz/archive_fuzzer.cc" $COMMON_OBJS $LIB_FUZZING_ENGINE -o "$OUT/archive_fuzzer"
$CXX $CXXFLAGS -I"$SRC" "$SRC/fuzz/stream_fuzzer.cc" $COMMON_OBJS $LIB_FUZZING_ENGINE -o "$OUT/stream_fuzzer"
$CXX $CXXFLAGS -I"$SRC" "$SRC/fuzz/manifest_fuzzer.cc" $COMMON_OBJS $LIB_FUZZING_ENGINE -o "$OUT/manifest_fuzzer"

(cd "$SRC/fuzz/corpus" && zip -q -r "$OUT/archive_fuzzer_seed_corpus.zip" .)
(cd "$SRC/fuzz/stream_corpus" && zip -q -r "$OUT/stream_fuzzer_seed_corpus.zip" .)
(cd "$SRC/fuzz/manifest_corpus" && zip -q -r "$OUT/manifest_fuzzer_seed_corpus.zip" .)
