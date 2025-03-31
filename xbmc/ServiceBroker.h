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

namespace ADDON
{
class CAddonMgr;
class CRepositoryUpdater;
} // namespace ADDON

namespace ANNOUNCEMENT
{
class CAnnouncementManager;
}

namespace PLAYLIST
{
class CPlayListPlayer;
}

namespace KODI
{
namespace MESSAGING
{
class CApplicationMessenger;
}
} // namespace KODI

class CAppParams;
class CContextMenuManager;
class CDataCacheCore;
class CSettingsComponent;
class CFileExtensionProvider;
class CPlayerCoreFactory;
class CDatabaseManager;
class CGUIComponent;
class CMediaManager;
class CTextureCache;
class CJobManager;

class CServiceBroker
{
public:
  CServiceBroker();
  ~CServiceBroker();

  static std::shared_ptr<CAppParams> GetAppParams();
  static void RegisterAppParams(const std::shared_ptr<CAppParams>& appParams);
  static void UnregisterAppParams();

  static std::shared_ptr<ANNOUNCEMENT::CAnnouncementManager> GetAnnouncementManager();
  static void RegisterAnnouncementManager(
      std::shared_ptr<ANNOUNCEMENT::CAnnouncementManager> announcementManager);
  static void UnregisterAnnouncementManager();

  static ADDON::CAddonMgr& GetAddonMgr();
  static CContextMenuManager& GetContextMenuManager();
  static CDataCacheCore& GetDataCacheCore();
  static PLAYLIST::CPlayListPlayer& GetPlaylistPlayer();
  static ADDON::CRepositoryUpdater& GetRepositoryUpdater();
  static CFileExtensionProvider& GetFileExtensionProvider();
  static CPlayerCoreFactory& GetPlayerCoreFactory();
  static CDatabaseManager& GetDatabaseManager();
  static CMediaManager& GetMediaManager();

  static CGUIComponent* GetGUI();
  static void RegisterGUI(CGUIComponent* gui);
  static void UnregisterGUI();

  static void RegisterSettingsComponent(const std::shared_ptr<CSettingsComponent>& settings);
  static void UnregisterSettingsComponent();
  static std::shared_ptr<CSettingsComponent> GetSettingsComponent();

  static void RegisterTextureCache(const std::shared_ptr<CTextureCache>& cache);
  static void UnregisterTextureCache();
  static std::shared_ptr<CTextureCache> GetTextureCache();

  static void RegisterJobManager(const std::shared_ptr<CJobManager>& jobManager);
  static void UnregisterJobManager();
  static std::shared_ptr<CJobManager> GetJobManager();

  static void RegisterAppMessenger(
      const std::shared_ptr<KODI::MESSAGING::CApplicationMessenger>& appMessenger);
  static void UnregisterAppMessenger();
  static std::shared_ptr<KODI::MESSAGING::CApplicationMessenger> GetAppMessenger();

private:
  std::shared_ptr<CAppParams> m_appParams;
  std::shared_ptr<ANNOUNCEMENT::CAnnouncementManager> m_pAnnouncementManager;
  CGUIComponent* m_pGUI;
  std::shared_ptr<CSettingsComponent> m_pSettingsComponent;
  std::shared_ptr<CTextureCache> m_textureCache;
  std::shared_ptr<CJobManager> m_jobManager;
  std::shared_ptr<KODI::MESSAGING::CApplicationMessenger> m_appMessenger;
};

XBMC_GLOBAL_REF(CServiceBroker, g_serviceBroker);
#define g_serviceBroker XBMC_GLOBAL_USE(CServiceBroker)
