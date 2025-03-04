/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ServiceManager.h"

#include "addons/AddonManager.h"
#include "utils/FileExtensionProvider.h"
#include "utils/log.h"

CServiceManager::CServiceManager() = default;

CServiceManager::~CServiceManager()
{
  if (init_level > 2)
    DeinitStageThree();
  if (init_level > 1)
    DeinitStageTwo();
  if (init_level > 0)
    DeinitStageOne();
}

bool CServiceManager::InitForTesting()
{
  m_addonMgr.reset(new ADDON::CAddonMgr());
  if (!m_addonMgr->Init())
  {
    CLog::Log(LOGFATAL, "CServiceManager::{}: Unable to start CAddonMgr", __FUNCTION__);
    return false;
  }

  m_fileExtensionProvider.reset(new CFileExtensionProvider());

  init_level = 1;
  return true;
}

void CServiceManager::DeinitTesting()
{
  init_level = 0;
  m_fileExtensionProvider.reset();
  m_addonMgr.reset();
}

bool CServiceManager::InitStageOne()
{
  init_level = 1;
  return true;
}

bool CServiceManager::InitStageTwo(const std::string& profilesUserDataFolder)
{
  m_addonMgr.reset(new ADDON::CAddonMgr());
  if (!m_addonMgr->Init())
  {
    CLog::Log(LOGFATAL, "CServiceManager::{}: Unable to start CAddonMgr", __FUNCTION__);
    return false;
  }

  m_fileExtensionProvider.reset(new CFileExtensionProvider());

  init_level = 2;
  return true;
}

// stage 3 is called after successful initialization of WindowManager
bool CServiceManager::InitStageThree()
{
  init_level = 3;
  return true;
}

void CServiceManager::DeinitStageThree()
{
  init_level = 2;
}

void CServiceManager::DeinitStageTwo()
{
  init_level = 1;

  m_fileExtensionProvider.reset();
  m_addonMgr.reset();
}

void CServiceManager::DeinitStageOne()
{
  init_level = 0;
}

ADDON::CAddonMgr& CServiceManager::GetAddonMgr()
{
  return *m_addonMgr;
}

CFileExtensionProvider& CServiceManager::GetFileExtensionProvider()
{
  return *m_fileExtensionProvider;
}
