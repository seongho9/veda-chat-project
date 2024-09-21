#include "client.h"
#include "parser.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int connect_server(const char *addr)
{
    struct sockaddr_in serv_addr;
    char mesg[BUFSIZ];
    int ssock;

    if((ssock = socket(AF_INET, SOCK_STREAM, 0))<0) {
        perror("socket");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    inet_pton(AF_INET, addr, &(serv_addr.sin_addr.s_addr));
    serv_addr.sin_port = htons(PORT);

    if(connect(ssock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0){
        perror("connect");
        return -1;
    }

    return ssock;
}

int get_userinfo(char* param)
{
    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);

    char reg_param[2][BUFSIZ];

    memset(reg_param[0], 0, BUFSIZ);
    memset(reg_param[1], 0, BUFSIZ);

    //  user name input
    clear_console();
    while(1) {
        printf("username\n>> ");
        fflush(stdout);
        fgets(buf, BUFSIZ, stdin);
        buf[strlen(buf)-1] = '\0';
        if(strlen(buf) > (MAX_NAME_LEN-1)) {
            clear_console();
            printf("user name is too long (max length : %d)\n", MAX_NAME_LEN-1);
            memset(buf, 0, BUFSIZ);
        }
        else if(strlen(buf) == 0) {
            clear_console();
            printf("input user name\n");
            memset(buf, 0, BUFSIZ);
        }
        else {
            strcpy(reg_param[0], buf);
            memset(buf, 0, BUFSIZ);
            break;
        }
    }
    //  password input
    while(1) {

        enable_raw_mode();
        printf("password\n>> ");
        fflush(stdout);
        fgets(buf, BUFSIZ, stdin);
        buf[strlen(buf)-1] = '\0';
        disable_raw_mode();
        printf("\n");
        
        if(strlen(buf) > (MAX_NAME_LEN-1)) {
            clear_console();
            printf("password is too long (max length : %d)\n", MAX_NAME_LEN-1);
            printf("username\n>> %s\n", reg_param[0]);
            fflush(stdout);
            memset(buf, 0, BUFSIZ);
        }
        else if(strlen(buf) == 0) {
            clear_console();
            printf("input password\n");
            printf("username\n>> %s", reg_param[0]);
            fflush(stdout);           
            memset(buf, 0, BUFSIZ);
        }
        else {
            strcpy(reg_param[1], buf);
            memset(buf, 0, BUFSIZ);
            break;
        }
    }

    memcpy(param, reg_param[0], BUFSIZ);
    memcpy(param+BUFSIZ, reg_param[1], BUFSIZ);

    return 0;
}
int register_member(const char *addr)
{
    int ssock = connect_server(addr);

    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);

    char reg_param[2][BUFSIZ];
    memset(reg_param, 0, 2*BUFSIZ);

    get_userinfo((char*)reg_param);

    tostring_command(buf, "reg", (char *)reg_param, 2);

    write(ssock, buf, BUFSIZ);

    read(ssock, buf, BUFSIZ);
    if(!strcmp(buf,"0")) {
        return 0;
    }
    else{
        return -1;
    }

}
int login_member(char* username, const char *addr)
{
    int ssock = connect_server(addr);

    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);

    char login_param[2][BUFSIZ];
    memset(login_param, 0, 2*BUFSIZ);

    get_userinfo((char*)login_param);

    tostring_command(buf, "login", (char*)login_param, 2);

    write(ssock, buf, BUFSIZ);

    read(ssock, buf, BUFSIZ);
    if(!strcmp(buf,"-1")) {
        return -1;
    }
    else{
        strcpy(username, buf);
        return ssock;
    }
}