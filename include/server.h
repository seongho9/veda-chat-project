#ifndef _SERVER_H
#define _SERVER_H

#define SESSION_SEM_NAME "user_session_sem"
#include "chat_data.h"
#define PORT 18080

struct User
{
    int pid;
    char name[MAX_NAME_LEN];
    int r_fd, w_fd;
};

/// @brief 소켓 프로세스가 read pipe에 데이터를 넣은 경우 발생
/// @param sig_no 
void server_io_handler(int sig_no);
/// @brief 종료를 위해 Ctrl+c 인터럽트를 발생
/// @param sig_no 
void server_int_signal_handler(int sig_no);
/// @brief 자식 프로세스가 끝낼 때, 처리를 위함
/// @param sig_no 
void server_child_signal_handler(int sig_no);

/// @brief 접속하는 유저마다 생성하는 프로세스가 수행하는 함수
/// @param csock 소켓 디스크립터 번호
/// @param w_fd 쓰기 파이프
/// @param r_fd 읽기 파이프
/// @return 
int client_proc(int csock, int w_fd, int r_fd);

/// @brief 메인 프로세스가 수행하는 함수
/// @return 
int accpet_proc();

//  이 아래는 세션 관리를 위한 함수

/// @brief 로그인시 세션을 등록
/// @param data 로그인한 유저의 정보
/// @return 0 성공 1 실패
int login_session(struct User* data);
/// @brief 세션 로그아웃
/// @param username 로그아웃을 위한 유저 이름
/// @return 0 성공 1 실패
int logout_session(char *username);
/// @brief 세션 정보를 가져오는 함수
/// @param data 세션 정보를 받아오기 위한 반환 포인터
/// @param idx  세션 배열의 idx
int get_session_by_idx(struct User** data, int idx);
/// @brief 해당 idx가 세션 인원을 초과하는지 확인
/// @param idx 인덱스
/// @return 0: flase, 1:true
int is_over_cnt(int idx);

#endif