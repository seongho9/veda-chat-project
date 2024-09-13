#include "server.h"
#include <semaphore.h>

int main(int argc, char const *argv[])
{

    sem_unlink(ROOM_SEM_NAME);
    sem_unlink(MESSAGE_SEM_NAME);

    accpet_proc();

    return 0;
}
