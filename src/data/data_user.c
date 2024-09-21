#include "chat_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

static sem_t *sem_user=NULL;
static struct UserData* user_list[MAX_USER];
static uint16_t user_cnt=0;

int register_user(struct UserData* user)
{
    if(sem_user == NULL){
        sem_user = sem_open(USER_SEM_NAME, O_CREAT, 0600, 1);
    }

    sem_wait(sem_user);

    if(user_cnt >= MAX_USER) {
        sem_post(sem_user);
        return -1;
    }

    user_list[user_cnt++] = user;

    sem_post(sem_user);

    return 0;
}

int remove_user(struct UserData* user)
{
    if(sem_user == NULL){
        sem_user = sem_open(USER_SEM_NAME, O_CREAT, 0600, 1);
    }

    sem_wait(sem_user);
    int i=0;
    for(i=0; i<user_cnt; i++) {
        //  유저이름 일치 확인
        if(!strcmp(user_list[i]->name, user->name)) {
            //  패스워드 불일치
            if(strcmp(user_list[i]->password, user->password)){
                sem_post(sem_user);
                return -1;
            }
            break;
        }
    }
    if(i == user_cnt){
        sem_post(sem_user);
        return -1;
    }
    free(user_list[i]);
    for(i; i<user_cnt-1; i++) {
        user_list[i] = user_list[i+1];
    }
    user_list[user_cnt-1] = NULL;
    user_cnt--;

    sem_post(sem_user);

    return 0;
}

int login_user(struct UserData* user)
{
    if(sem_user == NULL){
        sem_user = sem_open(USER_SEM_NAME, O_CREAT, 0600, 1);
    }

    sem_wait(sem_user);
    int i=0;
    for(i=0; i<user_cnt; i++) {
        //  유저이름 일치 확인
        if(!strcmp(user_list[i]->name, user->name)) {
            //  패스워드 불일치
            if(strcmp(user_list[i]->password, user->password)){
                sem_post(sem_user);
                return -1;
            }
            //  패스워드 일치
            else {
                sem_post(sem_user);
                return 0;
            }
        }
    }
    return -1;
}
