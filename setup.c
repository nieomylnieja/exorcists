#include "setup.h"
#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#include "exorcist.h"

#define MIST_GENERATORS_NUM_ENV "MIST_GENERATORS_NUM"
#define TAPE_RECORDER_NUM_ENV "TAPE_RECORDER_NUM"
#define SHEETS_NUM_ENV "SHEETS_NUM"
#define HOUSE_NUM_ENV "HOUSE_NUM"

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
    E.resources = (rank_resources *) malloc(E.houses_total * sizeof(rank_resources));
    for (int i = 0; i < E.houses_total; i++) {
        rank_resources res = {-1, -1};
        E.resources[i] = res;
    }
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
               " - Mist Generators: %d\n"
               " - Tape Recorders \"Kasprzak\": %d\n"
               " - Used-up Sheets: %d\n"
               " - Houses: %d\n\n",
               E.size, props_count[MistGenerator], props_count[TapeRecorder], props_count[Sheets],
               E.houses_total);
    }
}

void run_setup() {
    set_log_level();
    setup_blocking_logging();
    read_envs_and_set_defaults();
    verify_algorithm_constraints();
}