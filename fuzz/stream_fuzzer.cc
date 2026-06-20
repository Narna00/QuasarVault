#include <stddef.h>
#include <stdint.h>

extern "C" {
#include "include/quasarvault.h"
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    uint8_t out[256];
    size_t out_size = 0;
    (void)qv_decode_stream(data, size, out, sizeof(out), &out_size);
    return 0;
}
