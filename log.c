#include <stdio.h>
#include <stdarg.h>
#include <strings.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include "log.h"
#include "exorcist.h"

#define LOG_LEVEL_ENV "LOG_LEVEL"

typedef enum log_level_e {
    DEBUG, INFO, ERROR,
    LOG_LEVELS,
} log_level_e;
static const char *log_level_names[] = {"DEBUG", "INFO", "ERROR"};
static const char *log_level_colors[] = {"\x1b[36m", "\x1b[32m", "\x1b[31m"};

const char *state_e_names[] = {
        "Gathering Props",
        "Waiting For Free Props",
        "Reserving House",
        "Waiting For Free House",
        "Haunting House",};

static log_level_e log_level;

sem_t *sem_log;

void log_f(log_level_e level, char *format, va_list args) {
    if (log_level > level) {
        return;
    }
    usleep(1000); // Let the terminal process the logs correctly.
    if (sem_log != NULL) sem_wait(sem_log);
    printf("%s", log_level_colors[level]);
    printf("[T=%d][R=%d] %s ", E.lc, E.rank, log_level_names[level]);
    vprintf(format, args);
    printf(" {State: %s, Houses: %d, Props: %d, Responses: %d}\n",
           state_e_names[E.state], E.houses_available, E.mist_generators_available, E.responses);
    printf("\033[0m");
    if (sem_log != NULL) sem_post(sem_log);
}

void info(char *format, ...) {
    va_list args;
    va_start(args, format);
    log_f(INFO, format, args);
}

void debug(char *format, ...) {
    va_list args;
    va_start(args, format);
    log_f(DEBUG, format, args);
}

void error(char *format, ...) {
    va_list args;
    va_start(args, format);
    log_f(ERROR, format, args);
    exit(1);
}

void set_log_level() {
    char *str_level;
    if ((str_level = getenv(LOG_LEVEL_ENV)) == NULL) {
        log_level = INFO;
        return;
    }
    for (log_level = DEBUG; log_level < LOG_LEVELS; log_level++) {
        if (strcasecmp(str_level, log_level_names[log_level]) == 0) {
            return;
        }
    }
    log_level = ERROR;
    error("failed to set the log level, invalid log level");
}

void setup_blocking_logging() {
    sem_log = sem_open(LOG_SEM, O_CREAT, 0660, 1);
    if (sem_log == SEM_FAILED) {
        error("failed to open semaphore for blocking logging");
    }
}
