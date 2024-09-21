#include "client.h"
#include "parser.h"
#include "chat_data.h"

#include <string.h>
#include <unistd.h>

int dispaly_joinroom(char* username, char* roomname, int w_fd, int r_fd)
{
    char room_name[MAX_NAME_LEN];
    char buf[BUFSIZ];


    while(1) {
        printf("room name\n>> ");
        fflush(stdout);
        memset(buf, 0, BUFSIZ);
        fgets(buf, BUFSIZ, stdin);
        buf[strlen(buf)-1] = '\0';

        if(strlen(buf) > (MAX_NAME_LEN-1)) {
            clear_console();
            printf("room name is too long (max length: %d)\n", MAX_NAME_LEN-1);
        }
        else if(strlen(buf) == 0) {
            clear_console();
            printf("input room name\n");
        }
        else {
            memset(room_name, 0, MAX_NAME_LEN);
            strcpy(room_name, buf);
            break;
        }
    }

    tostring_command(buf, "joinr", room_name, 1);

    write(w_fd, buf, BUFSIZ);

    if(read(r_fd, buf, BUFSIZ)>0){
        if(!strcmp(buf, "wrong room name")){
            return -1;
        }
        else if(!strcmp(buf, "max user")){
            return -2;
        }
        else{
            strcpy(username, buf);
            strcpy(roomname, room_name);
            return 0;
        }
    }
    else{
        return -3;
    }
}