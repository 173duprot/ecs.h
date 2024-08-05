#ifndef ECS_H
#define ECS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdalign.h>

#define MAX_ENTS 1024
#define MAX_CMPS 32
#define MAX_CMP_SIZE 64
#define CACHE_LINE_SIZE 64
#define PAGE_SIZE 4096

typedef uint32_t ent_t;
typedef uint32_t cmp_t;

typedef void (*ecs_callback_t)(ent_t, void*, void*);

struct cmp_data {
    alignas(CACHE_LINE_SIZE) uint8_t data[MAX_ENTS][MAX_CMP_SIZE];
};

struct ECS {
    alignas(PAGE_SIZE) size_t ent_count;
    alignas(PAGE_SIZE) ent_t ents[MAX_ENTS];
    
    struct cmp_data cmps[MAX_CMPS];
    alignas(CACHE_LINE_SIZE) cmp_t types[MAX_CMPS][MAX_ENTS];
    
    alignas(PAGE_SIZE) size_t free_count;
    alignas(PAGE_SIZE) ent_t free_list[MAX_ENTS];
};

#ifndef prefetch
static inline void prefetch(const void* ptr) {
    __builtin_prefetch(ptr, 0, 3);
}
#endif

static inline ent_t create(struct ECS* ecs) {
    if (!ecs || ecs->ent_count >= MAX_ENTS) return (ent_t)-1;

    if (ecs->free_count > 0) {
        prefetch(&ecs->free_list[ecs->free_count - 1]);
        return ecs->free_list[--ecs->free_count];
    }

    ecs->ents[ecs->ent_count] = ecs->ent_count;
    return ecs->ent_count++;
}

static inline void destroy(struct ECS* ecs, ent_t ent) {
    if (!ecs || ent >= ecs->ent_count) return;

    for (size_t i = 0; i < MAX_CMPS; ++i) {
        ecs->types[i][ent] = 0;
    }
    prefetch(&ecs->free_list[ecs->free_count]);
    ecs->free_list[ecs->free_count++] = ent;
}

static inline int add(struct ECS* ecs, ent_t ent, cmp_t type, void* data, size_t size) {
    if (!ecs || !data || ent >= ecs->ent_count || type >= MAX_CMPS || size > MAX_CMP_SIZE) return -1;

    prefetch(ecs->cmps[type].data[ent]);
    if (ecs->types[type][ent] == 0) {
        ecs->types[type][ent] = type + 1; // Set the type (add 1 to differentiate from zero)
        memcpy(ecs->cmps[type].data[ent], data, size);
        return 0;
    }
    return -1;
}

static inline void del(struct ECS* ecs, ent_t ent, cmp_t type) {
    if (!ecs || ent >= ecs->ent_count || type >= MAX_CMPS) return;

    prefetch(ecs->cmps[type].data[ent]);
    ecs->types[type][ent] = 0;
}

static inline int save(struct ECS* ecs, const char* filename) {
    if (!ecs || !filename) return -1;

    prefetch(ecs);
    FILE* file = fopen(filename, "wb");
    if (!file) return -1;

    fwrite(ecs, sizeof(struct ECS), 1, file);
    fclose(file);
    return 0;
}

static inline int load(struct ECS* ecs, const char* filename) {
    if (!ecs || !filename) return -1;

    prefetch(ecs);
    FILE* file = fopen(filename, "rb");
    if (!file) return -1;

    fread(ecs, sizeof(struct ECS), 1, file);
    fclose(file);
    return 0;
}

static inline void iterate(struct ECS* ecs, ecs_callback_t callback, void* context) {
    if (!ecs || !callback) return;

    for (size_t i = 0; i < ecs->ent_count; ++i) {
        for (size_t j = 0; j < MAX_CMPS; ++j) {
            if (ecs->types[j][i] != 0) {
                prefetch(ecs->cmps[j].data[i]);
                callback(ecs->ents[i], ecs->cmps[j].data[i], context);
                break;
            }
        }
    }
}

#endif // ECS_H
