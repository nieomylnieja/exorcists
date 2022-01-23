#ifndef EXORCISTS_STATE_H
#define EXORCISTS_STATE_H

typedef enum {
    GATHERING_PROPS,
    WAITING_FOR_FREE_PROPS,
    RESERVING_HOUSE,
    WAITING_FOR_FREE_HOUSE,
    HAUNTING_HOUSE
} state_e;
static const char *state_e_names[] = {
        "Gathering Props",
        "Waiting For Free Props",
        "Reserving House",
        "Waiting For Free House",
        "Haunting House",
};

#endif //EXORCISTS_STATE_H
