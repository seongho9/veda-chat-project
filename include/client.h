#ifndef _CLIENT_H
#define _CLIENT_H
#include "chat_data.h"
#define PORT 18080

struct Msg 
{
    char content[MAX_NAME_LEN];
    char sender[MAX_NAME_LEN];
    time_t time;
    
};

void enable_raw_mode();
void disable_raw_mode();

int getch(void);

/// @brief 콘솔화면을 초기화
void clear_console();

/// @brief 초기 선택 페이지
/// @param addr 서버 주소
void init_page(const char *addr);

/// @brief 로그인 후 시작페이지
/// @param username 유저 이름
/// @param w_fd 서버로 보내는 디스크립터
/// @param r_fd 서버에서 받아오는 디스크립터
void display_process(char* username, int w_fd, int r_fd);

/// @brief 방 만드는 화면
/// @param w_fd 서버로 보내는 디스크립터
/// @param r_fd 서버에서 받아오는 디스크립터
int display_makeroom(int w_fd, int r_fd);

/// @brief 방에 참가
/// @param username 받아 온 사용자 이름
/// @param w_fd 서버로 보내는 디스크립터
/// @param r_fd 서버에서 받아오는 디스크립터
/// @return 0: 성공 -1: 잘못된 이름 -2: 인원 초과, -3: 별도 오류
int dispaly_joinroom(char* username, char* roomname, int w_fd, int r_fd);

/// @brief 채팅방 인터페이스 함수
/// @param userlist 현재 참가중인 유저
/// @param w_fd 서버로 보내는 디스크립터
/// @param r_fd 서버에서 받아오는 디스크립터
/// @return 
int dispaly_chatroom(char* userlist, char* roomname, int w_fd, int r_fd);

/// @brief 클라이언트에서 전송을 담당하는 프로세스
/// @param sock 소켓 디스크립터 번호
/// @param w_fd 쓰기 파이프 
/// @param r_fd 읽기 파이프
void client_sock(int sock, int w_fd, int r_fd);

/// @brief 회원가입
/// @param addr 서버 주소
/// @return 0: 성공, others: 실패
int register_member(const char *addr);

/// @brief 로그인
/// @param addr 서버 주소
/// @return -1: 실패 others: 소켓 디스크립터 번호
int login_member(char* username, const char* addr);

/// @brief addr로 연결 시도
/// @param addr 서버 주소
/// @return 소켓 디스크립터 번호
int connect_server(const char* addr);

/// @brief 유저 정보를 입력 받음
/// @param param BUFSIZE크기의 배열을 2개 갖는 이차원 배열
/// @return 0: 성공 -1: 실패
int get_userinfo(char* param);


#endif