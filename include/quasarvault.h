#ifndef QUASARVAULT_H
#define QUASARVAULT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    QV_OK = 0,
    QV_ERR_FORMAT = -1,
    QV_ERR_BOUNDS = -2,
    QV_ERR_NOMEM = -3,
    QV_ERR_CHECKSUM = -4
} QVStatus;

typedef struct {
    uint32_t id;
    uint8_t kind;
    uint8_t flags;
    uint32_t offset;
    uint32_t packed_size;
    uint32_t unpacked_size;
    char name[32];
} QVManifestEntry;

typedef struct {
    QVManifestEntry *entries;
    size_t count;
    size_t capacity;
} QVManifest;

typedef struct {
    uint32_t id;
    uint32_t logical_size;
    uint8_t flags;
    uint8_t *data;
} QVObject;

typedef struct {
    QVObject *objects;
    size_t count;
    size_t capacity;
} QVObjectTable;

typedef struct {
    QVManifest manifest;
    QVObjectTable table;
    uint32_t checksum;
    uint32_t decoded_bytes;
} QVContext;

void qv_context_init(QVContext *ctx);
void qv_context_free(QVContext *ctx);

int qv_parse_manifest(QVManifest *manifest, const uint8_t *data, size_t size);
void qv_manifest_free(QVManifest *manifest);

int qv_decode_stream(const uint8_t *data, size_t size, uint8_t *out, size_t out_cap, size_t *out_size);
int qv_parse_archive(QVContext *ctx, const uint8_t *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif
