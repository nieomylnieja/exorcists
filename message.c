#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include "message.h"
#include "exorcist.h"

void send_to_all(msg_t msg, msg_tag tag) {
    for (int i = 0; i < E.size; i++) {
        if (i == E.rank) {
            continue;
        }
        MPI_Send(&msg,
                 sizeof(msg_t),
                 MPI_BYTE,
                 i,
                 tag,
                 MPI_COMM_WORLD);
    }
}

void send_prop_requests() {
    msg_t msg = {E.lc, -1};
    send_to_all(msg, REQ_PROP);
}

const int HOUSE_NO_AVAILABLE = -1;
int pick_random_available_house() {
    time_t raw_time;
    time(&raw_time);
    srand(E.rank + raw_time);
    int house_i;
    while (1) {
        house_i = rand() % E.houses_total;
        // Check if the house was already taken.
        for (int rank = 0; rank < E.size; rank++) {
            if (E.resources[rank].house_n == house_i) {
                house_i = HOUSE_NO_AVAILABLE;
                break;
            }
        }
        if (house_i != HOUSE_NO_AVAILABLE) {
            return house_i;
        }
    }
}

void send_house_requests() {
    int house_n = pick_random_available_house();
    E_mark_house_ss_being_haunted(E.rank, house_n);
    msg_t msg = {E.lc, house_n};
    send_to_all(msg, REQ_HOUSE);
}

void send_release() {
    msg_t msg = {};
    send_to_all(msg, RELEASE);
}