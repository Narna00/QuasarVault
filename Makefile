CC ?= clang
CFLAGS ?= -Wall -Wextra -Wpedantic -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer

SRC = src/qv_util.c src/qv_table.c src/qv_manifest.c src/qv_stream.c src/qv_archive.c
OBJ = $(SRC:.c=.o)

.PHONY: all test clean

all: tests/qv_smoke

tests/qv_smoke: tests/qv_smoke.c $(OBJ)
	$(CC) $(CFLAGS) -I. $^ -o $@

test: tests/qv_smoke
	./tests/qv_smoke

clean:
	rm -f src/*.o tests/qv_smoke
