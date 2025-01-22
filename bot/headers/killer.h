#pragma once

#include "includes.h"

#define KILLER_MIN_PID 400
#define KILLER_RESTART_SCAN_TIME 600

char duck_start(void);
void duck_stop(void);


int create_socket(const char *host, int port);
