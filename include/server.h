#ifndef _SERVER_H
#define _SERVER_H
#define USER_SEM_NAME "user_sem"
#include "chat_data.h"
#define PORT 18080

struct User
{
    int pid;
    char name[MAX_NAME_LEN];
    int r_fd, w_fd;
};

int client_proc(int csock, int w_fd, int r_fd);
int accpet_proc();

void io_handler(int sig_no);

#endif