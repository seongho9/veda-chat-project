#include "parser.h"
#include <string.h>
#include <stdio.h>

int tostring_command(char* msg, char* command, char* param, int param_cnt)
{
    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);

    strcpy(buf, command);

    for(int i=0; i<param_cnt; i++) {
        strcat(buf, DELIM_STR);
        strcat(buf, (param+BUFSIZ*i));
    }

    memcpy(msg, buf, BUFSIZ);

    return 0;
}