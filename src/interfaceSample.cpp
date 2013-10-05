#include <stdio.h>

#include "uiActions.h"

int main(void) 
{

    for(int i = 0; i < 5; i++)
    {
        scrollDown();
        sleep(1);
    }

    for(int i = 0; i < 5; i++)
    {
        scrollUp();
        sleep(1);
    }

    sleep(1);
    lockScreen();

    exit(EXIT_SUCCESS);
}

