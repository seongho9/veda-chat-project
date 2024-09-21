#include "chat_data.h"
#include "client.h"
#include "parser.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

static struct Msg* msg[10] = {NULL,};
static int msg_cnt = 0;
static int client_w_fd = 0;
static int client_r_fd = 0;
static char room_name[MAX_NAME_LEN];
static int flag = 0;
static int exit_flag = 0;
void chat_sigio(int sig_no);

int dispaly_chatroom(char* userlist, char* roomname, int w_fd, int r_fd)
{
    char input[MAX_MESSAGE_LEN];
    int input_len = 0;    

    char inchr;

    char ulist[MAX_USER][MAX_NAME_LEN];

    char buf[BUFSIZ];
    
    client_w_fd = w_fd;
    client_r_fd = r_fd;
    strcpy(room_name, roomname);

    struct sigaction sig_io, old_io;
    sigfillset(&sig_io.sa_mask);
    sigdelset(&sig_io.sa_mask, SIGIO);
    sig_io.sa_handler=chat_sigio;
    sig_io.sa_flags=SA_RESTART;
    sigaction(SIGIO, &sig_io, &old_io);

    clear_console();

    fcntl(STDIN_FILENO, F_SETFL, O_ASYNC | O_NONBLOCK);
    fcntl(STDIN_FILENO, F_SETOWN, getpid());

    fcntl(client_r_fd, F_SETFL, O_ASYNC | O_NONBLOCK);
    fcntl(client_r_fd, F_SETOWN, getpid());


    memset(input, 0, MAX_MESSAGE_LEN);

    clear_console();
    printf("(exit: \'^]\')>> ");
    fflush(stdout);
    while(1) {
        if(exit_flag){
            break;
        }
        if(flag){
            // stdout으로 출력
            clear_console();
            for(int i=0; i<msg_cnt; i++){
                printf("%s - %s\n", msg[i]->content,msg[i]->sender);
                printf("\t%s\n", ctime(&(msg[i]->time)));
            }
            printf("(exit: \'^]\')>> ");
            fflush(stdout);
            flag = 0;
        }
        else{
            usleep(1000);
        }
    }
    

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    flags &= (~(O_NONBLOCK | O_ASYNC));
    fcntl(STDIN_FILENO, F_SETFL, flags);

    flags = fcntl(client_r_fd, F_GETFL, 0);
    flags &= (~(O_NONBLOCK | O_ASYNC));
    fcntl(r_fd, F_SETFL, flags);

    
    sigaction(SIGIO, &old_io, NULL);

    return 0;
}

void chat_sigio(int sig_no)
{
    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);
    if(read(client_r_fd, buf, BUFSIZ)>0){
        
        struct Msg* tmp = (struct Msg*)malloc(sizeof(struct Msg));
        if(tmp == NULL){
            return;
        }
        char* tok = strtok(buf, DELIM_STR);
        strcpy(tmp->sender, tok);
        tok = strtok(NULL, DELIM_STR);
        strcpy(tmp->content, tok);
        tok = strtok(NULL, DELIM_STR);
        tmp->time = (time_t) atol(tok);

        if(msg_cnt < 10){
            msg[msg_cnt++] = tmp;
        }
        else {
            free(msg[0]);
            for(int i=1; i<10; i++){
                msg[i-1] = msg[i];
            }
            msg[9] = tmp;
        }
    }
    else if(read(STDIN_FILENO, buf, BUFSIZ)>0) {
        buf[strlen(buf)-1] = '\0';
        if(buf[0] == 29){
            exit_flag = 1;
            return;
        }
        char param_stdin[3][BUFSIZ];

        strcpy(param_stdin[0], room_name);
        strcpy(param_stdin[1], buf);

        tostring_command(buf, "chat", (char*)param_stdin, 2);

        write(client_w_fd, buf, BUFSIZ);

        while(read(client_r_fd, buf, BUFSIZ)<=0);
                
        struct Msg* tmp = (struct Msg*)malloc(sizeof(struct Msg));
        if(tmp == NULL){
            return;
        }
        char* tok = strtok(buf, DELIM_STR);
        strcpy(tmp->sender, tok);
        tok = strtok(NULL, DELIM_STR);
        strcpy(tmp->content, tok);
        tok = strtok(NULL, DELIM_STR);
        tmp->time = (time_t) atol(tok);

        if(msg_cnt < 10){
            msg[msg_cnt++] = tmp;
        }
        else {
            free(msg[0]);
            for(int i=1; i<10; i++){
                msg[i-1] = msg[i];
            }
            msg[9] = tmp;
        }
    }
    flag = 1;
}