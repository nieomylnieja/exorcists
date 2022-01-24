#ifndef EXORCISTS_LOG_H
#define EXORCISTS_LOG_H

#define LOG_SEM "EXORCISTS_LOG_SEM"

void info(char *format, ...);
void debug(char *format, ...);
void error(char *format, ...);

void set_log_level();

#endif //EXORCISTS_LOG_H