#include "client.h"
#include <termios.h>
#include <unistd.h>

void enable_raw_mode()
{
    struct termios term;

    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void disable_raw_mode()
{
    struct termios term;

    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag |= (ICANON | ECHO);

    tcsetattr(STDIN_FILENO,TCSANOW, &term);
}