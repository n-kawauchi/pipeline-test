// -*- C++ -*-
/*!
 * @file StringInComp.h
 * @brief Sample string in component
 * @date $Date: 2005-05-12 09:06:20 $
 *
 * $Id$
 */



#include <rtm/RtcManager.h>
#include <iostream>
#include <string>
#include "StringIn.h"



#if defined(RTM_OS_VXWORKS) && not defined(__RTP__)
int stringin_main()
{
  RTC::Manager* manager = &RTC::Manager::instance();
  RTC::RtcBase* comp;
  StringInInit(manager);
  std::string name;

  comp = manager->createComponent("StringIn", "Generic", name);
  if(comp)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}
#else

void MyModuleInit(RtcManager* manager)
{
  StringInInit(manager);

  std::string name;
  RtcBase* comp;
  comp = manager->createComponent("StringIn", "Generic", name);

  std::cout << "RTComponent: " << name << " was created." << std::endl;

  return;
}


int main (int argc, char** argv)
{
  RTM::RtcManager manager(argc, argv);
  // Initialize manager
  manager.initManager();
  // Activate manager and register to naming service
  manager.activateManager();
  // Initialize my module on this maneger
  manager.initModuleProc(MyModuleInit);
  // Main loop
  manager.runManager();
  return 0;
}


#endif
