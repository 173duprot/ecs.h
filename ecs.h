#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_ENTS 1024
#define MAX_COMPS 32
#define MAX_COMP_SIZE 64

typedef uint32_t Ent;
typedef uint32_t CompType;

struct Comp {
    uint8_t data[MAX_COMPS][MAX_ENTS][MAX_COMP_SIZE];
};

struct ECS {
    Ent ents[MAX_ENTS];
    CompType types[MAX_ENTS][MAX_COMPS];
    struct Comp comps;
    size_t ent_count;
    Ent free_list[MAX_ENTS];
    size_t free_count;
};

static inline void ecs_init(struct ECS* ecs) {
    ecs->ent_count = 0;
    ecs->free_count = 0;
    memset(ecs->ents, 0, sizeof(ecs->ents));
    memset(ecs->types, 0, sizeof(ecs->types));
    memset(ecs->comps.data, 0, sizeof(ecs->comps.data));
}

static inline Ent ecs_create(struct ECS* ecs) {
    Ent ent;
    if (ecs->free_count > 0) {
        ent = ecs->free_list[--ecs->free_count];
    } else {
        if (ecs->ent_count >= MAX_ENTS) return (Ent)-1; // No more entities
        ent = ecs->ent_count++;
    }
    ecs->ents[ent] = ent;
    return ent;
}

static inline void ecs_destroy(struct ECS* ecs, Ent ent) {
    size_t i;
    for (i = 0; i < MAX_COMPS; ++i) {
        ecs->types[ent][i] = 0;
    }
    ecs->free_list[ecs->free_count++] = ent;
}

static inline void ecs_add(struct ECS* ecs, Ent ent, CompType type, void* data, size_t size) {
    size_t i;
    if (size > MAX_COMP_SIZE) return; // Component size too large
    for (i = 0; i < MAX_COMPS; ++i) {
        if (ecs->types[ent][i] == 0) {
            ecs->types[ent][i] = type;
            memcpy(ecs->comps.data[type][ent], data, size);
            break;
        }
    }
}

static inline void ecs_remove(struct ECS* ecs, Ent ent, CompType type) {
    size_t i;
    for (i = 0; i < MAX_COMPS; ++i) {
        if (ecs->types[ent][i] == type) {
            ecs->types[ent][i] = 0;
            break;
        }
    }
}

static inline void* ecs_get(struct ECS* ecs, Ent ent, CompType type) {
    size_t i;
    for (i = 0; i < MAX_COMPS; ++i) {
        if (ecs->types[ent][i] == type) {
            return ecs->comps.data[type][ent];
        }
    }
    return NULL;
}

