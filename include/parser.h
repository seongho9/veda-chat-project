#ifndef _PARSER_H
#define _PARSER_H

#define DELIM_STR ":"
#define DELIM_CHR ':'
#define COMM_LEN 10

#include <stdio.h>

/// @brief 소켓으로 받은 문자열 데이터를 파싱
/// @param msg 문자열 데이터
/// @param command 명령어 종류를 받을 문자열, 메모리 할당 후 사용할 것
/// @param param 각종 데이터를 받을 2차원 배열로 문자열의 크기는 stdio.h의 BUFSIZ, 2차원 배열의 시작 주소 값을 넘겨줄것
/// @return 성공시 param의 수, 실패시 -1
int parse_command(char* msg, char* command, char* param);

/// @brief 데이터를 소켓으로 보내기 위한 문자열화
/// @param msg 소켓으로 보낼 문자열
/// @param command 명령어
/// @param param 각종 매개변수를 넘겨줄 2차원 배열 문자열의 크기는 stdio.h의 BUFSIZ, 2차원 배열의 시작 주소 값을 넘겨줄 것
/// @param param_cnt 매개변수 수
/// @return 성공 0, 실패 NOT 0
int tostring_command(char* msg, char* command, char* param, int param_cnt);

#endif