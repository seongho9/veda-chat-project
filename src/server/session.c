#include "server.h"
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static struct User* client[MAX_USER] = { NULL, };
static int client_cnt = 0;
sem_t* user_session_sem = NULL;

int login_session(struct User* data) 
{
    if(user_session_sem == NULL) {
        user_session_sem = sem_open(SESSION_SEM_NAME, O_CREAT, 0600, 1);
    }
    sem_wait(user_session_sem);

    if(client_cnt >= MAX_USER) {
        sem_post(user_session_sem);

        return -1;
    }
    client[client_cnt++] = data;

    sem_post(user_session_sem);

    return 0;
}

int logout_session(char *username)
{
    if(user_session_sem == NULL) {
        user_session_sem = sem_open(SESSION_SEM_NAME, O_CREAT, 0600, 1);
    }
    sem_wait(user_session_sem);
    
    int flag = 0;
    for(int i=0; i<client_cnt; i++) {

        if(flag) {
            client[i-1] = client[i];
        }

        if(!strcmp(client[i]->name, username)) {
            printf("%d kill SIGTERM\n", client[i]->pid);
            
            if(kill(client[i]->pid, SIGTERM) < 0) {
                printf("%d not TERM send SIGKILL\n", client[i]->pid);
                kill(client[i]->pid, SIGKILL);
            }
            free(client[i]);
            client[i] = NULL;
            flag=1;
        }
    }  
    if(flag) {
        client_cnt--;
    }  
    sem_post(user_session_sem);
    return flag - 1;
}

int get_session_by_idx(struct User** data, int idx)
{
    if(user_session_sem == NULL) {
        user_session_sem = sem_open(SESSION_SEM_NAME, O_CREAT, 0600, 1);
    }
    sem_wait(user_session_sem);

    if(idx >= client_cnt) {
        sem_post(user_session_sem);
        return -1;
    }
    *data = client[idx];

    sem_post(user_session_sem);

    return 0;
}

int is_over_cnt(int idx)
{
    return (idx >= client_cnt);
}