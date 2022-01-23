#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "state_handlers.h"
#include "exorcist.h"
#include "log.h"
#include "request_handlers.h"

void *start_haunting(void *arg);

void handle_gathering_props() {
    if (E.responses == E.size - 1) {
        E.responses = 0;
        if (E.mist_generators_available > 0) {
            E.lc++;
            if (E.houses_available > 0) {
                E.state = RESERVING_HOUSE;
                E_remove_mist_generator_from_warehouse(E.rank);
                send_house_requests();
                info("Gathered the props, and now reserving the house");
            } else {
                E.state = WAITING_FOR_FREE_HOUSE;
                info("Gathered the props, but now have to wait for a free house");
            }
        } else {
            E.state = WAITING_FOR_FREE_PROPS;
            info("Waiting for free props");
        }
    }
}

void handle_waiting_for_free_props() {
    if (E.mist_generators_available > 0) {
        E.state = GATHERING_PROPS;
        send_prop_requests();
        info("Free props are available again! Requesting them.");
    }
}

void handle_house_reserving() {
    if (E.responses == E.size - 1) {
        E.responses = 0;
        if (E.houses_available > 0) {
            E.lc++;
            E.state = HAUNTING_HOUSE;
            pthread_t haunter;
            pthread_create(&haunter, NULL, start_haunting, NULL);
        } else {
            E.state = WAITING_FOR_FREE_HOUSE;
            info("Was reserving the house, but failed. Gotta try again.");
        }
    }
}

void handle_waiting_for_free_house() {
    if (E.houses_available > 0) {
        E.state = RESERVING_HOUSE;
        send_house_requests();
        info("Free house is available again. Trying to reserve it.");
    }
}

// Do nothing. The sleep thread will handle the change of state.
void handle_haunting_house() {}

// The haunting thread. This has to async.
void *start_haunting(void *arg) {
    int haunting_duration = rand()
                            % (E.max_haunting_time_seconds + 1 - E.min_haunting_time_seconds)
                            + E.min_haunting_time_seconds;
    info("Started haunting house %d for: %ds", E.resources_state[E.rank].house_n, haunting_duration);
    sleep(haunting_duration);

    info("Finished haunting house %d", E.resources_state[E.rank].house_n);
    E_mark_house_as_being_free(E.rank);
    E_add_mist_generator_to_warehouse(E.rank);

    send_release();

    if (E.mist_generators_available > 0) {
        E.state = GATHERING_PROPS;
        send_prop_requests();
    } else {
        E.state = WAITING_FOR_FREE_PROPS;
    }
    return NULL;
}