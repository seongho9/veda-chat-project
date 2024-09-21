#include "server.h"
#include "parser.h"

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

static int sock;

void client_kill_handler(int sig_no);

int client_proc(int csock, int w_fd, int r_fd)
{
    char buf[BUFSIZ];

    char logout_buf[BUFSIZ];
    char logout_comm[COMM_LEN];
    char param[10][BUFSIZ];

    sock = csock;

    //  접속자가 로그아웃시 자원 정리를 위한 sigterm시그널 핸들러
    struct sigaction sig_killed;
    sigfillset(&sig_killed.sa_mask);
    sigdelset(&sig_killed.sa_mask, SIGTERM);
    sig_killed.sa_handler = client_kill_handler;
    sig_killed.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sig_killed, NULL);


    fcntl(r_fd, F_SETFL, O_NONBLOCK);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    while(1) {
        
        memset(buf, 0, BUFSIZ);
        if(read(r_fd, buf, BUFSIZ) > 0){
            sprintf(buf, "%s", buf);

            write(sock, buf, BUFSIZ);
        }
        
        memset(buf, 0, BUFSIZ);
        if(read(sock, buf, BUFSIZ) > 0){
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

void client_kill_handler(int sig_no)
{
    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);
    strcpy(buf, "0");

    write(sock, buf, BUFSIZ);
    
    close(sock);
    exit(0);
}