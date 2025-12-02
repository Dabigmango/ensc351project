#ifndef _UDP_SERVER_H
#define _UDP_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "musicStorage.h"

bool udpServer_start(int port);
void udpServer_stop(void);

#endif