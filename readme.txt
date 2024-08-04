ECS(3)                 Library Functions Manual                ECS(3)

NAME
    ecs.h - Entity Component System (ECS) library

DESCRIPTION
    ecs.h provides a simple. ultra-high performance
    Entity Component System (ECS) framework in only
    88 lines of code.

    It's tiny, cache-friendly, static, opinionated
    design targets single-thread high-performance,
    and secure applications.

    It allows the following:

    State saving and loading,
    Entity creation and destruction,
    Component addition and deletion,
    and Itteration of state through
    entity-embedded callback functions.

SYNOPSIS
    #include "ecs.h"

    typedef uint32_t ent_t;
    typedef uint32_t cmp_t;
    typedef void (*ecs_callback_t)(ent_t, void*, 
                                   void*);

    struct ECS {
        size_t ent_count;
        ent_t ents[MAX_ENTS];
        struct cmp_data cmps[MAX_CMPS];
        cmp_t types[MAX_CMPS][MAX_ENTS];
        size_t free_count;
        ent_t free_list[MAX_ENTS];
    };

    ent_t create(struct ECS *ecs);
    void destroy(struct ECS *ecs, ent_t ent);
    int add(struct ECS *ecs, ent_t ent, cmp_t type, 
            void *data, size_t size);
    void del(struct ECS *ecs, ent_t ent, cmp_t type);
    int save(struct ECS *ecs, const char *filename);
    int load(struct ECS *ecs, const char *filename);
    void iterate(struct ECS *ecs, ecs_callback_t 
                 callback, void *context);

FUNCTIONS
    ent_t create(struct ECS *ecs)
        Creates a new entity. Returns the entity 
        ID or -1 on failure.

    void destroy(struct ECS *ecs, ent_t ent)
        Destroys the specified entity, freeing its 
        slot for reuse.

    int add(struct ECS *ecs, ent_t ent, cmp_t type, 
            void *data, size_t size)
        Adds a component of the given type to the 
        specified entity, copying the provided data. 
        Returns 0 on success or -1 on failure.

    void del(struct ECS *ecs, ent_t ent, cmp_t type)
        Removes the component of the given type 
        from the specified entity.

    int save(struct ECS *ecs, const char *filename)
        Saves the current ECS state to the specified 
        file. Returns 0 on success or -1 on failure.

    int load(struct ECS *ecs, const char *filename)
        Loads the ECS state from the specified file. 
        Returns 0 on success or -1 on failure.

    void iterate(struct ECS *ecs, ecs_callback_t 
                 callback, void *context)
        Iterates over all entities, calling the 
        specified callback for each entity that 
        has any component.

CONSTANTS
    MAX_ENTS
        Maximum number of entities supported (1024).

    MAX_CMPS
        Maximum number of component types supported 
        (32).

    MAX_CMP_SIZE
        Maximum size of a component (64 bytes).

    CACHE_LINE_SIZE
        Cache line size for alignment (64 bytes).

    PAGE_SIZE
        Page size for alignment (4096 bytes).

AUTHOR
    173duprot <https://github.com/173duprot>

COPYRIGHT
    This library is free software: you can redistribute 
    it and/or modify it under the terms of the GNU 
    General Public License as published by the Free 
    Software Foundation, either version 3 of the 
    License, or (at your option) any later version.

    This library is distributed in the hope that it 
    will be useful, but WITHOUT ANY WARRANTY; without 
    even the implied warranty of MERCHANTABILITY or 
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
    General Public License for more details.

    You should have received a copy of the GNU General 
    Public License along with this library. If not, 
    see <https://www.gnu.org/licenses/>.

ECS(3)                 Library Functions Manual                ECS(3)
