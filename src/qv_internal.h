#ifndef QV_INTERNAL_H
#define QV_INTERNAL_H

#include "include/quasarvault.h"

uint16_t qv_read_u16(const uint8_t *p);
uint32_t qv_read_u32(const uint8_t *p);
uint32_t qv_checksum_update(uint32_t state, const uint8_t *data, size_t size);

int qv_table_put(QVObjectTable *table, uint32_t id, uint8_t flags, const uint8_t *data, uint32_t stored_size, uint32_t logical_size);
QVObject *qv_table_get(QVObjectTable *table, uint32_t id);
void qv_table_free(QVObjectTable *table);

#endif
