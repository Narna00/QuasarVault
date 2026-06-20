# QuasarVault

QuasarVault is a compact C library for parsing a binary object archive. The
format stores a manifest, raw objects, filtered objects, and links between
objects in a single container. The code is intended to be small enough to embed
in command-line tools while still keeping the parsing and decoding paths
separate.

## Layout

```text
include/
  quasarvault.h
src/
  qv_archive.c
  qv_manifest.c
  qv_stream.c
  qv_table.c
  qv_util.c
fuzz/
  archive_fuzzer.cc
  manifest_fuzzer.cc
  stream_fuzzer.cc
  corpus/
  manifest_corpus/
  stream_corpus/
  dictionary.txt
tests/
  qv_smoke.c
.clusterfuzzlite/
  build.sh
  project.yaml
Makefile
```

## Archive Format

Archive files begin with `QVLT`, followed by a little-endian version, record
count, and flags field. Each record contains a one-byte type, a four-byte
little-endian body length, and the record body.

Supported record types:

- `0x11`: manifest
- `0x21`: raw object
- `0x22`: filtered object
- `0x31`: link object

The stream decoder accepts command streams beginning with `QS`. Commands can
append literals, emit repeated bytes, or copy from previously decoded output.

## Build

```bash
make test
```

ClusterFuzzLite builds the fuzz targets from `.clusterfuzzlite/build.sh` and
writes `archive_fuzzer`, `stream_fuzzer`, and `manifest_fuzzer` to `$OUT`.
