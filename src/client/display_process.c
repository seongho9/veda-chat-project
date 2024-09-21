#include "client.h"
#include "parser.h"
#include "chat_data.h"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void display_process(char* username, int w_fd, int r_fd)
{
    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);
    char room_list[MAX_ROOM][MAX_NAME_LEN];
    char room_name[MAX_NAME_LEN];
    int room_cnt;
    while(1){
        //  서버로 채팅방 요청
        tostring_command(buf, "rlist", NULL, 0);
        write(w_fd, buf, BUFSIZ);
        
        //  blocking 방식으로 채팅방 리스트를 가져옴
        memset(buf, 0, BUFSIZ);
        if(read(r_fd, buf, BUFSIZ)>0){
            memset(room_list, 0, MAX_ROOM * MAX_NAME_LEN);
            room_cnt = 0;
            //  받아온 채팅방 리스트를 배열화
            char* tok = strtok(buf, ":");
            room_cnt = 0;
            while(tok!=NULL) {
                strcpy((char*)(room_list+MAX_NAME_LEN*room_cnt), tok);
                tok = strtok(NULL, ":");
                room_cnt++;
            }
        }
        //  채팅방 목록 출력
        clear_console();
        printf("chatting room\n");
        for(int i=0; i<room_cnt; i++){
            printf(" %s\n", room_list[i]);
        }
        printf("\n");
        int choice;
        char input;
        printf("choice menu\n");
        printf("1. join room\n");
        printf("2. make room\n");
        printf("3. logout\n");
        printf(">> ");
        fflush(stdout);
        input = getchar(); getchar();
        choice = atoi(&input);

        if(choice == 1){
            memset(buf, 0, BUFSIZ);
            int ret=0;
            //  방에 입장하여 유저를 가져옴
            do {
                
                clear_console();
                if(ret == -1){
                    printf("invalid room name\n");
                }
                else if(ret == -2){
                    printf("room user is max\n");
                }
                else if(ret == -3){
                    printf("network error\n");
                    break;
                }

                ret = dispaly_joinroom(buf, room_name, w_fd, r_fd);

            } while(ret != 0);
            // buf에 <user_name>:<user_name>:... 형식으로 존재함
            dispaly_chatroom(buf, room_name, w_fd, r_fd);
        }
        else if(choice == 2) {
            if(!display_makeroom(w_fd, r_fd)){
            
            }
        }
        else if(choice == 3) {
            memset(buf, 0, BUFSIZ);
            tostring_command(buf, "logout", username, 1);

            write(w_fd, buf, BUFSIZ);

            read(r_fd, buf, BUFSIZ);
            
            break;
        }
    }
}