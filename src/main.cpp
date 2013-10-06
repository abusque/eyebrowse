#define DEFAULT_LOCKSCREEN_ACTIVATION true
#define DEFAULT_LOCKSCREEN_DELAY      2

#define DEFAULT_SCROLLDOWN_ACTIVATION true
#define DEFAULT_SCROLLDOWN_DELAY      2
#define DEFAULT_SCROLLDOWN_TICKS      8

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
  int  scrollDownTicks = DEFAULT_SCROLLDOWN_TICKS;
  time_t startValue = time(NULL),
         currentValue = time(NULL);

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
    scrollDown.add("ticks", Setting::TypeInt) = DEFAULT_SCROLLDOWN_TICKS;    

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
    scrollDown.lookupValue("ticks", scrollDownTicks); 
  }

  // Read screen resolution which will be used to compute mouse position
  initScreenResolution();

  while(true)
  {
    if(!tracker.update())
      break;

    if(lockActivated && tracker.isAbsent())
      lockScreen();

    currentValue = time(NULL);
    cout << "current " << currentValue << " start " << startValue << " = " << difftime(currentValue, startValue) << std::endl; 
    if(scrollDownActivated && difftime(currentValue, startValue) >= DEFAULT_SCROLLDOWN_DELAY && tracker.isWatchingBottom())
    {
      startValue = currentValue;
      scrollDown(scrollDownTicks);
    }
  }
    

  return 0;
}
