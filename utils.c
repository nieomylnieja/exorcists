#include <stdio.h>
#include <stdarg.h>
#include "utils.h"

static int colors[4] = {36, 35, 34, 33};

void color_print(int color, char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("\033[0;%dm", colors[color]);
    vprintf(format, args);
    printf("\033[0m");
}
