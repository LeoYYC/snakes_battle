#ifndef SERVER_H__
#define SERVER_H__

#include "utils/serverlib.h"

#include <stdlib.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/socket.h> /* for socket pair */

#include "utils/config.h"

int accept_client();


#endif // SERVER_H__
