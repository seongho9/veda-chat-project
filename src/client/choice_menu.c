#include "client.h"
#include <termio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include<arpa/inet.h>

int choice_menu();

void init_page(const char *addr)
{

    int w_fd, r_fd;
    int page;

    while(1){
        page = choice_menu();
        int ssock;
        if(page==0){
            char username[MAX_NAME_LEN];
            //  로그인 성공
            if((ssock=login_member(username, "127.0.0.1"))!=-1){
                // 0: read 1:write
                int c_to_p[2], p_to_c[2];
                pipe(c_to_p); pipe(p_to_c);

                pid_t pid = fork();
                //  에러
                if(pid < 0) {
                    fprintf(stderr, "%s %d", __FILE__, __LINE__);
                    return;
                }
                //  자식
                else if(pid==0) {
                    close(c_to_p[0]);   close(p_to_c[1]);
                    client_sock(ssock, c_to_p[1], p_to_c[0]);
                    exit(0);
                }
                //  부모
                else {
                    close(c_to_p[1]);   close(p_to_c[0]);

                    display_process(username, p_to_c[1], c_to_p[0]);    
                }
            }
            //  로그인 실패
            else{
                clear_console();
                printf("\nlogin failed\n");
            }
        }
        else if(page==1){
            //  회원가입 성공
            if(!register_member("127.0.0.1")){

            }
            //  회원가입 실패
            else {
                clear_console();
                printf("\nregister failed\n");
            }
        }
        else if(page) {
            break;
        }
    }
    

}
//  매뉴 선택을 위한 함수로 해당 메뉴의 숫자를 return
int choice_menu()
{
    struct termio cur_term, old_term;
    int input = -1;
    int pos = 0;
    while(1){
        clear_console();
        printf("\nchatting client\n");
        if (pos==0) {
            printf("> Login\n");
            printf("  Register\n");
            printf("  Quit\n");
        }
        else if(pos==1) {
            printf("  Login\n");
            printf("> Register\n");
            printf("  Quit\n");
        }
        else if(pos==2) {
            printf("  Login\n");
            printf("  Register\n");
            printf("> Quit\n");            
        }


        input = getch();
        //  escape character
        if(input==27){
            input=getch();
            //  [ character
            if(input==91){
                input=getch();
                // break;
            }
        }
        //  enter
        else if(input==10) {
            clear_console();
            break;
        }
        switch (input)
        {
        // up
        case 65:
            pos--;
            break;
        // down
        case 66:
            pos++;
            break;
        
        default:
            break;
        }

        pos = (pos > 2) ? 2 : pos;
        pos = (pos < 0) ? 0 : pos;
    }

    return pos;
}

int getch(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void clear_console()
{
    printf("\033[H\033[J");
    fflush(stdout);
}