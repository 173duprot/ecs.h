#include "ecs.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_ENTS 1024
#define MAX_CMPS 32
#define MAX_CMP_SIZE 64
#define SAVE_FILE "ecs_state.dat"

void component_sum_callback(ent_t ent, void* data, void* context) {
    int* sum = (int*)context;
    int* component_data = (int*)data;
    for (size_t i = 0; i < MAX_CMP_SIZE / sizeof(int); ++i) {
        *sum += component_data[i];
    }
}

double get_time_in_seconds(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    struct ECS ecs = {0};
    struct timespec start, end;
    double duration;

    // Test creating the maximum number of entities
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < MAX_ENTS; ++i) {
        ent_t ent = create(&ecs);
        assert(ent != (ent_t)-1);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    duration = get_time_in_seconds(start, end);
    printf("Time taken to create %d entities: %f seconds\n", MAX_ENTS, duration);

    // Test adding the maximum number of components to each entity
    clock_gettime(CLOCK_MONOTONIC, &start);
    int data[MAX_CMP_SIZE / sizeof(int)];
    for (size_t i = 0; i < MAX_CMP_SIZE / sizeof(int); ++i) {
        data[i] = i;
    }
    for (size_t i = 0; i < MAX_ENTS; ++i) {
        for (size_t j = 0; j < MAX_CMPS; ++j) {
            assert(add(&ecs, i, j, data, sizeof(data)) == 0);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    duration = get_time_in_seconds(start, end);
    printf("Time taken to add %d components to each of %d entities: %f seconds\n", MAX_CMPS, MAX_ENTS, duration);

    // Test saving ECS state
    clock_gettime(CLOCK_MONOTONIC, &start);
    assert(save(&ecs, SAVE_FILE) == 0);
    clock_gettime(CLOCK_MONOTONIC, &end);
    duration = get_time_in_seconds(start, end);
    printf("Time taken to save ECS state: %f seconds\n", duration);

    // Test loading ECS state
    struct ECS loaded_ecs = {0};
    clock_gettime(CLOCK_MONOTONIC, &start);
    assert(load(&loaded_ecs, SAVE_FILE) == 0);
    clock_gettime(CLOCK_MONOTONIC, &end);
    duration = get_time_in_seconds(start, end);
    printf("Time taken to load ECS state: %f seconds\n", duration);

    // Test iterating over all entities
    int sum = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    iterate(&loaded_ecs, component_sum_callback, &sum);
    clock_gettime(CLOCK_MONOTONIC, &end);
    duration = get_time_in_seconds(start, end);
    printf("Time taken to iterate over %d entities and sum components: %f seconds\n", MAX_ENTS, duration);
    printf("Sum of component data: %d\n", sum);

    // Test destroying all entities
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < MAX_ENTS; ++i) {
        destroy(&ecs, i);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    duration = get_time_in_seconds(start, end);
    printf("Time taken to destroy %d entities: %f seconds\n", MAX_ENTS, duration);

    return 0;
}
