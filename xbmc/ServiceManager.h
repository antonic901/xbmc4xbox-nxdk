/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <memory>

namespace ADDON
{
class CAddonMgr;
class CRepositoryUpdater;
} // namespace ADDON

namespace PLAYLIST
{
class CPlayListPlayer;
}

class CContextMenuManager;
class CDataCacheCore;

namespace KODI
{
namespace ADDONS
{
class CExtsMimeSupportList;
}
} // namespace KODI

class CFileExtensionProvider;
class CPlayerCoreFactory;
class CDatabaseManager;
class CProfileManager;
class CMediaManager;

class CServiceManager
{
public:
  CServiceManager();
  ~CServiceManager();

  bool InitForTesting();
  bool InitStageOne();
  bool InitStageTwo(const std::string& profilesUserDataFolder);
  bool InitStageThree(const std::shared_ptr<CProfileManager>& profileManager);
  void DeinitTesting();
  void DeinitStageThree();
  void DeinitStageTwo();
  void DeinitStageOne();

  ADDON::CAddonMgr& GetAddonMgr();
  KODI::ADDONS::CExtsMimeSupportList& GetExtsMimeSupportList();
  ADDON::CRepositoryUpdater& GetRepositoryUpdater();
  CContextMenuManager& GetContextMenuManager();
  CDataCacheCore& GetDataCacheCore();

  PLAYLIST::CPlayListPlayer& GetPlaylistPlayer();
  int init_level = 0;

  CFileExtensionProvider& GetFileExtensionProvider();

  CPlayerCoreFactory& GetPlayerCoreFactory();

  CDatabaseManager& GetDatabaseManager();

  CMediaManager& GetMediaManager();

protected:
  struct delete_dataCacheCore
  {
    void operator()(CDataCacheCore* p) const;
  };

  struct delete_contextMenuManager
  {
    void operator()(CContextMenuManager* p) const;
  };

  std::unique_ptr<ADDON::CAddonMgr> m_addonMgr;
  std::unique_ptr<KODI::ADDONS::CExtsMimeSupportList> m_extsMimeSupportList;
  std::unique_ptr<ADDON::CRepositoryUpdater> m_repositoryUpdater;
  std::unique_ptr<CContextMenuManager, delete_contextMenuManager> m_contextMenuManager;
  std::unique_ptr<CDataCacheCore, delete_dataCacheCore> m_dataCacheCore;
  std::unique_ptr<PLAYLIST::CPlayListPlayer> m_playlistPlayer;
  std::unique_ptr<CFileExtensionProvider> m_fileExtensionProvider;
  std::unique_ptr<CPlayerCoreFactory> m_playerCoreFactory;
  std::unique_ptr<CDatabaseManager> m_databaseManager;
  std::unique_ptr<CMediaManager> m_mediaManager;
};
