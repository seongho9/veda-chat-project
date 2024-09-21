#include "client.h"

#include <sys/wait.h>
#include <signal.h>

void child_handler(int sig_no);

int main(int argc, char const *argv[])
{

    if(argc!=2){
        printf("%s <ip addr>\n", argv[0]);

        return 1;
    }
    struct sigaction sig_child, old_child;
    sigfillset(&sig_child.sa_mask);
    sigdelset(&sig_child.sa_mask, SIGCHLD);
    sig_child.sa_handler = child_handler;
    sig_child.sa_flags=SA_RESTART;

    sigaction(SIGCHLD, &sig_child, &old_child);
    
    init_page(argv[1]);

    return 0;
}

void child_handler(int sig_no)
{
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
}