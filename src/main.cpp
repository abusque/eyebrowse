#define DEFAULT_LOCKSCREEN_ACTIVATION true
#define DEFAULT_LOCKSCREEN_DELAY      2

#define DEFAULT_SCROLLDOWN_ACTIVATION true
#define DEFAULT_SCROLLDOWN_DELAY      2
#define DEFAULT_SCROLLDOWN_TICKS      8

#define DEFAULT_HORIZ_FOCUS_ACTIVATED true
#define DEFAULT_HORIZ_FOCUS_DELAY     3

#include <iostream>
#include <stdlib.h>
#include <libconfig.h++>

#include "uiActions.h"
#include "eyeTracker.h"

using namespace std;
using namespace libconfig;

int main( int argc, const char** argv ) {

  static const std::string configFile = "config.cfg";
  EyeTracker tracker;
  bool lockActivated = DEFAULT_LOCKSCREEN_ACTIVATION;
  bool scrollDownActivated = DEFAULT_SCROLLDOWN_ACTIVATION;
  bool horizontalFocusActivated = DEFAULT_HORIZ_FOCUS_ACTIVATED;
  int  scrollDownDelay = DEFAULT_SCROLLDOWN_DELAY;
  int  scrollDownTicks = DEFAULT_SCROLLDOWN_TICKS;
  int horizontalFocusDelay = DEFAULT_HORIZ_FOCUS_DELAY;
  time_t scrollDownStartTime = time(NULL),
          horizontalFocusStartTime = time(NULL),
          currentTime = time(NULL);

  Config cfg;
  // Read the file. If there is an error, report it and exit.
  try
  {
    cfg.readFile(configFile.c_str());
  }
  catch(const FileIOException &fioex)
  {
    //File do not exist or error!
    //std::cerr << "I/O error while reading file." << std::endl;
    //return(EXIT_FAILURE);
  }
  catch(const ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
    return(EXIT_FAILURE);
  }

  // Find the 'lockscreen' setting. Add intermediate settings if they don't yet
  // exist.
  Setting &root = cfg.getRoot();
  if(!root.exists("lockScreen") || !root.exists("scrollDown"))
  {
    Setting &lockScreen = root.add("lockScreen", Setting::TypeGroup);
    lockScreen.add("activated", Setting::TypeBoolean) = DEFAULT_LOCKSCREEN_ACTIVATION;
    lockScreen.add("delay", Setting::TypeInt) = DEFAULT_LOCKSCREEN_DELAY;

    Setting &scrollDown = root.add("scrollDown", Setting::TypeGroup);
    scrollDown.add("activated", Setting::TypeBoolean) = DEFAULT_SCROLLDOWN_ACTIVATION;
    scrollDown.add("delay", Setting::TypeInt) = DEFAULT_SCROLLDOWN_DELAY;
    scrollDown.add("ticks", Setting::TypeInt) = DEFAULT_SCROLLDOWN_TICKS;

    Setting &horizFocus = root.add("horizFocus", Setting::TypeGroup);
    horizFocus.add("activated", Setting::TypeBoolean) = DEFAULT_HORIZ_FOCUS_ACTIVATED;
    horizFocus.add("delay", Setting::TypeInt) = DEFAULT_HORIZ_FOCUS_DELAY;

    // Write out the updated configuration.
    try
    {
      cfg.writeFile(configFile.c_str());
      cerr << "Updated configuration successfully written to: " << configFile
           << endl;

    }
    catch(const FileIOException &fioex)
    {
      cerr << "I/O error while writing file: " << configFile << endl;
      return(EXIT_FAILURE);
    }
  }
  else
  {

    Setting &lockScreen = root["lockScreen"];
    lockScreen.lookupValue("activated", lockActivated);

    int delay = DEFAULT_LOCKSCREEN_DELAY;
    lockScreen.lookupValue("delay", delay); 
    tracker.setDelay(delay);

    Setting &scrollDown = root["scrollDown"];
    scrollDown.lookupValue("activated", scrollDownActivated);
    scrollDown.lookupValue("delay", scrollDownDelay);
    scrollDown.lookupValue("ticks", scrollDownTicks);

    Setting &horizFocus = root["horizFocus"];
    horizFocus.lookupValue("activated", horizontalFocusActivated);
    horizFocus.lookupValue("delay", horizontalFocusDelay);
  }

  // Read screen resolution which will be used to compute mouse position
  initScreenResolution();

  while(true)
  {
    if(!tracker.update())
      break;

    if(lockActivated && tracker.isAbsent() && !tracker.isScreenLocked)
    {
      lockScreen();
      tracker.isScreenLocked = true;
    }

    currentTime = time(NULL);
    if(scrollDownActivated && difftime(currentTime, scrollDownStartTime) >= scrollDownDelay && tracker.isWatchingBottom())
    {
      scrollDownStartTime = currentTime;
      scrollDown(scrollDownTicks);
    }

    if(horizontalFocusActivated && difftime(currentTime, horizontalFocusStartTime) >= horizontalFocusDelay)
    {
      horizontalFocusStartTime = currentTime;

      if(tracker.isWatchingLeft())
        moveMouseLeftWindow();
      else if(tracker.isWatchingRight())
        moveMouseRightWindow();
    }
  }
    

  return 0;
}
