#include"server.h"
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>

#define PORT 18080

struct User* client[MAX_USER] = { NULL, };
int client_cnt = 0;
sem_t *user_sem = NULL;

void int_signal_handler(int sig_no);
void child_signal_handler(int sig_no);
void io_handler(int sig_no);

int main_proc(int ssock, char* buf);

int accpet_proc()
{
    if(user_sem == NULL){
        user_sem = sem_open(USER_SEM_NAME, O_CREAT, 0600);
    }
    struct sigaction sig_int, old_int;
    sigfillset(&sig_int.sa_mask);
    sigdelset(&sig_int.sa_mask, SIGINT);
    sig_int.sa_handler=int_signal_handler;
    sig_int.sa_flags=SA_RESTART;

    sigaction(SIGINT, &sig_int, &old_int);

    struct sigaction sig_child, old_child;
    sigfillset(&sig_child.sa_mask);
    sigdelset(&sig_child.sa_mask, SIGCHLD);
    sig_child.sa_handler = child_signal_handler;
    sig_child.sa_flags=SA_RESTART;

    sigaction(SIGCHLD, &sig_child, &old_child);

    struct sigaction sig_io, old_io;
    sigfillset(&sig_io.sa_mask);
    sigdelset(&sig_io.sa_mask, SIGIO);
    sig_io.sa_handler=io_handler;
    sig_io.sa_flags=SA_RESTART;

    sigaction(SIGIO, &sig_io, &old_io);

    sigset_t sigmask;
    sigfillset(&sigmask);
    sigdelset(&sigmask, SIGINT);
    sigdelset(&sigmask, SIGCHLD);
    sigdelset(&sigmask, SIGIO);
    sigprocmask(SIG_SETMASK, &sigmask, NULL);
    int ssock;
    socklen_t clen;

    struct sockaddr_in server_addr, client_addr;
    char buf[BUFSIZ];

    if((ssock=socket(AF_INET, SOCK_STREAM, 0))<0) {
        perror("socket()");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    int yes = 1;
    if(setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
       perror("setsockopt");
    }

    if(bind(ssock, (struct sockaddr* )&server_addr, sizeof(server_addr))<0) {
        perror("bind()");
        return -1;
    }
    
    if(listen(ssock, 10)<0){
        perror("listen()");
        return -1;
    }
    printf("listen\n");
    clen = sizeof(client_addr);
    while(1) {
        // 0: read 1: write
        int p_to_c[2];
        int c_to_p[2];
        
        pipe(p_to_c);
        pipe(c_to_p);

        int n, csock = accept(ssock, (struct sockaddr*)&client_addr, &clen);
        printf("accept\n");
        // 세션 연결과 로그인 사이에 공백?
        char* tok;

        while(1){
            if(read(csock, buf, BUFSIZ) > 0){
                char* replace = strchr(buf, '\r');
                if(replace != NULL)
                    *replace = '\0';
                replace = strchr(buf, '\n');
                if(replace != NULL)
                    *replace = '\0';
                sprintf(buf,"%s:%d", buf, csock);
                if(!main_proc(csock, buf)){
                    csock=-1;
                    break;
                }
            }
        }

    }
}

void int_signal_handler(int sig_no)
{

    for(int i=0; i<client_cnt; i++){
        printf("%d\n", client[i]->pid);

        kill(client[i]->pid, SIGTERM);
    }

    exit(0);
}

void child_signal_handler(int sig_no)
{
    waitpid(0, NULL, WNOHANG);
}

int main_proc(int ssock, char* buf)
{
    int pid;
    char* tok;
    while(1) {
        tok = strtok(buf, ":");
    
        // 0 : read 1 : write
        int p_to_c[2], c_to_p[2];
        pipe(p_to_c);   pipe(c_to_p);

        int r_cnt=0;
        char room_name_buf[MAX_ROOM][MAX_NAME_LEN];


        //  유저 로그인(프로세스 생성)
        if(!strcmp(tok, "login")) {
            printf("login\n");
            char name[BUFSIZ], csock_str[5];

            tok = strtok(NULL, ":");

            strcpy(name, tok);
            tok = strtok(NULL, ":");
            strcpy(csock_str, tok);

            pid=fork();
            if(pid < 0 ){
                perror("fork()");
            }
            else if(pid==0){
                // client 처리 logic
                int w_fd = c_to_p[1];
                close(c_to_p[0]);
                int r_fd = p_to_c[0];
                close(p_to_c[1]);

                client_proc(atoi(csock_str), w_fd, r_fd);
            }
            else {
                struct User* tmp = malloc(sizeof(struct User));

                strcpy(tmp->name, name);
                tmp->pid=pid;
                tmp->w_fd = p_to_c[1];
                close(p_to_c[0]);
                tmp->r_fd = c_to_p[0];
                close(c_to_p[1]);

                fcntl(tmp->r_fd, F_SETFL, O_ASYNC | O_NONBLOCK);
                fcntl(tmp->r_fd, F_SETOWN, getpid());

                sem_wait(user_sem);
                client[client_cnt++] = tmp;
                sem_post(user_sem);

                memset(buf, 0, BUFSIZ);

                for(int i=0; i<MAX_ROOM; i++) {
                    struct Room* tmp_room;

                    if(total_room(&tmp_room, i)<0){
                        break;
                    }
                    strcat(buf, tmp_room->name);
                    strcat(buf, ":");
                }
                if(strlen(buf)==0){
                    sprintf(buf, "No room");
                }
                write(ssock, buf, BUFSIZ);

            }
            return 0;
        }
        else{
            strcpy(buf, "invalid command");
            write(ssock, buf, strlen(buf));

            return -1;
        }
    }
}

void io_handler(int sig_no)
{
    sem_wait(user_sem);
    char buf[BUFSIZ];
    struct User* cur_user=NULL;

    for(int i=0; i<client_cnt; i++) {
        if(read(client[i]->r_fd, buf, BUFSIZ) > 0) {
            cur_user = client[i];
            break;
        }
    }
    // 예외
    if(cur_user == NULL) {
        fprintf(stderr,"%s:%d SIGIO signal error\n", __FILE__, __LINE__);
        return;
    }

    //*) = '\0';
    char* re = strchr(buf, '\n');
    if(re != NULL)
        *re='\0';
    char* tok;
    tok = strtok(buf, ":");

    //  mkr:<room_name>
    if(!strcmp(tok, "mkr")){
        
        tok = strtok(NULL, ":");

        struct Room* room = (struct Room*) malloc(sizeof(struct Room));
        room->M_HEAD=NULL; room->M_TAIL=NULL; room->message_cnt=0;
        strcpy(room->name, tok);
        room->user_cnt=0;
        insert_room(room, 1);
        memset(buf, 0, BUFSIZ);
        sprintf(buf, "0");
        write(cur_user->w_fd, buf,BUFSIZ);
    }
    //  joinr:<room_name>
    else if(!strcmp(tok, "joinr")) {
        tok = strtok(NULL, ":");
        char roombuf[BUFSIZ];
        memset(roombuf, 0, BUFSIZ);
        strcpy(roombuf, tok);
        join_room(roombuf, cur_user->name);
        struct Room* r;
        
        memset(buf, 0, BUFSIZ);
        if(!search_room(&r, tok)){
            sprintf(buf, "Invalid");
        }
        if(r==NULL){
            return;
        }
        for(int i=0; i<r->user_cnt; i++){
            strcat(buf, r->user_list[i]);
            strcat(buf, ":");
        }
        write(cur_user->w_fd, buf, BUFSIZ);
        printf("joinr\n");
    }
    //  exitr:<room_name>
    else if(!strcmp(tok, "exitr")) {
        tok = strtok(NULL, ":");
        exit_room(tok, cur_user->name);
        memset(buf, 0, BUFSIZ);
        sprintf(buf,"0");
        write(cur_user->w_fd, buf, BUFSIZ);
    }
    //  chat:<room_name>:<message>
    else if(!strcmp(tok, "chat")) {
        struct Message* msg = (struct Message*)malloc(sizeof(struct Message));

        tok = strtok(NULL, ":");
        strcpy(msg->room_name, tok);
        tok = strtok(NULL, ":");
        
        strcpy(msg->content, tok);
        strcpy(msg->user, cur_user->name);
        msg->time = time(NULL);
        msg->next = NULL;
        
        insert_message(msg, msg->room_name, 1);

        struct Room* room;

        search_room(&room, msg->room_name);


        char msg_buf[BUFSIZ];
        memset(msg_buf, 0, BUFSIZ);
        sprintf(msg_buf, "%s:%s:%ld",msg->user, msg->content, msg->time);

        for(int i=0; i<room->user_cnt; i++){
            for(int j=0; j<client_cnt; j++) {
                write(client[j]->w_fd, msg_buf, BUFSIZ);
            }
        }
    }
    //  rlist:
    else if(!strcmp(tok, "rlist")){

        memset(buf, 0, BUFSIZ);
        for(int i=0; i<MAX_ROOM; i++) {
            struct Room* tmp_room;

            if(total_room(&tmp_room, i)<0){
                break;
            }
            strcat(buf, tmp_room->name);
            strcat(buf, ":");
        }
        buf[strlen(buf)-1] = '\0';
        write(cur_user->w_fd, buf, BUFSIZ);
        printf("rlist");
    }
    //  find:<room_name>:<message>:<request_cnt>
    else if(!strcmp(tok, "find")) {
        char room_name[BUFSIZ];
        char msg[BUFSIZ];
        int req_cnt=0;
        memset(room_name, 0, BUFSIZ);
        memset(msg, 0, BUFSIZ);

        tok = strtok(NULL, ":");
        strcpy(room_name, tok);

        tok = strtok(NULL, ":");
        strcpy(msg, tok);

        tok = strtok(NULL, ":");
        req_cnt = atoi(tok);

        char ret[BUFSIZ];
        memset(ret, 0, BUFSIZ);

        if(get_message(ret, room_name, msg, req_cnt)) {
            sprintf(ret, "No data");
        }
        write(cur_user->w_fd, ret, BUFSIZ);

    }
    else if(!strcmp(tok, "logout")){

        tok = strtok(NULL, ":");
        int flag = 0;
        for(int i=0; i<client_cnt; i++) {
            if(flag){
                client[i-1] = client[i];
                continue;
            }
            if(!strcmp(client[i]->name, tok)) {
                kill(client[i]->pid, SIGTERM);
                free(client[i]);
                client[i]=NULL;
                flag=1;
            }
        }
        if(flag){
            client_cnt--;
        }
    }
    else{
        printf("%s:%d\n",__FILE__, __LINE__);
        memset(buf, 0, BUFSIZ);
        sprintf(buf, "Invalid");
        write(cur_user->w_fd, buf, BUFSIZ);
    }
}