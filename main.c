#include <mpi.h>
#include <signal.h>
#include <semaphore.h>
#include "main.h"
#include "log.h"
#include "state.h"
#include "request_handlers.h"
#include "state_handlers.h"
#include "exorcist.h"
#include "setup.h"

int main(int argc, char **argv) {
    // Signal handler
    signal(SIGINT, sig_handler);

    // Initialize the MPI process.
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &E.size);
    MPI_Comm_rank(MPI_COMM_WORLD, &E.rank);

    // Initialize process state and configuration.
    run_setup();
    E.state = GATHERING_PROPS;

    // Start the actual program.
    start_main_event_loop();
}

_Noreturn void start_main_event_loop() {
    send_prop_requests();
    int loop_i = 0;
    while (1) {
//        debug("Loop iteration: %d", loop_i);

        msg_t msg;
        MPI_Status status;
        MPI_Recv(&msg,
                 sizeof(msg_t),
                 MPI_BYTE,
                 MPI_ANY_SOURCE,
                 MPI_ANY_TAG,
                 MPI_COMM_WORLD,
                 &status);

        // State agnostic message handling.
        switch (status.MPI_TAG) {
            case REQ_PROP:
                handle_prop_request(status.MPI_SOURCE, msg);
                break;
            case REQ_HOUSE:
                handle_house_request(status.MPI_SOURCE, msg);
                break;
            case ACK:
                handle_ack_request(status.MPI_SOURCE, msg);
                break;
            case NACK:
                handle_nack_request(status.MPI_SOURCE, msg);
                break;
            case RELEASE:
                handle_release(status.MPI_SOURCE, msg);
                break;
            default:
                error("Invalid status value. Exiting.");
        }

        // State specific handlers.
        switch (E.state) {
            case GATHERING_PROPS:
                handle_gathering_props();
                break;
            case WAITING_FOR_FREE_PROPS:
                handle_waiting_for_free_props();
                break;
            case RESERVING_HOUSE:
                handle_house_reserving();
                break;
            case WAITING_FOR_FREE_HOUSE:
                handle_waiting_for_free_house();
                break;
            case HAUNTING_HOUSE:
                handle_haunting_house();
                break;
            default:
                error("Invalid state value. Exiting.");
        }

        loop_i++;
    }
}

void sig_handler(int num) {
    debug("Finishing process due to SIGINT. Running cleanup.");
    MPI_Finalize();
    sem_unlink(LOG_SEM);
    E_cleanup();
}
