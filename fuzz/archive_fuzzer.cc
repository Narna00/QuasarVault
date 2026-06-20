#include <stddef.h>
#include <stdint.h>

extern "C" {
#include "include/quasarvault.h"
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    QVContext ctx;
    qv_context_init(&ctx);
    (void)qv_parse_archive(&ctx, data, size);
    qv_context_free(&ctx);
    return 0;
}
