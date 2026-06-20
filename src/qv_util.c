#include "qv_internal.h"

uint16_t qv_read_u16(const uint8_t *p) {
    return (uint16_t)(((uint16_t)p[0]) | ((uint16_t)p[1] << 8));
}

uint32_t qv_read_u32(const uint8_t *p) {
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

uint32_t qv_checksum_update(uint32_t state, const uint8_t *data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        state ^= data[i];
        state *= 16777619u;
    }
    return state;
}
