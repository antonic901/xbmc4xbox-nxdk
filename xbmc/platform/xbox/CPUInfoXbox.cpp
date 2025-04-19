/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "CPUInfoXbox.h"

std::shared_ptr<CCPUInfo> CCPUInfo::GetCPUInfo()
{
  return std::make_shared<CCPUInfoXbox>();
}

CCPUInfoXbox::CCPUInfoXbox()
{
  // TODO: query CPU information
}

CCPUInfoXbox::~CCPUInfoXbox()
{
}

int CCPUInfoXbox::GetUsedPercentage()
{
  // TODO: calculate CPU usage
  return 0;
}

float CCPUInfoXbox::GetCPUFrequency()
{
  // TODO: get cpu frequency
  return 733.0f;
}

bool CCPUInfoXbox::GetTemperature(CTemperature& temperature)
{
  // TODO: get cpu temperature
  temperature.SetValid(false);
  return false;
}
