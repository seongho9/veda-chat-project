#include "chat_data.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <semaphore.h>

static sem_t *sem_msg = NULL;

int insert_message(struct Message* data, char* room_name, int lock)
{
    if(sem_msg == NULL){
        sem_msg = sem_open(MESSAGE_SEM_NAME, O_CREAT, 0600, 1);
    }

    if(lock){
        sem_wait(sem_msg);
    }

    //  메시지를 넣을 방을 검색
    struct Room* room_ptr;
    if(search_room(&room_ptr, room_name)!=0){
        sem_post(sem_msg);
        return -1;
    }

    //  방에 메시지가 없을 경우
    if(room_ptr->M_HEAD==NULL){
        room_ptr->M_HEAD = data;
        room_ptr->M_TAIL = data;
    }
    // 방에 메시지가 있는 경우
    else{
        room_ptr->M_TAIL->next = data;
        room_ptr->M_TAIL = data;
    }

    if(lock){
        sem_post(sem_msg);
    }

    return 0;
}

int get_message(struct Message** data, const char* room_name, const char* search_msg, int req_cnt)
{
    if(sem_msg == NULL){
        sem_msg = sem_open(MESSAGE_SEM_NAME, O_CREAT, 0600, 1);
    }
    sem_wait(sem_msg);

    //  메시지를 넣을 방을 검색
    struct Room* room_ptr;
    if(!search_room(&room_ptr, room_name)) {
        sem_post(sem_msg);
        return -1;
    }

    //  메시지 검색
    struct Message* m_ptr = room_ptr->M_HEAD;
    int cnt = 0;
    while(m_ptr!=NULL) {
        if(!strncmp(m_ptr->content, search_msg, strlen(search_msg))) {
            if(cnt==req_cnt){
                *data = m_ptr;
                break;
            }
            else{
                cnt++;
                m_ptr = m_ptr->next;
            }
        }
        else{
            m_ptr = m_ptr->next;
        }
    }

    if(m_ptr!=NULL){
        sem_post(sem_msg);
        return 0;
    }
    else{
        sem_post(sem_msg);
        return -1;
    }
}