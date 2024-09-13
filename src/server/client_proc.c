#include "server.h"
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int client_proc(int csock, int w_fd, int r_fd)
{
    char buf[BUFSIZ];

    fcntl(r_fd, F_SETFL, O_NONBLOCK);
    fcntl(csock, F_SETFL, O_NONBLOCK);
    while(1) {
        
        memset(buf, 0, BUFSIZ);
        if(read(r_fd, buf, BUFSIZ) > 0){
            sprintf(buf, "%s", buf);
            write(csock, buf, BUFSIZ);
        }
        
        memset(buf, 0, BUFSIZ);
        if(read(csock, buf, BUFSIZ) > 0){
            char* replace = strchr(buf, '\r');
            if(replace != NULL)
                *replace = '\0';

            replace = strchr(buf, '\n');
            if(replace != NULL)
                *replace = '\0';

            write(w_fd, buf, BUFSIZ);
        }
        
    }
}