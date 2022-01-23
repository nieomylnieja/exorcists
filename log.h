#ifndef EXORCISTS_LOG_H
#define EXORCISTS_LOG_H

#define LOG_SEM "EXORCISTS_LOG_SEM"

void info(char *format, ...);
void debug(char *format, ...);
void error(char *format, ...);

void set_log_level();
void setup_blocking_logging();

#endif //EXORCISTS_LOG_H