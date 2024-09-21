#include "client.h"

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

static int client_sockd;

void sig_kill_handler(int sig_no);

void client_sock(int sock, int w_fd, int r_fd)
{
    struct sigaction sig_killed;
    sigfillset(&sig_killed.sa_mask);
    sigdelset(&sig_killed.sa_mask, SIGTERM);
    sig_killed.sa_handler = sig_kill_handler;
    sig_killed.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sig_killed, NULL);


    char buf[BUFSIZ];
    client_sockd = sock;

    fcntl(r_fd, F_SETFL, O_NONBLOCK);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    while(1) {
        memset(buf, 0, BUFSIZ);
        if(read(r_fd, buf, BUFSIZ) > 0) {
            write(sock, buf, BUFSIZ);

            char* tok = strtok(buf, ":");
            if(!strcmp(tok, "logout")){
                close(sock);
                exit(0);
            }
        }

        memset(buf, 0, BUFSIZ);
        if(read(sock, buf, BUFSIZ) > 0) {
            char *tok = strchr(buf, '\r');
            if(tok != NULL)
                *tok = '\0';
            
            tok = strchr(buf, '\n');
            if(tok != NULL)
                *tok = '\0';
            write(w_fd, buf, BUFSIZ);
        }
    }
}

void sig_kill_handler(int sig_no)
{
    close(client_sockd);

    exit(0);
}