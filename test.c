// gcc -Ofast -funroll-loops -ffast-math -o test test.c;

#include "ecs.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_ENTS 1024
#define MAX_CMPS 32
#define MAX_CMP_SIZE 64
#define SAVE_FILE "save.dat"
#define NUM_ITERATIONS 10000

void component_sum_callback(ent_t ent, void* data, void* context) {
    int* sum = (int*)context;
    int* component_data = (int*)data;
    for (size_t i = 0; i < MAX_CMP_SIZE / sizeof(int); ++i) {
        *sum += component_data[i];
    }
}

double get_time_in_nanoseconds(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

int main() {
    struct ECS ecs = {0};
    struct timespec start, end;
    double total_create_duration = 0;
    double total_add_duration = 0;
    double total_save_duration = 0;
    double total_load_duration = 0;
    double total_iterate_duration = 0;
    double total_destroy_duration = 0;

    for (size_t iter = 0; iter < NUM_ITERATIONS; ++iter) {
        memset(&ecs, 0, sizeof(ecs));

        // Creating entities
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        for (size_t i = 0; i < MAX_ENTS; ++i) {
            create(&ecs);
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_create_duration += get_time_in_nanoseconds(start, end);

        // Adding components to entities
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        int data[MAX_CMP_SIZE / sizeof(int)];
        for (size_t i = 0; i < MAX_CMP_SIZE / sizeof(int); ++i) {
            data[i] = i;
        }
        for (size_t i = 0; i < MAX_ENTS; ++i) {
            for (size_t j = 0; j < MAX_CMPS; ++j) {
                add(&ecs, i, j, data, sizeof(data));
            }
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_add_duration += get_time_in_nanoseconds(start, end);

        // Saving ECS state
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        save(&ecs, SAVE_FILE);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_save_duration += get_time_in_nanoseconds(start, end);

        // Loading ECS state
        struct ECS loaded_ecs = {0};
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        load(&loaded_ecs, SAVE_FILE);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_load_duration += get_time_in_nanoseconds(start, end);

        // Iterating over entities
        int sum = 0;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        iterate(&loaded_ecs, component_sum_callback, &sum);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_iterate_duration += get_time_in_nanoseconds(start, end);

        // Destroying entities
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        for (size_t i = 0; i < loaded_ecs.ent_count; ++i) {
            destroy(&loaded_ecs, loaded_ecs.ents[i]);
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_destroy_duration += get_time_in_nanoseconds(start, end);
    }

    printf("Average time taken to create %d entities: %.2f nanoseconds\n", MAX_ENTS, total_create_duration / NUM_ITERATIONS);
    printf("Average time taken to add %d components to each of %d entities: %.2f nanoseconds\n", MAX_CMPS, MAX_ENTS, total_add_duration / NUM_ITERATIONS);
    printf("Average time taken to save ECS state: %.2f nanoseconds\n", total_save_duration / NUM_ITERATIONS);
    printf("Average time taken to load ECS state: %.2f nanoseconds\n", total_load_duration / NUM_ITERATIONS);
    printf("Average time taken to iterate over %d entities and sum components: %.2f nanoseconds\n", MAX_ENTS, total_iterate_duration / NUM_ITERATIONS);
    printf("Average time taken to destroy %d entities: %.2f nanoseconds\n", MAX_ENTS, total_destroy_duration / NUM_ITERATIONS);

    return 0;
}
