/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ServiceBroker.h"

#include "ServiceManager.h"
#include "Application.h"
#include "settings/SettingsComponent.h"
#include "utils/log.h"

#include <stdexcept>
#include <utility>

CServiceBroker::CServiceBroker()
{
}

CServiceBroker::~CServiceBroker()
{
}

std::shared_ptr<CAppParams> CServiceBroker::GetAppParams()
{
  if (!g_serviceBroker.m_appParams)
    throw std::logic_error("AppParams not yet available / not available anymore.");

  return g_serviceBroker.m_appParams;
}

void CServiceBroker::RegisterAppParams(const std::shared_ptr<CAppParams>& appParams)
{
  g_serviceBroker.m_appParams = appParams;
}

void CServiceBroker::UnregisterAppParams()
{
  g_serviceBroker.m_appParams.reset();
}

ADDON::CAddonMgr& CServiceBroker::GetAddonMgr()
{
  return g_application.m_ServiceManager->GetAddonMgr();
}

void CServiceBroker::RegisterSettingsComponent(const std::shared_ptr<CSettingsComponent>& settings)
{
  g_serviceBroker.m_pSettingsComponent = settings;
}

void CServiceBroker::UnregisterSettingsComponent()
{
  g_serviceBroker.m_pSettingsComponent.reset();
}

std::shared_ptr<CSettingsComponent> CServiceBroker::GetSettingsComponent()
{
  return g_serviceBroker.m_pSettingsComponent;
}

CFileExtensionProvider& CServiceBroker::GetFileExtensionProvider()
{
  return g_application.m_ServiceManager->GetFileExtensionProvider();
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
