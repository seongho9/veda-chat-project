#include "parser.h"
#include <string.h>
#include <stdio.h>


int parse_command(char* msg, char* command, char* param)
{
    char* tok;

    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);
    strcpy(buf, msg);

    //  끝에 /r/n 처리
    tok = strchr(buf, '\r');
    if(tok!=NULL) {
        *tok = '\0';
    }
    tok = strchr(buf, '\n');
    if(tok!=NULL) {
        *tok = '\0';
    }

    //  command 분리
    tok = strtok(buf, DELIM_STR);
    if(tok==NULL){
        return -1;
    }
    memset(command, 0, COMM_LEN);
    strcpy(command, tok);

    //  이 후 매개변수 분리
    int param_cnt = 0;
    tok = strtok(NULL, DELIM_STR);
    while (tok != NULL) {
        strcpy((param+(BUFSIZ*param_cnt)), tok);
        param_cnt++;
        tok = strtok(NULL, DELIM_STR);
    }

    return param_cnt;
}