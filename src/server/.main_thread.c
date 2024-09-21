#include "server.h"

struct User
{
    int pid;
    char name[MAX_NAME_LEN];
    int r_fd, w_fd;
};

void int_handler(int sig_no);
void io_handler(int sig_no);
void child_handler(int sig_no);

struct User* client[MAX_USER];
int client_cnt=0;

int main_proc(int r_fd, int w_fd)
{
    struct sigaction sig_int, old_int;

    sig_int.sa_handler=int_handler;
    sig_int.sa_flags=SA_RESTART;

    sigaction(SIGINT, &sig_int, &old_int);

    struct sigaction sig_child, old_child;

    sig_child.sa_handler = child_handler;
    sig_child.sa_flags=SA_RESTART;

    sigaction(SIGCHLD, &sig_child, &old_child);

    int f_case;
    ssize_t r_cnt=0;
    char buf[BUFSIZ];
    char* tok;
    while(1) {
        r_cnt = read(r_fd, buf, BUFSIZ);

        if(r_cnt>0) {
            tok = strtok(buf, ":");
        }
        // 0 : read 1 : write
        int p_to_c[2];
        int c_to_p[2];

        int r_cnt=0;
        char room_name[MAX_ROOM][MAX_NAME_LEN];

        pipe(p_to_c);   pipe(c_to_p);

        //  유저 로그인(프로세스 생성)
        if(!strcmp(tok, "login")) {
            char name[BUFSIZ], csock_str[5];

            tok = strtok(NULL, "|");
            strcpy(name, tok);
            tok = strtok(NULL, "|");
            strcpy(csock_str, tok);

            f_case=fork();
            switch (f_case)
            {
            //  error
            case -1:
                perror("fork()");
                break;
            //  client process
            case 0:
                // client 처리 logic
                int w_fd = c_to_p[1];
                int r_fd = p_to_c[0];

                client_proc(atoi(csock_str), w_fd, r_fd);
                break;
            //  parente process
            default:

                struct User* tmp = malloc(sizeof(struct User));

                strcpy(tmp->name, name);
                tmp->pid=f_case;
                tmp->w_fd = p_to_c[1];
                tmp->r_fd = c_to_p[0];

                fcntl(tmp->r_fd, F_SETFL, O_ASYNC);

                client[client_cnt++] = tmp;

                memset(buf, 0, BUFSIZ);

                for(int i=0; i<MAX_ROOM; i++) {
                    struct Room* tmp_room;

                    if(total_room(&tmp_room, i)<0){
                        break;
                    }
                    strcat(buf, tmp_room->name);
                    strcat(buf, "|");
                }
                write(w_fd, buf, BUFSIZ);

                break;
            }
        }
        //  방정보 요청
        else if(!strcmp(tok, "rlist")){
            //  유저 이름
            memset(buf, 0, BUFSIZ);

            for(int i=0; i<MAX_ROOM; i++) {
                struct Room* tmp_room;

                if(total_room(&tmp_room, i)<0){
                    break;
                }
                strcat(buf, tmp_room->name);
                strcat(buf, "|");
            }
            write(w_fd, buf, BUFSIZ);
        }
        else{
            strcpy(buf, "invalid command");
            write(w_fd, buf, strlen(buf));
        }

    }
}

void int_handler(int sig_no)
{
    for(int i=0; i<client_cnt; i++) {
        kill(client[i]->pid, SIGTERM);
    }

    exit(0);
}

void child_handler(int sig_no)
{
    waitpid(0, NULL, WNOHANG);
}

void io_handler(int sig_no)
{
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

    }

    char* tok;

    tok = strtok(buf, ":");

    if(!strcmp(tok, "mkr")){
        
        tok = strtok(NULL, "|");

        struct Room* room = (struct Room*) malloc(sizeof(struct Room));
        room->M_HEAD=NULL; room->M_TAIL=NULL; room->message_cnt=0;
        strcpy(room->name, tok);
        room->user_cnt=0;
        insert_room(room, 1);
    }
    else if(!strcmp(tok, "joinr")) {

        tok = strtok(NULL, "|");
        join_room(tok, cur_user->name);

    }
    else if(!strcmp(tok, "exitr")) {
        tok = strtok(NULL, "|");
        exit_room(tok, cur_user->name);
    }
    else if(!strcmp(tok, "chat")) {
        struct Message* msg = (struct Message*)malloc(sizeof(struct Message));

        tok = strtok(NULL, "|");
        strcpy(msg->room_name, tok);

        tok = strtok(NULL, "|");
        strcpy(msg->content, tok);

        strcpy(msg->user, cur_user->name);

        msg->time = time(NULL);

        msg->next = NULL;
        
        insert_message(msg, msg->room_name, 1);

        struct Room* room;

        search_room(&room, msg->room_name);

        for(int i=0; i<room->user_cnt; i++){

            for(int j=0; j<client_cnt; j++) {
                if(!strcmp(room->user_list[i], client[j]->name)) {
                    char msg_buf[BUFSIZ];
                    strcat(msg_buf, msg->content);
                    strcat(msg_buf, ":");
                    strcat(msg_buf,msg->user);
                    write(client[j]->w_fd, msg_buf, BUFSIZ);
                }
            }
        }
    }
    else if(!strcmp(tok, "logout")){

    }
    else{

    }
}