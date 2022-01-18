#include <pthread.h>
#include <mpi.h>
#include "utils.h"

// Message types.
const char *msg_e_names[3] = {"REQ", "ACK", "NACK"};
typedef enum msg_e {
    REQ, ACK, NACK
} msg_e;

// Prop types.
const char *prop_e_names[3] = {
        "Tape Recorder \"Kasprzak\"",
        "Artificial Mist Generator",
        "Used Up Sheets"};
typedef enum prop_e {
    TapeRecorder, MistGenerator, Sheets
} prop_e;
int prop_warehouse_inventory[3] = {2, 3, 4};

// States an exorcists can be in.
const char *state_e_names[3] = {
        "Gathering Props",
        "Reserving House",
        "Haunting House"};
typedef enum state_e {
    GATHERING_PROPS, RESERVING_HOUSE, HAUNTING_HOUSE
} state_e;

// General message type.
typedef struct msg_t {
    int t;
    prop_e p;
} msg_t;

int rank, size, state;
// lc represents the Lamport clocks timestamp.
int lc = 0;

void send_prop_requests(prop_e prop);

_Noreturn void *listen(void *arg);

int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int main(int argc, char **argv) {
    // Initialize the MPI process.
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    state = GATHERING_PROPS;

    send_prop_requests(TapeRecorder);

    pthread_t listener;
    pthread_create(&listener, NULL, listen, NULL);
    pthread_join(listener, NULL);

    MPI_Finalize();
}

void send_prop_requests(prop_e prop) {
    for (int i = 0; i < size; i++) {
        if (i == rank) {
            continue;
        }
        msg_t msg = {++lc, prop};
        MPI_Send(&msg,
                 sizeof(msg_t),
                 MPI_BYTE,
                 i,
                 REQ,
                 MPI_COMM_WORLD);
    }
}

void handle_request(int dest, int our_t, msg_t req_msg) {
    msg_t msg = {++lc, -1};
    int tag;
    if (state != GATHERING_PROPS) {
        tag = ACK;
    } else if (prop_warehouse_inventory[msg.p] == 0) {
        tag = NACK;
    } else if (our_t > req_msg.t || (our_t == req_msg.t && dest > rank)) {
        prop_warehouse_inventory[msg.p]--; // Remove one prop from the inventory.
        tag = ACK;
    } else {
        tag = NACK;
    }
    MPI_Send(&msg,
             sizeof(msg_t),
             MPI_BYTE,
             dest,
             tag,
             MPI_COMM_WORLD);
}

_Noreturn void *listen(void *arg) {
    int requests_gathered = 0;
    int nack_count = 0;
    while (1) {
        MPI_Status status;
        msg_t msg;
        MPI_Recv(&msg,
                 sizeof(msg_t),
                 MPI_BYTE,
                 MPI_ANY_SOURCE,
                 MPI_ANY_TAG,
                 MPI_COMM_WORLD,
                 &status);

        color_print(rank, "[R=%d][T=%d] I've received %s from %d\n",
                    rank,
                    lc,
                    msg_e_names[status.MPI_TAG],
                    status.MPI_SOURCE);

        // Original timestamp.
        int ot = lc;
        // Increment Lamport clock.
        lc = max(lc, msg.t) + 1;

        switch (status.MPI_TAG) {
            case REQ:
                handle_request(status.MPI_SOURCE, ot, msg);
                break;
            case ACK:
                requests_gathered++;
                break;
            case NACK:
                requests_gathered++;
                nack_count++;
                break;
        }

        if (state == GATHERING_PROPS) {
            if (requests_gathered == size) {
                if (prop_warehouse_inventory[TapeRecorder] - nack_count > 0) {
                    color_print(rank, "I'm good to go!");
                } else {
                    color_print(rank, "I'm done boss...");
                }
            }
        }
    }
}
