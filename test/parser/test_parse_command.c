#include "parser.h"
#include "stdio.h"
#include "string.h"

int main()
{
    char command_val[COMM_LEN];
    char param_val[10][BUFSIZ];

    char login_command[BUFSIZ];
    strcpy(login_command, "login:user1:password1");

    int cnt = parse_command(login_command, command_val, param_val);

    if(strcmp(command_val, "login")){
        printf("login command fail\n");
    }
    if(strcmp(param_val[0], "user1")){
        printf("login username failed\n");
        printf("\texpect %s", "user1");
        printf("\tactually %s", param_val[0]);
    }
    if(strcmp(param_val[1], "password1")){
        printf("login password failed\n");
        printf("\texpect %s", "password1");
        printf("\tactually %s", param_val[1]);
    }

    char rlist_command[BUFSIZ];
    strcpy(rlist_command, "rlist:");

    int param_cnt =  parse_command(rlist_command, command_val, param_val);

    if(strcmp(command_val, "rlist")){
        printf("rlist command fail\n");
    }

    if(param_cnt != 0){
        printf("rlist faild\n");
        printf("\texpect %d", 0);
        printf("\tactually %d", param_cnt);
    }

}