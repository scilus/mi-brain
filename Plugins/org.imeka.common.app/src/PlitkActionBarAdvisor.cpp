
#include "PlitkActionBarAdvisor.hpp"

#include <berryIActionBarConfigurer.h>

PlitkActionBarAdvisor::PlitkActionBarAdvisor(
  const berry::IActionBarConfigurer::Pointer& configurer)
  : berry::ActionBarAdvisor(configurer)
{}

void PlitkActionBarAdvisor::FillMenuBar(berry::IMenuManager*)
{}
