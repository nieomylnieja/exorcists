#ifndef EXORCISTS_MESSAGE_H
#define EXORCISTS_MESSAGE_H

// Message types.
const char *msg_e_names[5];
typedef enum {
    REQ_PROP, REQ_HOUSE, ACK, NACK, RELEASE
} msg_tag;

// General message type.
typedef struct msg_t {
    int lc;          // Lamport's clock timestamp.
    int house_n;     // House number.
} msg_t;

void send_prop_requests();
void send_house_requests();
void send_release();

#endif //EXORCISTS_MESSAGE_H
