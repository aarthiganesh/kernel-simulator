#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>

#include "semaphores.h"
#include "message.h"
#include "dbarray.h"