/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "utils/GlobalsHandling.h"

#include <memory>

class CJobManager;

class CServiceBroker
{
public:
  CServiceBroker();
  ~CServiceBroker();

  static void RegisterJobManager(const std::shared_ptr<CJobManager>& jobManager);
  static void UnregisterJobManager();
  static std::shared_ptr<CJobManager> GetJobManager();

private:
  std::shared_ptr<CJobManager> m_jobManager;
};

XBMC_GLOBAL_REF(CServiceBroker, g_serviceBroker);
#define g_serviceBroker XBMC_GLOBAL_USE(CServiceBroker)
