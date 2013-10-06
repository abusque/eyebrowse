#include "uiActions.h"

xdo_t* xdo = xdo_new(NULL);

int lockScreen()
{
	return system(CINNAMON);
}

void scrollUp()
{
	xdo_click(xdo, CURRENTWINDOW, 4);
}

void scrollDown(int scrollTicks)
{
	for(int i = 0; i < scrollTicks; ++i)
		xdo_click(xdo, CURRENTWINDOW, 5);
}