#ifndef _CHAT_DATA_H
#define _CHAT_DATA_H

#include <stdint.h>
#include <time.h>

#define MAX_ROOM 50
#define MAX_USER 50
#define MAX_NAME_LEN 20
#define MAX_MESSAGE_LEN 200
#define ROOM_SEM_NAME "sem_room"
#define MESSAGE_SEM_NAME "sem_msg"

//  메시지
struct Message
{
    //  해당 메시지를 보낸 유저
    char user[MAX_NAME_LEN];
    //  메시지를 보낸 시간
    time_t time;
    //  메시지가 속해있는 방의 이름
    char room_name[MAX_NAME_LEN];
    //  메시지 내용
    char content[MAX_MESSAGE_LEN];

    //  데이터를 찾기위한 연결리스트
    struct Message* next;
};
//  채팅방
struct Room
{
    //  채팅방 이름
    char name[MAX_NAME_LEN];
    //  해당 채팅방에 소속되어 있는 유저
    char user_list[MAX_USER][MAX_NAME_LEN];
    //  현 채팅방에 소속되어 있는 유저 수
    int user_cnt;
    //  메시지 id부여를 위한 메시지 총 개수
    uint64_t message_cnt;

    struct Message* M_HEAD;
    struct Message* M_TAIL;
};

/* User에 데이터를 추가 lock이 0이 아니면, semlock return -1:err, 0:ok */
int insert_user(char* data, int lock);
/* User 데이터를 제거 return -1:err, 0:ok*/
int remove_user(char* user_name);


/* 방의 정보를 인덱스로 가져옴, return -1:err, 0:ok */
int total_room(struct Room** data, int room_idx);
/* 방의 정보를 이름을 통해 가져옴, return -1:err, 0:ok */
int search_room(struct Room** data, char* room_name);
/* Room에 데이터 추가 lock이 0이 아니면, sem_lock*/
int insert_room(struct Room* data, int lock);
/* 방의 정보를 이름을 이용해 제거*/
int remove_room(char* room_name);
/* Room에 참가*/
int join_room(char* room_name, char* user_name);
/* Room에서 나오기 */
int exit_room(char* room_name, char* user_name);

/* 메시지를 추가*/
int insert_message(struct Message* data, char* room_name, int lock);
/* 메시지를 검색, 중복 키워드에 대비하여, 몇번째 입력인지 req_cnt로 전달*/
int get_message(char* data, const char* room_name, const char* search_msg, int req_cnt);
#endif