#include <stdio.h>
#include <mpi.h>
#include "request_handlers.h"
#include "exorcist.h"
#include "log.h"

int source_takes_precedence(int source, int source_lc) {
    return (E.lc > source_lc || (E.lc == source_lc && source > E.rank));
}

void handle_prop_request(int dest, msg_t req_msg) {
    int tag;
    char log_reason[64];
    if (source_takes_precedence(dest, req_msg.lc)) {
        tag = ACK;
        E_remove_tape_recorder_from_warehouse(dest);
        snprintf(log_reason, sizeof(log_reason), "His timestamp: %d is lower than mine: %d", req_msg.lc, E.lc);
    } else {
        tag = NACK;
        snprintf(log_reason, sizeof(log_reason), "His timestamp: %d is higher than mine: %d", req_msg.lc, E.lc);
    }
    msg_t msg = {E.lc};
    MPI_Send(&msg,
             sizeof(msg_t),
             MPI_BYTE,
             dest,
             tag,
             MPI_COMM_WORLD);
    debug("I responded with %s to %d [Reason: %s]", msg_e_names[tag], dest, log_reason);
}

void handle_house_request(int dest, msg_t req_msg) {
    int tag;
    char log_reason[64];
    if (E.resources[dest].house_n != -1) {
        if (E.state == RESERVING_HOUSE &&
            source_takes_precedence(dest, req_msg.lc)) {
            tag = ACK;
            E_mark_house_ss_being_haunted(dest, req_msg.house_n);
        } else {
            tag = NACK;
        }
    } else {
        tag = ACK;
        E_mark_house_ss_being_haunted(dest, req_msg.house_n);
    }
    msg_t msg = {E.lc, req_msg.house_n};
    MPI_Send(&msg,
             sizeof(msg_t),
             MPI_BYTE,
             dest,
             tag,
             MPI_COMM_WORLD);
    debug("I responded with %s to %d [Reason: %s]", msg_e_names[tag], dest, log_reason);
}

void handle_ack_request(int dest, msg_t) {
    E.responses++;
    debug("received ACK from: %d", dest);
}

void handle_nack_request(int dest, msg_t req_msg) {
    switch (E.state) {
        case GATHERING_PROPS:
            E_remove_tape_recorder_from_warehouse(dest);
            break;
        case RESERVING_HOUSE:
            E_mark_house_ss_being_haunted(dest, req_msg.house_n);
            break;
        default:
            break;
    }
    debug("received NACK from: %d", dest);
}

void handle_release(int dest, msg_t) {
    E_mark_house_as_being_free(dest);
    E_add_tape_recorder_to_warehouse(dest);
    debug("received RELEASE from: %d", dest);
}
