#ifndef _CHAT_DATA_H
#define _CHAT_DATA_H

#include <stdint.h>
#include <time.h>
#include <stdio.h>

#define MAX_ROOM 50
#define MAX_USER 50
#define MAX_NAME_LEN 20
//  command, room_name을 뺀 값
#define MAX_MESSAGE_LEN (BUFSIZ - 21 - 5 - 1)

#define USER_SEM_NAME "sem_user"
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
//  유저
struct UserData
{
    char name[MAX_NAME_LEN];
    char password[MAX_NAME_LEN];
};

/// @brief 회원 가입
/// @param user 유저 정보
/// @return 최대 인원 초과시 -1 성공시 9
int register_user(struct UserData* user);
/// @brief 회원 탈퇴
/// @param user 유저 정보
/// @return 0 성공, -1 회원정보 불일치
int remove_user(struct UserData* user);
/// @brief 로그인
/// @param user 유저 정보
/// @return 0 성공, -1 회원정보 불일치
int login_user(struct UserData* user);

/// @brief 방 정보를 idx를 통해 가져옴
/// @param data 받아올 데이터 포인터의 주소값을 전달
/// @param room_idx 방의 idx
/// @return 0 성공, -1 인덱스 초과, -2 방 정보 없음
int total_room(struct Room** data, int room_idx);

/// @brief 방 정보를 방 이름을 통해 가져옴
/// @param data 받아올 데이터 포인터의 주소값을 전달
/// @param room_name 방의 이름
/// @return 0 성공, -1 방 정보 없음
int search_room(struct Room** data, char* room_name);

/// @brief 방 정보 추가
/// @param data 추가할 데이터
/// @param lock 데이터 추가시 lock을 할지 말지
/// @return 0 성공, 
int insert_room(struct Room* data, int lock);

/// @brief 방 제거
/// @param room_name 방 이름
/// @param lock 세마포어 락 여부 0:x, 1:ok
/// @return 0 성공, -1 실패
int remove_room(char* room_name, int lock);

/// @brief 방에 참가
/// @param room_name 참가하고자 하는 방 이름
/// @param user_name 요청 유저 이름
/// @return 0 성공 -1 실패
int join_room(char* room_name, char* user_name);

/// @brief 방에서 나오기
/// @param room_name 나오려는 방 이름
/// @param user_name 유저이름
/// @return 0 성공, -1 실패
int exit_room(char* room_name, char* user_name);

/// @brief 메시지를 채팅방에 추가
/// @param data 받아올 데이터
/// @param room_name 추가 할 방이름
/// @param lock 잠금 여부
/// @return 0이면 성공, 아니면 실패
int insert_message(struct Message* data, char* room_name, int lock);

/// @brief 메시지를 키워드 기반으로 가져옴
/// @param data 받아올 데이터
/// @param room_name 찾을 방 이름
/// @param search_msg 키워드
/// @param req_cnt 몇번째 키워드를 찾는지
/// @return 0이면 성공, 아니면 실패
int get_message(struct Message** data, const char* room_name, const char* search_msg, int req_cnt);
#endif