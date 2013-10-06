#include <iostream>

#include "uiActions.h"
#include "eyeTracker.h"

/**
 * @function main
 */
int main( int argc, const char** argv ) {
  
  EyeTracker tracker;

  while(true)
  {
    if(!tracker.update())
      break;

    if(tracker.isAbsent())
      lockScreen();
  }
    

  return 0;
}
