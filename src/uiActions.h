#ifndef UI_ACTIONS_H
#define UI_ACTIONS_H

#define CINNAMON "cinnamon-screensaver-command -l"

#include <stdlib.h>
extern "C"{
#include <xdo.h>
}

int lockScreen();
void scrollDown(int scrollTicks = 8);
void scrollUp();
void moveMouseLeftWindow();
void moveMouseRightWindow();
void initScreenResolution();

#endif //UI_ACTIONS_H
