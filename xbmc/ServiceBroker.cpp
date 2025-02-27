/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ServiceBroker.h"

#include "utils/log.h"

#include <stdexcept>
#include <utility>

CServiceBroker::CServiceBroker()
{
}

CServiceBroker::~CServiceBroker()
{
}

void CServiceBroker::RegisterJobManager(const std::shared_ptr<CJobManager>& jobManager)
{
  g_serviceBroker.m_jobManager = jobManager;
}

void CServiceBroker::UnregisterJobManager()
{
  g_serviceBroker.m_jobManager.reset();
}

std::shared_ptr<CJobManager> CServiceBroker::GetJobManager()
{
  return g_serviceBroker.m_jobManager;
}

void CServiceBroker::RegisterAppMessenger(
    const std::shared_ptr<KODI::MESSAGING::CApplicationMessenger>& appMessenger)
{
  g_serviceBroker.m_appMessenger = appMessenger;
}

void CServiceBroker::UnregisterAppMessenger()
{
  g_serviceBroker.m_appMessenger.reset();
}

std::shared_ptr<KODI::MESSAGING::CApplicationMessenger> CServiceBroker::GetAppMessenger()
{
  return g_serviceBroker.m_appMessenger;
}
