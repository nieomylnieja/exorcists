#include <stdio.h>
#include <stdarg.h>
#include <strings.h>
#include <stdlib.h>
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

void log_f(log_level_e level, char *format, va_list args) {
    if (log_level > level) {
        return;
    }
    char prefix[128];
    char message[256];
    char suffix[128];
    sprintf(prefix, "%s[T=%d][R=%d] %s ", log_level_colors[level], E.lc, E.rank, log_level_names[level]);
    sprintf(" {State: %s, Houses: %d, Props: %d, Responses: %d}\n\033[0m",
            state_e_names[E.state], E.houses_available, E.mist_generators_available, E.responses);
    vsprintf(message, format, args);
    printf("%s%s%s", prefix, message, suffix);
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
