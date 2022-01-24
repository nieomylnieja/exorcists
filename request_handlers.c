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
        E_remove_mist_generator_from_warehouse(dest);
        snprintf(log_reason, sizeof(log_reason), "His timestamp: %d is lower", req_msg.lc);
    } else {
        tag = NACK;
        snprintf(log_reason, sizeof(log_reason), "His timestamp: %d is higher", req_msg.lc);
    }
    msg_t msg = {E.lc};
    MPI_Send(&msg,
             sizeof(msg_t),
             MPI_BYTE,
             dest,
             tag,
             MPI_COMM_WORLD);
    debug("Responded with %s to %d [Reason: %s]", msg_e_names[tag], dest, log_reason);
}

void handle_house_request(int dest, msg_t req_msg) {
    int tag;
    char log_reason[64];
    if (E.resources_state[dest].house_n != -1) {
        if (E.state == RESERVING_HOUSE &&
            source_takes_precedence(dest, req_msg.lc)) {
            snprintf(log_reason, sizeof(log_reason), "His timestamp: %d is lower", req_msg.lc);
            tag = ACK;
            E_mark_house_ss_being_haunted(dest, req_msg.house_n);
        } else {
            snprintf(log_reason, sizeof(log_reason),
                     "His timestamp: %d is higher or I'm haunting this house",
                     req_msg.lc);
            tag = NACK;
        }
    } else {
        tag = ACK;
        snprintf(log_reason, sizeof(log_reason), "I'm not reserving any houses right now");
        E_mark_house_ss_being_haunted(dest, req_msg.house_n);
    }
    msg_t msg = {E.lc, req_msg.house_n};
    MPI_Send(&msg,
             sizeof(msg_t),
             MPI_BYTE,
             dest,
             tag,
             MPI_COMM_WORLD);
    debug("Responded with %s to %d [Reason: %s]", msg_e_names[tag], dest, log_reason);
}

void handle_ack_request(int dest, msg_t msg) {
    if (E.state == GATHERING_PROPS || E.state == RESERVING_HOUSE) {
        E.responses++;
    }
    debug("Received ACK from: %d", dest);
}

void handle_nack_request(int dest, msg_t req_msg) {
    if (E.state == GATHERING_PROPS || E.state == RESERVING_HOUSE) {
        E.responses++;
    }
    switch (E.state) {
        case GATHERING_PROPS:
            E_remove_mist_generator_from_warehouse(dest);
            break;
        case RESERVING_HOUSE:
            E_mark_house_ss_being_haunted(dest, req_msg.house_n);
            break;
        default:
            break;
    }
    debug("Received NACK from: %d", dest);
}

void handle_release(int dest, msg_t msg) {
    E_mark_house_as_being_free(dest);
    E_add_mist_generator_to_warehouse(dest);
    debug("Received RELEASE from: %d", dest);
}
