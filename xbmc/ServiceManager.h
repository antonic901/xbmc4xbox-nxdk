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
class CBinaryAddonCache;
class CServiceAddonManager;
class CRepositoryUpdater;
} // namespace ADDON

namespace PLAYLIST
{
class CPlayListPlayer;
}

class CContextMenuManager;
class CDataCacheCore;
class CFavouritesService;
class CWeatherManager;

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
  ADDON::CBinaryAddonCache& GetBinaryAddonCache();
  KODI::ADDONS::CExtsMimeSupportList& GetExtsMimeSupportList();
  ADDON::CServiceAddonManager& GetServiceAddons();
  ADDON::CRepositoryUpdater& GetRepositoryUpdater();
  CContextMenuManager& GetContextMenuManager();
  CDataCacheCore& GetDataCacheCore();

  PLAYLIST::CPlayListPlayer& GetPlaylistPlayer();
  int init_level = 0;

  CFavouritesService& GetFavouritesService();
  CFileExtensionProvider& GetFileExtensionProvider();

  CWeatherManager& GetWeatherManager();

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

  struct delete_favouritesService
  {
    void operator()(CFavouritesService* p) const;
  };

  std::unique_ptr<ADDON::CAddonMgr> m_addonMgr;
  std::unique_ptr<ADDON::CBinaryAddonCache> m_binaryAddonCache;
  std::unique_ptr<KODI::ADDONS::CExtsMimeSupportList> m_extsMimeSupportList;
  std::unique_ptr<ADDON::CServiceAddonManager> m_serviceAddons;
  std::unique_ptr<ADDON::CRepositoryUpdater> m_repositoryUpdater;
  std::unique_ptr<CContextMenuManager, delete_contextMenuManager> m_contextMenuManager;
  std::unique_ptr<CDataCacheCore, delete_dataCacheCore> m_dataCacheCore;
  std::unique_ptr<PLAYLIST::CPlayListPlayer> m_playlistPlayer;
  std::unique_ptr<CFavouritesService, delete_favouritesService> m_favouritesService;
  std::unique_ptr<CFileExtensionProvider> m_fileExtensionProvider;
  std::unique_ptr<CWeatherManager> m_weatherManager;
  std::unique_ptr<CPlayerCoreFactory> m_playerCoreFactory;
  std::unique_ptr<CDatabaseManager> m_databaseManager;
  std::unique_ptr<CMediaManager> m_mediaManager;
};
