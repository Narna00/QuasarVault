#ifndef QUASARVAULT_H
#define QUASARVAULT_H

#include <stdint.h>
#include <stddef.h>

#define QV_OK               0
#define QV_ERR_FORMAT      -1
#define QV_ERR_BOUNDS      -2
#define QV_ERR_NOMEM       -3

typedef struct {
    uint32_t id;
    uint8_t  flags;
    uint32_t logical_size;
    uint8_t *data;
    uint32_t stored_size;   // <-- ADDED: actual allocated size of data
} QVObject;

typedef struct {
    QVObject *objects;
    size_t count;
    size_t capacity;
} QVObjectTable;

typedef struct {
    uint8_t name[64];
    uint32_t id;
    uint8_t  kind;
    uint8_t  flags;
    uint32_t offset;
    uint32_t packed_size;
    uint32_t unpacked_size;
} QVManifestEntry;

typedef struct {
    QVManifestEntry *entries;
    size_t count;
    size_t capacity;
} QVManifest;

typedef struct {
    QVManifest manifest;
    QVObjectTable table;
    uint32_t checksum;
    uint32_t decoded_bytes;
} QVContext;

void qv_context_init(QVContext *ctx);
void qv_context_free(QVContext *ctx);
int  qv_parse_archive(QVContext *ctx, const uint8_t *data, size_t size);

#endif
