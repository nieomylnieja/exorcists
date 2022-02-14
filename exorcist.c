#include <stdlib.h>
#include "exorcist.h"
#include "log.h"

const int HOUSE_AVAILABLE = -1;
const int PROP_TAKEN = 1;
const int PROP_FREE = 0;

void E_remove_mist_generator_from_warehouse(int taker) {
    if (E.resources_state[taker].mist_generator == PROP_FREE) {
        E.mist_generators_available--;
    }
    E.resources_state[taker].mist_generator = PROP_TAKEN;
}

void E_add_mist_generator_to_warehouse(int releaser) {
    E.mist_generators_available++;
    if (E.mist_generators_available > E.houses_total) {
        error("available mist generators exceeded their total number");
    }
    E.resources_state[releaser].mist_generator = PROP_FREE;
}

void E_mark_house_ss_being_haunted(int taker, int house_n) {
    if (E.resources_state[taker].house_n == HOUSE_AVAILABLE) {
        E.houses_available--;
    }
    E.resources_state[taker].house_n = house_n;
}

void E_mark_house_as_being_free(int releaser) {
    E.houses_available++;
    if (E.houses_available > E.houses_total) {
        error("available houses exceeded their total number");
    }
    E.resources_state[releaser].house_n = HOUSE_AVAILABLE;
}

void E_setup_resources_state() {
    E.resources_state = (resource_t *) malloc(E.size * sizeof(resource_t));
    for (int i = 0; i < E.size; i++) {
        resource_t res = {-1, 0};
        E.resources_state[i] = res;
    }
}

void E_cleanup() {
    free(E.resources_state);
}
