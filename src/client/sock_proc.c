#include "client.h"
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


void sock_proc(int sock, char* addr, int w_fd, int r_fd)
{
    char buf[BUFSIZ];
    printf("%d", sock);

    fcntl(r_fd, F_SETFL, O_NONBLOCK);
    fcntl(sock, F_SETFL, O_NONBLOCK);
    while(1) {

        memset(buf, 0, BUFSIZ);
        //  부모로 부터 입력이 들어옴
        if(read(r_fd, buf, BUFSIZ)>0){
            sprintf(buf, "%s", buf);
            write(sock, buf, BUFSIZ);
        }
        //  네트워크에서 입력이 들어옴
        memset(buf, 0, BUFSIZ);
        if(read(sock, buf, BUFSIZ)>0){
            printf("2buf %s", buf);
            char* replace = strchr(buf, '\r');
            if(replace != NULL)
                *replace = '\0';

            replace = strchr(buf, '\n');
            if(replace != NULL)
                *replace = '\0';
            //  부모로 전송
            write(w_fd, buf, BUFSIZ);
        }
    }
}