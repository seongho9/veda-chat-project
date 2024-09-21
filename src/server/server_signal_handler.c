#include "server.h"
#include "parser.h"
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void server_io_handler(int sig_no)
{
    char buf[BUFSIZ];
    struct User* cur_user=NULL;

    //  SIGIO 발생한 프로세스를 확인
    int idx=0;
    memset(buf, 0, BUFSIZ);

    while(!get_session_by_idx(&cur_user, idx)){
        if(read(cur_user->r_fd, buf, BUFSIZ) > 0) {
            break;
        }
        idx++;
    }
    if(is_over_cnt(idx)) {
        cur_user = NULL;
    }

    // 예외
    if(cur_user == NULL) {
        fprintf(stderr,"%s:%d SIGIO signal error\n", __FILE__, __LINE__);
        return;
    }

    char command[COMM_LEN];
    char param[10][BUFSIZ];
    memset(command, 0, COMM_LEN);
    memset(param, 0, 10 * BUFSIZ);
    parse_command(buf, command, (char *)param);

    //  mkr:<room_name>
    if(!strcmp(command, "mkr")){
        if(param[0]==NULL) {
            memset(buf, 0, BUFSIZ);
            strcpy(buf, "wrong room name");

            return;
        }

        struct Room* room = (struct Room*) malloc(sizeof(struct Room));
        room->M_HEAD=NULL; room->M_TAIL=NULL; room->message_cnt=0;
        strcpy(room->name, param[0]);
        room->user_cnt=0;
        insert_room(room, 1);

        memset(buf, 0, BUFSIZ);
        sprintf(buf, "0");
        write(cur_user->w_fd, buf,BUFSIZ);
    }
    //  joinr:<room_name>
    else if(!strcmp(command, "joinr")) {

        //  채팅방 이름이 존재하지 않음
        if(param[0]==NULL) {
            memset(buf, 0, BUFSIZ);
            strcpy(buf, "wrong room name");
            write(cur_user->w_fd, buf, BUFSIZ);

            return;
        }
        //  데이터 오류
        if(join_room(param[0], cur_user->name)){
            memset(buf, 0, BUFSIZ);
            strcpy(buf, "max user");
            write(cur_user->w_fd, buf, BUFSIZ);

            return;
        }

        struct Room* room_buf = NULL;
        memset(buf, 0, BUFSIZ);
        if(search_room(&room_buf, param[0])) {
            sprintf(buf, "%d", -1);
            write(cur_user->w_fd, buf, BUFSIZ);

            return;
        }
        for(int i=0; i<room_buf->user_cnt; i++) {
            strcat(buf, room_buf->user_list[i]);
            strcat(buf, ":");
        }
        buf[strlen(buf)-1] = '\0';
        write(cur_user->w_fd, buf,BUFSIZ);
    }
    //  exitr:<room_name>
    else if(!strcmp(command, "exitr")) {

        //  채팅방 이름이 존재하지 않음
        if(param[0]==NULL) {
            memset(buf, 0, BUFSIZ);
            strcpy(buf, "wrong room name");
            write(cur_user->w_fd, buf, BUFSIZ);

            return;
        }
        //  데이터 오류
        if(exit_room(param[0], cur_user->name)){
            memset(buf, 0, BUFSIZ);
            strcpy(buf, "max user");
            write(cur_user->w_fd, buf, BUFSIZ);

            return;
        }

        memset(buf, 0, BUFSIZ);
        sprintf(buf, "0");
        write(cur_user->w_fd, buf,BUFSIZ);
    }
    //  chat:<room_name>:<message>
    else if(!strcmp(command, "chat")) {
        printf("chat\n");
        struct Message* msg = (struct Message*)malloc(sizeof(struct Message));

        //  채팅방 이름
        strcpy(msg->room_name, param[0]);
        //  채팅 내용
        strcpy(msg->content, param[1]);
        //  채팅 보낸 유저
        strcpy(msg->user, cur_user->name);
        //  채팅 시각
        msg->time = time(NULL);
        //  다음 메시지
        msg->next = NULL;
        
        insert_message(msg, msg->room_name, 1);

        //  들어온 메시지를 방 멤버에게 전송
        struct Room* room;
        search_room(&room, msg->room_name);

        memset(buf, 0, BUFSIZ);
        sprintf(buf, "%s:%s:%ld",msg->user, msg->content, msg->time);

        struct User* send_client = NULL;
        int send_idx = 0;

        for(int i=0; i<room->user_cnt; i++){
            while(!get_session_by_idx(&send_client, send_idx)){
                if(!strcmp(send_client->name, room->user_list[i])) {
                    printf("%s\t", send_client->name);
                    write(send_client->w_fd, buf, BUFSIZ);
                    send_idx = 0;
                    break;
                }
                send_idx++;
            }
        }
        printf("\n");
    }
    //  rlist:
    else if(!strcmp(command, "rlist")){
        printf("rlist\n");
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
        printf("%s\n", buf);
        write(cur_user->w_fd, buf, BUFSIZ);
    }
    //  find:<room_name>:<message>:<request_cnt>
    else if(!strcmp(command, "find")) {

        int req_cnt=0;
        struct Message* msg_buf;


        memset(buf, 0, BUFSIZ);
        if(get_message(&msg_buf, param[0], param[1], atoi(param[2]))) {
            strcpy(buf, "No data");
        }
        write(cur_user->w_fd, buf, BUFSIZ);

    }
    //  logout:<user_name>
    else if(!strcmp(command, "logout")){

        memset(buf, 0, BUFSIZ);
        if(!logout_session(param[0])){
            strcpy(buf, "0");
            write(cur_user->w_fd, buf, BUFSIZ);
        }
        else{
            strcpy(buf, "-1");
            write(cur_user->w_fd, buf, BUFSIZ);
        }
    }
    //  delete_user:<user_name>:<password>
    else if(!strcmp(command, "delete_user")) {

        struct UserData user_buf;
        memset(user_buf.name, 0, MAX_NAME_LEN);
        strcpy(user_buf.name, param[0]);

        memset(user_buf.password, 0, MAX_NAME_LEN);
        strcpy(user_buf.password, param[1]);

        if(remove_user(&user_buf)){
            memset(buf, 0, BUFSIZ);
            strcpy(buf, "invalid");
        }
        else{
            memset(buf, 0, BUFSIZ);
            strcpy(buf, "0");
        }
    }
    else{
        printf("%s:%d\n",__FILE__, __LINE__);
        memset(buf, 0, BUFSIZ);
        sprintf(buf, "Invalid");
        write(cur_user->w_fd, buf, BUFSIZ);
    }
}

void server_int_signal_handler(int sig_no)
{
    struct User* session;
    int idx=0;
    printf("kill child\n");
    fflush(stdout);
    while(!get_session_by_idx(&session, idx)){
        printf("kill pid %d\n", session->pid);
        
        kill(session->pid, SIGTERM);
        idx++;
    }
    exit(0);
}

void server_child_signal_handler(int sig_no)
{
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0){
        if(WIFEXITED(status)) {
            printf("%d exited\n", pid);
        } 
        else if(WIFSIGNALED(status)) {
            printf("%d killed by signal\n", pid);
        }
    }
}
