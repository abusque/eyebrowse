#include "uiActions.h"

xdo_t* xdo = xdo_new(NULL);
unsigned int screenCenterX = 0;
unsigned int screenCenterY = 0;
unsigned int screenWidth = 0;
unsigned int screenHeight = 0;

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

void moveMouseLeftWindow()
{
    xdo_mousemove(xdo, screenCenterX - (screenWidth / 4), screenCenterY, 0);
}

void moveMouseRightWindow()
{
    xdo_mousemove(xdo, screenCenterX + (screenWidth / 4), screenCenterY, 0);
}

void initScreenResolution()
{
    xdo_get_viewport_dimensions(xdo, &screenWidth, &screenHeight, 0);
    screenCenterX = screenWidth / 2;
    screenCenterY = screenHeight / 2;
}
