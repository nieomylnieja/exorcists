#ifndef EXORCISTS_REQUEST_HANDLERS_H
#define EXORCISTS_REQUEST_HANDLERS_H

#include "message.h"

void handle_prop_request(int dest, msg_t req_msg);
void handle_house_request(int dest, msg_t req_msg);
void handle_ack_request(int dest, msg_t);
void handle_nack_request(int dest, msg_t);
void handle_release(int dest, msg_t);

#endif //EXORCISTS_REQUEST_HANDLERS_H
