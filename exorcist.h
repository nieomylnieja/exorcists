#ifndef EXORCISTS_EXORCIST_H
#define EXORCISTS_EXORCIST_H

#include "state.h"

typedef struct {
    int house_n;
    int mist_generator;
} rank_resources;

struct exorcist {
    // MPI rank which denotes the exorcist unique id and size = number of all exorcists.
    int rank;
    int size;
    // Lamport's clock.
    int lc;
    // The state an Exorcist is in.
    state_e state;
    // Count the number of response for either house reserving or prop gathering.
    int responses;
    // Number of all houses.
    int houses_total;
    // Number of available houses.
    int houses_available;
    // Number of available mist generators.
    int mist_generators;
    // List of all the ranks with their resources;
    // For houses it's the house number being haunted, -1 if none is;
    // For mist generators It's simply a boolean, whether we already
    // received a NACK or gave an ACK for that resource to the specific rank.
    rank_resources *resources;
} E;

void E_remove_tape_recorder_from_warehouse(int taker);
void E_add_tape_recorder_to_warehouse(int releaser);
void E_mark_house_ss_being_haunted(int taker, int house_n);
void E_mark_house_as_being_free(int releaser);
void E_cleanup();

#endif //EXORCISTS_EXORCIST_H
