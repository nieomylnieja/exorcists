#include <pthread.h>
#include <unistd.h>
#include "state_handlers.h"
#include "exorcist.h"
#include "log.h"
#include "message.h"

void *start_haunting(void *arg);

void handle_gathering_props() {
    if (E.responses == E.size - 1) {
        E.responses = 0;
        if (E.mist_generators > 0) {
            E.lc++;
            if (E.houses_available > 0) {
                E.state = RESERVING_HOUSE;
                send_house_requests();
            } else {
                E.state = WAITING_FOR_FREE_HOUSE;
            }
        } else {
            E.state = WAITING_FOR_FREE_PROPS;
        }
    }
}

void handle_waiting_for_free_props() {
    if (E.mist_generators > 0) {
        E.state = GATHERING_PROPS;
        send_prop_requests();
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
        }
    }
}

void handle_waiting_for_free_house() {
    if (E.houses_available > 0) {
        E.state = RESERVING_HOUSE;
        send_house_requests();
    }
}

// Do nothing. The sleep thread will handle the change of state.
void handle_haunting_house() {}

// The haunting thread. This has to async.
void *start_haunting(void *) {
    int haunting_duration = 5;
    info("started haunting house for: %ds", haunting_duration);
    sleep(haunting_duration);

    E_mark_house_as_being_free(E.rank);
    if (E.mist_generators > 0) {
        E.state = GATHERING_PROPS;
    } else {
        E.state = WAITING_FOR_FREE_PROPS;
    }

    send_release();
    return NULL;
}