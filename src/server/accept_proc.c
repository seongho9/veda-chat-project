#include"server.h"
#include"parser.h"
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 18080

int main_proc(int ssock, int csock, char* buf);

int accpet_proc()
{
    //  SIGINT
    struct sigaction sig_int;
    sigfillset(&sig_int.sa_mask);
    sigdelset(&sig_int.sa_mask, SIGINT);
    sig_int.sa_handler=server_int_signal_handler;
    sig_int.sa_flags=SA_RESTART;
    sigaction(SIGINT, &sig_int, NULL);

    //  SIGTERM
    struct sigaction sig_term;
    sigfillset(&sig_term.sa_mask);
    sigdelset(&sig_term.sa_mask, SIGTERM);
    sigdelset(&sig_term.sa_mask, SIGINT);
    sig_term.sa_handler=server_child_signal_handler;
    sig_term.sa_flags=SA_RESTART;
    sigaction(SIGTERM, &sig_term, NULL);

    //  SIGCHILD
    struct sigaction sig_child;
    sigfillset(&sig_child.sa_mask);
    sigdelset(&sig_child.sa_mask, SIGCHLD);
    sig_child.sa_handler = server_child_signal_handler;
    sig_child.sa_flags=SA_RESTART;
    sigaction(SIGCHLD, &sig_child, NULL);

    //  SIGIO
    struct sigaction sig_io;
    sigfillset(&sig_io.sa_mask);
    sigdelset(&sig_io.sa_mask, SIGIO);
    sig_io.sa_handler=server_io_handler;
    sig_io.sa_flags=SA_RESTART;
    sigaction(SIGIO, &sig_io, NULL);

    //  초기 소켓 생성
    int ssock;
    socklen_t clen;

    struct sockaddr_in server_addr, client_addr;
    char buf[BUFSIZ];

    if((ssock=socket(AF_INET, SOCK_STREAM, 0))<0) {
        perror("socket()");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    int yes = 1;
    if(setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
       perror("setsockopt");
    }

    if(bind(ssock, (struct sockaddr* )&server_addr, sizeof(server_addr))<0) {
        perror("bind()");
        return -1;
    }
    
    if(listen(ssock, 10)<0){
        perror("listen()");
        return -1;
    }
    printf("listen\n");
    clen = sizeof(client_addr);
    while(1) {
        char c_addr_p[BUFSIZ];
        int n, csock = accept(ssock, (struct sockaddr*)&client_addr, &clen);
        inet_ntop(AF_INET, &client_addr.sin_addr, c_addr_p, sizeof(c_addr_p));
        printf("accept %s\n", c_addr_p);
        while(1){
            if(read(csock, buf, BUFSIZ) > 0){
                if(!main_proc(ssock, csock, buf)){
                    csock=-1;
                    break;
                }
                else{
                    csock=-1;
                    break;
                }
            }
        }

    }
}

//  이 함수에서만 사용하는 것으로 로그인과 fork만을 수행
int main_proc(int ssock, int csock, char* buf)
{
    int pid;
    char command[COMM_LEN];
    char param[2][BUFSIZ];

    parse_command(buf, command, (char*)param);

    if(!strcmp(command, "login")){

        //  로그인 정보 확인
        struct UserData login_info;
        memset(login_info.name, 0, MAX_NAME_LEN);
        strcpy(login_info.name, param[0]);

        memset(login_info.password, 0, MAX_NAME_LEN);
        strcpy(login_info.password, param[1]);

        if(login_user(&login_info)){
            memset(buf, 0, BUFSIZ);
            strcpy(buf, "login info not matched");
            write(csock, buf, BUFSIZ);
            close(csock);
            return -1;
        }
        
        // 0 : read 1 : write
        int p_to_c[2], c_to_p[2];
        pipe(p_to_c);   pipe(c_to_p);

        pid = fork();
        //  error
        if(pid<0){

        }
        //  child
        else if(pid==0) {
            int w_fd = c_to_p[1];
            close(c_to_p[0]);
            int r_fd = p_to_c[0];
            close(p_to_c[1]);

            client_proc(csock, w_fd, r_fd);
        }
        //  parent
        else { 
            struct User* tmp = malloc(sizeof(struct User));

            tmp->w_fd = p_to_c[1];  tmp->r_fd = c_to_p[0];
            close(p_to_c[0]);   close(c_to_p[1]);
            strcpy(tmp->name, param[0]);

            tmp->pid = pid;
            
            //  client 프로세스에서 소켓을 통해 받아온 데이터를 비동기적으로 처리
            fcntl(tmp->r_fd, F_SETFL, O_ASYNC | O_NONBLOCK);
            fcntl(tmp->r_fd, F_SETOWN, getpid());

            if(login_session(tmp)){
                memset(buf, 0, BUFSIZ);
                sprintf(buf, "%d", -1);
                write(csock, buf, BUFSIZ);
            }
            else{
                memset(buf, 0, BUFSIZ);
                sprintf(buf, "%s", param[0]);
                //sprintf(buf, "0");
                write(csock, buf, BUFSIZ);
            }
            printf("login %s\n", param[0]);
        }
    }
    //  회원가임
    else if(!strcmp(command, "reg")){
        if(strlen(param[0]) < MAX_NAME_LEN && strlen(param[1]) < MAX_NAME_LEN){

            struct UserData* reg_user = (struct UserData*)malloc(sizeof(struct UserData));
            
            memset(reg_user->name, 0, MAX_NAME_LEN);
            strcpy(reg_user->name, param[0]);

            memset(reg_user->password, 0, MAX_NAME_LEN);
            strcpy(reg_user->password, param[1]);

            if(register_user(reg_user)) {
                memset(buf, 0, BUFSIZ);
                strcpy(buf, "user max");
                write(csock, buf, BUFSIZ);
            }
        }
        else {
            memset(buf, 0, BUFSIZ);
            strcpy(buf, "id or password length too long");
            write(csock, buf, BUFSIZ);
        }
        printf("register %s\n", param[0]);
        memset(buf, 0, BUFSIZ);
        sprintf(buf, "%d", 0);
        write(csock, buf, BUFSIZ);
        close(csock);
    }

    return 0;
}

