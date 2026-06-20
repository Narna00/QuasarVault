#include "qv_internal.h"

#include <string.h>

int qv_decode_stream(const uint8_t *data, size_t size, uint8_t *out, size_t out_cap, size_t *out_size) {
    if (data == NULL || out == NULL || out_size == NULL || size < 4) {
        return QV_ERR_FORMAT;
    }
    if (data[0] != 'Q' || data[1] != 'S') {
        return QV_ERR_FORMAT;
    }

    uint16_t command_count = qv_read_u16(data + 2);
    size_t cursor = 4;
    size_t produced = 0;

    for (uint16_t i = 0; i < command_count; ++i) {
        if (cursor >= size) {
            return QV_ERR_BOUNDS;
        }

        uint8_t opcode = data[cursor++];
        if (opcode == 0x01) {
            if (cursor >= size) {
                return QV_ERR_BOUNDS;
            }
            uint8_t len = data[cursor++];
            if (cursor + len > size || produced + len > out_cap) {
                return QV_ERR_BOUNDS;
            }
            memcpy(out + produced, data + cursor, len);
            cursor += len;
            produced += len;
        } else if (opcode == 0x02) {
            if (cursor + 2 > size) {
                return QV_ERR_BOUNDS;
            }
            uint8_t repeat = data[cursor++];
            uint8_t value = data[cursor++];
            if (produced + repeat > out_cap) {
                return QV_ERR_BOUNDS;
            }
            memset(out + produced, value, repeat);
            produced += repeat;
        } else if (opcode == 0x03) {
            if (cursor + 2 > size) {
                return QV_ERR_BOUNDS;
            }
            uint8_t distance = data[cursor++];
            uint8_t len = data[cursor++];
            if (distance == 0 || distance > produced) {
                return QV_ERR_BOUNDS;
            }
            for (uint8_t j = 0; j < len; ++j) {
                out[produced + j] = out[produced - distance + j];
            }
            produced += len;
        } else if (opcode == 0xff) {
            break;
        } else {
            return QV_ERR_FORMAT;
        }
    }

    *out_size = produced;
    return QV_OK;
}
