#include "uiActions.h"

xdo_t* xdo = xdo_new(NULL);

int lockScreen()
{
	return system("cinnamon-screensaver-command -l");
}

void scrollUp()
{
	xdo_click(xdo, CURRENTWINDOW, 4);
}

void scrollDown()
{
	xdo_click(xdo, CURRENTWINDOW, 5);
}