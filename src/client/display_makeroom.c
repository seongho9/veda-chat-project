#include "client.h"
#include "parser.h"
#include "chat_data.h"

#include <string.h>
#include <unistd.h>

int display_makeroom(int w_fd, int r_fd)
{
    char room_name[BUFSIZ];
    char buf[BUFSIZ];

    clear_console();
    //  올바른 방 정보를 얻기위한 loop
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
            memset(room_name, 0, BUFSIZ);
            strcpy(room_name, buf);
            break;
        }
    }

    tostring_command(buf, "mkr", room_name, 1);

    write(w_fd, buf, BUFSIZ);

    if(read(r_fd, buf,BUFSIZ)>0){
        if(!strcmp(buf, "0")){
            return 0;
        }
        else {
            return -1;
        }
    }
}