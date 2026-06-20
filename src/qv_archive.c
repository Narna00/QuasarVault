#include "qv_internal.h"

#include <stdlib.h>
#include <string.h>

enum {
    QV_REC_MANIFEST = 0x11,
    QV_REC_OBJECT = 0x21,
    QV_REC_FILTERED_OBJECT = 0x22,
    QV_REC_LINK = 0x31
};

void qv_context_init(QVContext *ctx) {
    memset(ctx, 0, sizeof(*ctx));
    ctx->checksum = 2166136261u;
}

void qv_context_free(QVContext *ctx) {
    if (ctx == NULL) {
        return;
    }
    qv_manifest_free(&ctx->manifest);
    qv_table_free(&ctx->table);
    ctx->checksum = 0;
    ctx->decoded_bytes = 0;
}

static int parse_object(QVContext *ctx, const uint8_t *body, uint32_t body_size) {
    if (body_size < 9) {
        return QV_ERR_FORMAT;
    }

    uint32_t id = qv_read_u32(body);
    uint8_t flags = body[4];
    uint32_t logical_size = qv_read_u32(body + 5);
    const uint8_t *payload = body + 9;
    uint32_t payload_size = body_size - 9;

    ctx->checksum = qv_checksum_update(ctx->checksum, payload, payload_size);
    return qv_table_put(&ctx->table, id, flags, payload, payload_size, logical_size);
}

static int parse_filtered_object(QVContext *ctx, const uint8_t *body, uint32_t body_size) {
    if (body_size < 9) {
        return QV_ERR_FORMAT;
    }

    uint32_t id = qv_read_u32(body);
    uint8_t flags = body[4];
    uint32_t logical_size = qv_read_u32(body + 5);
    uint8_t decoded[256];
    size_t decoded_size = 0;

    int status = qv_decode_stream(body + 9, body_size - 9, decoded, sizeof(decoded), &decoded_size);
    if (status != QV_OK) {
        return status;
    }

    ctx->decoded_bytes += (uint32_t)decoded_size;
    ctx->checksum = qv_checksum_update(ctx->checksum, decoded, decoded_size);
    return qv_table_put(&ctx->table, id, flags, decoded, (uint32_t)decoded_size, logical_size);
}

static int parse_link(QVContext *ctx, const uint8_t *body, uint32_t body_size) {
    if (body_size < 9) {
        return QV_ERR_FORMAT;
    }

    uint32_t src_id = qv_read_u32(body);
    uint32_t dst_id = qv_read_u32(body + 4);
    uint8_t span = body[8];
    QVObject *src = qv_table_get(&ctx->table, src_id);
    if (src == NULL) {
        return QV_ERR_FORMAT;
    }

    return qv_table_put(&ctx->table, dst_id, src->flags, src->data, span, src->logical_size);
}

int qv_parse_archive(QVContext *ctx, const uint8_t *data, size_t size) {
    if (ctx == NULL || data == NULL || size < 10) {
        return QV_ERR_FORMAT;
    }
    if (memcmp(data, "QVLT", 4) != 0) {
        return QV_ERR_FORMAT;
    }

    uint16_t version = qv_read_u16(data + 4);
    uint16_t record_count = qv_read_u16(data + 6);
    uint16_t flags = qv_read_u16(data + 8);
    if (version != 2 || record_count > 512) {
        return QV_ERR_FORMAT;
    }

    size_t cursor = 10;
    for (uint16_t i = 0; i < record_count; ++i) {
        if (cursor + 5 > size) {
            return QV_ERR_BOUNDS;
        }

        uint8_t type = data[cursor++];
        uint32_t body_size = qv_read_u32(data + cursor);
        cursor += 4;
        if (cursor + body_size > size) {
            return QV_ERR_BOUNDS;
        }

        const uint8_t *body = data + cursor;
        int status = QV_ERR_FORMAT;
        if (type == QV_REC_MANIFEST) {
            status = qv_parse_manifest(&ctx->manifest, body, body_size);
        } else if (type == QV_REC_OBJECT) {
            status = parse_object(ctx, body, body_size);
        } else if (type == QV_REC_FILTERED_OBJECT) {
            status = parse_filtered_object(ctx, body, body_size);
        } else if (type == QV_REC_LINK) {
            status = parse_link(ctx, body, body_size);
        }

        if (status != QV_OK) {
            return status;
        }
        cursor += body_size;
    }

    if ((flags & 1u) != 0 && ctx->manifest.count == 0) {
        return QV_ERR_FORMAT;
    }
    return QV_OK;
}
