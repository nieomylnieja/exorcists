#include "setup.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "log.h"
#include "exorcist.h"

#define MIST_GENERATORS_NUM_ENV "MIST_GENERATORS_NUM"
#define TAPE_RECORDER_NUM_ENV "TAPE_RECORDER_NUM"
#define SHEETS_NUM_ENV "SHEETS_NUM"
#define HOUSE_NUM_ENV "HOUSE_NUM"
#define MIN_HAUNTING_TIME_SECONDS_ENV "MIN_HAUNTING_TIME_SECONDS"
#define MAX_HAUNTING_TIME_SECONDS_ENV "MAX_HAUNTING_TIME_SECONDS"

typedef enum prop_e {
    MistGenerator, TapeRecorder, Sheets
} prop_e;
const char *prop_warehouse_envs[] = {MIST_GENERATORS_NUM_ENV, TAPE_RECORDER_NUM_ENV, SHEETS_NUM_ENV};
int props_count[3] = {0, 0, 0};

int read_int_from_env(const char *env_name) {
    char *raw_int;
    if ((raw_int = getenv(env_name)) != NULL) {
        char *ptr;
        int parsed_int = (int) strtol(raw_int, &ptr, 10);
        if (*ptr) {
            error("%s env should be a number, conversion to int failed", env_name);
        }
        return parsed_int;
    }
    return -1;
}

void read_envs_and_set_defaults() {
    if ((E.min_haunting_time_seconds = read_int_from_env(MIN_HAUNTING_TIME_SECONDS_ENV)) == -1) {
        E.min_haunting_time_seconds = 5;
    }
    if ((E.max_haunting_time_seconds = read_int_from_env(MAX_HAUNTING_TIME_SECONDS_ENV)) == -1) {
        E.max_haunting_time_seconds = 10;
    }
    int l_bound = E.size;
    if (l_bound == 1) {
        l_bound = 2;
    }
    for (int i = 0; i < 3; i++) {
        if ((props_count[i] = read_int_from_env(prop_warehouse_envs[i])) == -1) {
            props_count[i] = l_bound - 1 + i;
        }
    }
    if ((E.houses_total = read_int_from_env(HOUSE_NUM_ENV)) == -1) {
        E.houses_total = l_bound + 2;
    }
    E_setup_resources_state();
    E.houses_available = E.houses_total;
    E.mist_generators_total = props_count[0];
    E.mist_generators_available = E.mist_generators_total;
}

void verify_algorithm_constraints() {
    if (props_count[MistGenerator] >= props_count[TapeRecorder] ||
        props_count[TapeRecorder] >= props_count[Sheets] ||
        props_count[Sheets] >= E.houses_total) {
        error("the setup does not not match the algorithm constraints: M < K < P < D. Was: %d < %d < %d < %d",
              props_count[MistGenerator],
              props_count[TapeRecorder],
              props_count[Sheets],
              E.houses_total);
    }
    // Print the initial setup info once.
    if (E.rank == 0) {
        printf("Running with config: \n"
               " - Size: %d\n"
               " - Haunting time bounds: [%d:%d] seconds\n"
               " - Mist Generators: %d\n"
               " - Tape Recorders \"Kasprzak\": %d\n"
               " - Used-up Sheets: %d\n"
               " - Houses: %d\n\n",
               E.size,
               E.min_haunting_time_seconds, E.max_haunting_time_seconds,
               props_count[MistGenerator], props_count[TapeRecorder], props_count[Sheets],
               E.houses_total);
    }
}

void seed_random() {
    time_t raw_time;
    time(&raw_time);
    srand(E.rank + raw_time);
}

void run_setup() {
    seed_random();
    set_log_level();
    setup_blocking_logging();
    read_envs_and_set_defaults();
    verify_algorithm_constraints();
}