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

typedef uint16_t ent_t;
typedef uint8_t cmp_t;

typedef void (*ecs_callback_t)(ent_t, void*, void*);

struct ECS {
    alignas(CACHE_LINE_SIZE) size_t ent_count;
    alignas(CACHE_LINE_SIZE) ent_t ents[MAX_ENTS];

    alignas(CACHE_LINE_SIZE) size_t free_count;
    alignas(CACHE_LINE_SIZE) ent_t free_list[MAX_ENTS];

    struct {
        alignas(CACHE_LINE_SIZE) uint8_t data[MAX_ENTS][MAX_CMP_SIZE];
        alignas(CACHE_LINE_SIZE) cmp_t types[MAX_ENTS];
    } cmps[MAX_CMPS];
};

static inline ent_t create(struct ECS* ecs) {
    if (!ecs || ecs->ent_count >= MAX_ENTS) return (ent_t)-1;

    if (ecs->free_count > 0) {
        return ecs->free_list[--ecs->free_count];
    }

    ecs->ents[ecs->ent_count] = ecs->ent_count;
    return ecs->ent_count++;
}

static inline void destroy(struct ECS* ecs, ent_t ent) {
    if (!ecs || ent >= ecs->ent_count) return;

    for (size_t i = 0; i < MAX_CMPS; ++i) {
        ecs->cmps[i].types[ent] = 0;
    }
    ecs->free_list[ecs->free_count++] = ent;
}

static inline int add(struct ECS* ecs, ent_t ent, cmp_t type, void* data, size_t size) {
    if (!ecs || !data || ent >= ecs->ent_count || type >= MAX_CMPS || type == 0 || size > MAX_CMP_SIZE) return -1;
    // Type cannot be 0, 0 means empty

    if (ecs->cmps[type].types[ent] == 0) {
        ecs->cmps[type].types[ent] = type;
        memcpy(ecs->cmps[type].data[ent], data, size);
        return 0;
    }
    return -1;
}

static inline void del(struct ECS* ecs, ent_t ent, cmp_t type) {
    if (!ecs || ent >= ecs->ent_count || type >= MAX_CMPS) return;

    ecs->cmps[type].types[ent] = 0;
}

static inline int save(struct ECS* ecs, const char* filename) {
    if (!ecs || !filename) return -1;

    FILE* file = fopen(filename, "wb");
    if (!file) return -1;

    fwrite(ecs, sizeof(struct ECS), 1, file);
    fclose(file);
    return 0;
}

static inline int load(struct ECS* ecs, const char* filename) {
    if (!ecs || !filename) return -1;

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
            if (ecs->cmps[j].types[i] != 0) {
                callback(ecs->ents[i], ecs->cmps[j].data[i], context);
                break;
            }
        }
    }
}

#endif // ECS_H
