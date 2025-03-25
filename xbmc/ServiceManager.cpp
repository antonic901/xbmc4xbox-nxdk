/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ServiceManager.h"

#include "ContextMenuManager.h"
#include "PlayListPlayer.h"
#include "addons/AddonManager.h"
#include "addons/RepositoryUpdater.h"
#include "cores/playercorefactory/PlayerCoreFactory.h"
#include "profiles/ProfileManager.h"
#include "storage/MediaManager.h"
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
  m_playlistPlayer.reset(new PLAYLIST::CPlayListPlayer());

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

  m_repositoryUpdater.reset(new ADDON::CRepositoryUpdater(*m_addonMgr));

  m_contextMenuManager.reset(new CContextMenuManager(*m_addonMgr));

  m_fileExtensionProvider.reset(new CFileExtensionProvider());

  m_mediaManager.reset(new CMediaManager());
  m_mediaManager->Initialize();

  init_level = 2;
  return true;
}

// stage 3 is called after successful initialization of WindowManager
bool CServiceManager::InitStageThree(const std::shared_ptr<CProfileManager>& profileManager)
{
  m_contextMenuManager->Init();

  m_playerCoreFactory.reset(new CPlayerCoreFactory(*profileManager));

  init_level = 3;
  return true;
}

void CServiceManager::DeinitStageThree()
{
  init_level = 2;
  m_playerCoreFactory.reset();
  m_contextMenuManager->Init();
}

void CServiceManager::DeinitStageTwo()
{
  init_level = 1;

  m_fileExtensionProvider.reset();
  m_contextMenuManager.reset();
  m_repositoryUpdater.reset();
  m_addonMgr.reset();

  m_mediaManager->Stop();
  m_mediaManager.reset();
}

void CServiceManager::DeinitStageOne()
{
  init_level = 0;

  m_playlistPlayer.reset();
}

ADDON::CAddonMgr& CServiceManager::GetAddonMgr()
{
  return *m_addonMgr;
}

ADDON::CRepositoryUpdater& CServiceManager::GetRepositoryUpdater()
{
  return *m_repositoryUpdater;
}

CContextMenuManager& CServiceManager::GetContextMenuManager()
{
  return *m_contextMenuManager;
}

PLAYLIST::CPlayListPlayer& CServiceManager::GetPlaylistPlayer()
{
  return *m_playlistPlayer;
}

CFileExtensionProvider& CServiceManager::GetFileExtensionProvider()
{
  return *m_fileExtensionProvider;
}

void CServiceManager::delete_contextMenuManager::operator()(CContextMenuManager* p) const
{
  delete p;
}

CPlayerCoreFactory& CServiceManager::GetPlayerCoreFactory()
{
  return *m_playerCoreFactory;
}

CMediaManager& CServiceManager::GetMediaManager()
{
  return *m_mediaManager;
}
