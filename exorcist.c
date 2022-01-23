#include "exorcist.h"
#include <stdlib.h>

const int HOUSE_AVAILABLE = -1;
const int PROP_TAKEN = 1;
const int PROP_FREE = 0;

void E_remove_tape_recorder_from_warehouse(int taker) {
    if (E.resources[taker].mist_generator == PROP_FREE) {
        E.mist_generators--;
    }
    E.resources[taker].mist_generator = PROP_TAKEN;
}

void E_add_tape_recorder_to_warehouse(int releaser) {
    E.mist_generators++;
    E.resources[releaser].mist_generator = PROP_FREE;
}

void E_mark_house_ss_being_haunted(int taker, int house_n) {
    if (E.resources[taker].house_n != HOUSE_AVAILABLE) {
        E.houses_available--;
    }
    E.resources[taker].house_n = house_n;
}

void E_mark_house_as_being_free(int releaser) {
    E.houses_available++;
    E.resources[releaser].house_n = HOUSE_AVAILABLE;
}

void E_cleanup() {
    free(E.resources);
}
