#ifndef EXORCISTS_EXORCIST_H
#define EXORCISTS_EXORCIST_H

#include "state.h"

typedef struct {
    int house_n;
    int mist_generator;
} resource_t;

struct exorcist {
    // MPI rank which denotes the exorcist unique id and size = number of all exorcists.
    int rank;
    int size;
    // Lamport's clock.
    int lc;
    // Setup for haunting times.
    int min_haunting_time_seconds;
    int max_haunting_time_seconds;
    // The state an Exorcist is in.
    state_e state;
    // Count the number of response for either house reserving or prop gathering.
    int responses;
    // Number of all houses.
    int houses_total;
    // Number of available houses.
    int houses_available;
    // Number of available mist generators.
    int mist_generators_available;
    // Number of all mist generators.
    int mist_generators_total;
    // List of all the ranks with their resources_state;
    // For houses it's the house number being haunted, -1 if none is;
    // For mist generators It's simply a boolean, whether we already
    // received a NACK or gave an ACK for that resource to the specific rank.
    resource_t *resources_state;
} E;

void E_remove_mist_generator_from_warehouse(int taker);
void E_add_mist_generator_to_warehouse(int releaser);
void E_mark_house_ss_being_haunted(int taker, int house_n);
void E_mark_house_as_being_free(int releaser);

void E_setup_resources_state();
void E_cleanup();

#endif //EXORCISTS_EXORCIST_H
