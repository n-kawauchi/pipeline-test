// -*- C++ -*-
/*!
 * @file StringOutComp.h
 * @brief Sample string out component
 * @date $Date: 2005-05-12 09:06:20 $
 *
 * $Id$
 */



#include <rtm/RtcManager.h>
#include <iostream>
#include <string>
#include "StringOut.h"

#if defined(RTM_OS_VXWORKS) && not defined(__RTP__)
int stringout_main()
{
  RTC::Manager* manager = &RTC::Manager::instance();
  RTC::RtcBase* comp;
  StringOutInit(manager);

  std::string name;
  comp = manager->createComponent("StringOut", "Generic", name);
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
  StringOutInit(manager);

  std::string name;
  RtcBase* comp;
  comp = manager->createComponent("StringOut", "Generic", name);

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
