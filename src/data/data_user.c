#include "chat_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

static sem_t *sem_user=NULL;
static char user_list[MAX_USER][MAX_NAME_LEN];
static uint16_t user_cnt=0;

int insert_user(char* data, int lock)
{
    if(sem_user==NULL){
        sem_user = sem_open("sem_user", O_CREAT, 0600, 1);
    }
    if(lock)
        sem_wait(sem_user);

    strcpy(user_list[user_cnt++], data);

    if(lock)
        sem_post(sem_user);

    return 0;
}

int remove_user(char* user_name)
{
    sem_wait(sem_user);
    int flag = 0;

    for(int i=0; i<user_cnt; i++) {

        if(flag){
            strcpy(user_list[i-1], user_list[i]);
            continue;
        }

        if(!strcmp(user_list[i], user_name)) {
            flag = 1;
        }

    }

    if(flag) {
        user_cnt--;
    }

    sem_post(sem_user);

    return flag - 1;
}
