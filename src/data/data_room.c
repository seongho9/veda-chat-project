#include "chat_data.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <semaphore.h>


static struct Room* room_list[MAX_ROOM] = { NULL, };
static uint32_t room_cnt = 0;
static sem_t *sem_room=NULL;

int total_room(struct Room** data, int room_idx)
{
    if(sem_room==NULL){
        sem_room = sem_open(ROOM_SEM_NAME, O_CREAT, 0600, 1);
    }
    
    sem_wait(sem_room);

    if(room_idx > room_cnt) {
        sem_post(sem_room);
        return -1;
    }
    *data = room_list[room_idx];
    if(*data == NULL) {
        sem_post(sem_room);
        return -1;
    }
    sem_post(sem_room);
    return 0;
}

int search_room(struct Room** data, char* room_name)
{
    sem_wait(sem_room);

    for(int i=0; i<room_cnt; i++) {
        if(!strcmp(room_list[i]->name, room_name)){
            *data = room_list[i];
            
            sem_post(sem_room);
            return 0;
        }
    }
    
    sem_post(sem_room);

    return -1;
}

int insert_room(struct Room* data, int lock)
{
    if(sem_room==NULL){
        sem_room = sem_open(ROOM_SEM_NAME, O_CREAT, 0600, 1);
    }

    if(lock)
        sem_wait(sem_room);

    room_list[room_cnt++] = data;

    if(lock)
        sem_post(sem_room);

    return 0;
}
int remove_room(char* room_name)
{
    sem_wait(sem_room);
    int flag = 0;

    for(int i=0; i<room_cnt; i++){
        if(flag) {
            room_list[i-1] = room_list[i];
        }
        if(!strcmp(room_list[i]->name, room_name)){
            flag=1;
            struct Message* ptr = room_list[i]->M_HEAD;
            struct Message* next;
            while(ptr!=NULL) {
                next = ptr->next;
                free(ptr);
                ptr = next;
            }
            free(room_list[i]);
            room_list[i] = NULL;
        }
    }

    if(flag){
        room_cnt--;
    }
    sem_post(sem_room);

    return flag-1;
}
int join_room(char* room_name, char* user_name)
{
    sem_wait(sem_room);
    struct Room* ptr;

    for(int i=0; i<room_cnt; i++){
        ptr = room_list[i];

        if(!strcmp(ptr->name, room_name)){

            strcpy(ptr->user_list[ptr->user_cnt], user_name);
            ptr->user_cnt++;

            sem_post(sem_room);
            return 0;
        }
    }
    sem_post(sem_room);
    return -1;
}

int exit_room(char* room_name, char* user_name)
{
    sem_wait(sem_room);
    struct Room* ptr;
    int flag = 0;

    for(int i=0; i<room_cnt; i++){
        ptr = room_list[i];
        if(!strcmp(ptr->name, room_name)){
            break;
        }
    }

    for(int i=0; i<ptr->user_cnt; i++){
        
        if(flag){
            strcpy(ptr->user_list[i-1], ptr->user_list[i]);
            continue;
        }

        if(!strcmp(ptr->user_list[i], user_name)){
            flag = 1;
        }
    }
    if(flag){
        ptr->user_cnt--;
    }
    if(!ptr->user_cnt){
        remove_room(room_name);
    }
    sem_post(sem_room);

    return flag-1;
}
