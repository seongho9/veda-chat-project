#include"server.h"

#define PORT 18080

int w_fd, r_fd;
struct session_info
{
    int w_fd, r_fd;
    int csock;
};
void int_handler(int sig_no);

void child_handler(int sig_no);

void session_handler(void *param);

int accpet_proc()
{
    struct sigaction sig_int, old_int;

    sig_int.sa_handler=int_handler;
    sig_int.sa_flags=SA_RESTART;

    sigaction(SIGINT, &sig_int, &old_int);

    struct sigaction sig_child, old_child;

    sig_child.sa_handler = child_handler;
    sig_child.sa_flags=SA_RESTART;

    sigaction(SIGCHLD, &sig_child, &old_child);


    int ssock;
    socklen_t clen;

    struct sockaddr_in server_addr, client_addr;
    char msg[BUFSIZ];

    if((ssock=socket(AF_INET, SOCK_STREAM, 0)<0)) {
        perror("socket()");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if(bind(ssock, (struct sockaddr* )&server_addr, sizeof(server_addr))<0) {
        perror("bind()");
        return -1;
    }

    if(listen(ssock, 10)<0){
        perror("listen()");
        return -1;
    }

    int pid = fork();

    // 0 : read 1 : write
    int p_to_c[2];
    int c_to_p[2];

    pipe(p_to_c);
    pipe(c_to_p);

    if(pid<0){

    }
    else if(pid==0){
        main_proc(p_to_c[0], c_to_p[1]);
    }
    else{
        r_fd = c_to_p[0];
        w_fd = p_to_c[1];
    }

    clen = sizeof(client_addr);
    while(1) {
        int n, csock = accept(ssock, (struct sockaddr*)&client_addr, &clen);
        // 세션 연결과 로그인 사이에 공백?
        pthread_t tid;
        pthread_create(&tid, NULL, session_handler, csock);
        pthread_detach(tid);
    }
}

void int_handler(int sig_no)
{


    exit(0);
}

void child_handler(int sig_no)
{
    waitpid(0, NULL, WNOHANG);
}

void session_handler(void* param)
{
    struct session_info info = *(struct session_info*)param;
    char msg[BUFSIZ];
    char buf[BUFSIZ];
    while(1){
        if(read(info.csock, &buf, BUFSIZ) > 0){
            sprintf(msg, "%s|%d", msg, info.csock);
            write(w_fd, msg, BUFSIZ);
            
        }
    }
}
