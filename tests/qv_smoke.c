#include "include/quasarvault.h"

#include <stdint.h>
#include <stdio.h>

int main(void) {
    const uint8_t stream[] = {
        'Q', 'S', 2, 0,
        0x01, 3, 'a', 'b', 'c',
        0x02, 2, 'z'
    };
    uint8_t out[16];
    size_t out_size = 0;

    int status = qv_decode_stream(stream, sizeof(stream), out, sizeof(out), &out_size);
    if (status != QV_OK || out_size != 5) {
        fprintf(stderr, "stream smoke failed: %d %zu\n", status, out_size);
        return 1;
    }

    return 0;
}
