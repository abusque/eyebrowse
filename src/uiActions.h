#ifndef UI_ACTIONS_H
#define UI_ACTIONS_H

#define CINNAMON "cinnamon-screensaver-command -l"

#include <stdlib.h>
extern "C"{
  #include <xdo.h>
}

	int lockScreen();
	void scrollDown();
	void scrollUp();

#endif //UI_ACTIONS_H