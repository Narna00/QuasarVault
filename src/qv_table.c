#include "qv_internal.h"

#include <stdlib.h>
#include <string.h>

static QVObject *find_object(QVObjectTable *table, uint32_t id) {
    for (size_t i = 0; i < table->count; ++i) {
        if (table->objects[i].id == id) {
            return &table->objects[i];
        }
    }
    return NULL;
}

static int grow_table(QVObjectTable *table) {
    size_t next_capacity = table->capacity == 0 ? 8 : table->capacity * 2;
    QVObject *next = (QVObject *)realloc(table->objects, next_capacity * sizeof(QVObject));
    if (next == NULL) {
        return QV_ERR_NOMEM;
    }
    table->objects = next;
    table->capacity = next_capacity;
    return QV_OK;
}

int qv_table_put(QVObjectTable *table, uint32_t id, uint8_t flags, const uint8_t *data, uint32_t stored_size, uint32_t logical_size) {
    QVObject *object = find_object(table, id);
    if (object == NULL) {
        if (table->count == table->capacity && grow_table(table) != QV_OK) {
            return QV_ERR_NOMEM;
        }
        object = &table->objects[table->count++];
        memset(object, 0, sizeof(*object));
        object->id = id;
    } else {
        free(object->data);
        object->data = NULL;
    }

    object->data = (uint8_t *)malloc(stored_size == 0 ? 1 : stored_size);
    if (object->data == NULL) {
        object->logical_size = 0;
        return QV_ERR_NOMEM;
    }

    if (stored_size != 0) {
        memcpy(object->data, data, stored_size);
    }
    object->flags = flags;
    object->logical_size = logical_size;
    return QV_OK;
}

QVObject *qv_table_get(QVObjectTable *table, uint32_t id) {
    return find_object(table, id);
}

void qv_table_free(QVObjectTable *table) {
    if (table == NULL) {
        return;
    }
    for (size_t i = 0; i < table->count; ++i) {
        free(table->objects[i].data);
    }
    free(table->objects);
    table->objects = NULL;
    table->count = 0;
    table->capacity = 0;
}
