#include <stddef.h>
#include <stdint.h>

extern "C" {
#include "include/quasarvault.h"
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    QVManifest manifest = {0};
    (void)qv_parse_manifest(&manifest, data, size);
    qv_manifest_free(&manifest);
    return 0;
}
