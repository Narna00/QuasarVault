#include "qv_internal.h"

#include <stdlib.h>
#include <string.h>

static int manifest_grow(QVManifest *manifest) {
    size_t next_capacity = manifest->capacity == 0 ? 8 : manifest->capacity * 2;
    QVManifestEntry *next = (QVManifestEntry *)realloc(manifest->entries, next_capacity * sizeof(QVManifestEntry));
    if (next == NULL) {
        return QV_ERR_NOMEM;
    }
    manifest->entries = next;
    manifest->capacity = next_capacity;
    return QV_OK;
}

void qv_manifest_free(QVManifest *manifest) {
    if (manifest == NULL) {
        return;
    }
    free(manifest->entries);
    manifest->entries = NULL;
    manifest->count = 0;
    manifest->capacity = 0;
}

int qv_parse_manifest(QVManifest *manifest, const uint8_t *data, size_t size) {
    if (manifest == NULL || data == NULL || size < 4) {
        return QV_ERR_FORMAT;
    }
    if (data[0] != 'M' || data[1] != 'F') {
        return QV_ERR_FORMAT;
    }

    uint16_t count = qv_read_u16(data + 2);
    if (count > 512) {
        return QV_ERR_FORMAT;
    }

    size_t cursor = 4;
    for (uint16_t i = 0; i < count; ++i) {
        if (cursor + 19 > size) {
            return QV_ERR_BOUNDS;
        }

        if (manifest->count == manifest->capacity && manifest_grow(manifest) != QV_OK) {
            return QV_ERR_NOMEM;
        }

        QVManifestEntry *entry = &manifest->entries[manifest->count++];
        memset(entry, 0, sizeof(*entry));
        entry->id = qv_read_u32(data + cursor);
        entry->kind = data[cursor + 4];
        entry->flags = data[cursor + 5];
        entry->offset = qv_read_u32(data + cursor + 6);
        entry->packed_size = qv_read_u32(data + cursor + 10);
        entry->unpacked_size = qv_read_u32(data + cursor + 14);
        uint8_t name_len = data[cursor + 18];
        cursor += 19;

        if (cursor + name_len > size) {
            return QV_ERR_BOUNDS;
        }

        memcpy(entry->name, data + cursor, name_len);
        entry->name[name_len < sizeof(entry->name) ? name_len : sizeof(entry->name) - 1] = '\0';
        cursor += name_len;
    }

    return QV_OK;
}
