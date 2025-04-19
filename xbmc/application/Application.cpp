/*
 *      Copyright (C) 2005-2015 Team XBMC
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "Application.h"

#include "PlayListPlayer.h"
#include "ServiceManager.h"
#include "URL.h"
#include "Util.h"
#include "addons/AddonManager.h"
#include "addons/RepositoryUpdater.h"
#include "addons/Service.h"
#include "addons/Skin.h"
#include "application/ApplicationActionListeners.h"
#include "application/ApplicationPlayer.h"
#include "application/ApplicationPowerHandling.h"
#include "application/ApplicationSkinHandling.h"
#include "application/ApplicationStackHelper.h"
#include "guilib/GUIComponent.h"
#include "guilib/GUIFontManager.h"
#include "playlists/PlayListFactory.h"

#include "GUIPassword.h"
#include "ServiceBroker.h"
#include "TextureCache.h"
#include "filesystem/Directory.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/LocalizeStrings.h"
#include "input/KeyboardLayoutManager.h"
#include "input/Key.h"
#include "messaging/ApplicationMessenger.h"
#include "messaging/ThreadMessage.h"

#include "playlists/PlayList.h"
#include "playlists/SmartPlayList.h"
#include "profiles/ProfileManager.h"
#include "settings/AdvancedSettings.h"
#include "settings/DisplaySettings.h"
#include "settings/MediaSettings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "utils/CPUInfo.h"
#include "utils/SystemInfo.h"
#include "utils/Splash.h"
#include "utils/log.h"

#include "interfaces/AnnouncementManager.h"

// Windows includes
#include "guilib/GUIWindowManager.h"
#include "video/PlayerController.h"

#include "DatabaseManager.h"
#include "storage/MediaManager.h"

#include "addons/AddonSystemSettings.h"

#include <mutex>

using namespace ADDON;
using namespace XFILE;
using namespace KODI::MESSAGING;

using namespace std::chrono_literals;

CApplication::CApplication(void)
  : m_pPlayer(new CApplicationPlayer)
{
  // register application components
  RegisterComponent(std::make_shared<CApplicationActionListeners>(m_critSection));
  RegisterComponent(std::make_shared<CApplicationPlayer>());
  RegisterComponent(std::make_shared<CApplicationPowerHandling>());
  RegisterComponent(std::make_shared<CApplicationSkinHandling>(this, this, m_bInitializing));
  RegisterComponent(std::make_shared<CApplicationStackHelper>());
}

CApplication::~CApplication(void)
{
  DeregisterComponent(typeid(CApplicationStackHelper));
  DeregisterComponent(typeid(CApplicationSkinHandling));
  DeregisterComponent(typeid(CApplicationPowerHandling));
  DeregisterComponent(typeid(CApplicationPlayer));
  DeregisterComponent(typeid(CApplicationActionListeners));
}

bool CApplication::Create()
{
  m_bStop = false;

  RegisterSettings();

  CServiceBroker::RegisterCPUInfo(CCPUInfo::GetCPUInfo());

  // Register JobManager service
  CServiceBroker::RegisterJobManager(std::make_shared<CJobManager>());

  // Announcement service
  m_pAnnouncementManager = std::make_shared<ANNOUNCEMENT::CAnnouncementManager>();
  m_pAnnouncementManager->Start();
  CServiceBroker::RegisterAnnouncementManager(m_pAnnouncementManager);

  const auto appMessenger = std::make_shared<CApplicationMessenger>();
  CServiceBroker::RegisterAppMessenger(appMessenger);

  const auto keyboardLayoutManager = std::make_shared<CKeyboardLayoutManager>();
  CServiceBroker::RegisterKeyboardLayoutManager(keyboardLayoutManager);

  m_ServiceManager.reset(new CServiceManager());

  if (!m_ServiceManager->InitStageOne())
  {
    return false;
  }

  // here we register all global classes for the CApplicationMessenger,
  // after that we can send messages to the corresponding modules
  appMessenger->RegisterReceiver(this);
  appMessenger->RegisterReceiver(&CServiceBroker::GetPlaylistPlayer());
  appMessenger->SetGUIThread(CThread::GetCurrentThreadId());
  appMessenger->SetProcessThread(CThread::GetCurrentThreadId());

  // copy required files
  CUtil::CopyUserDataIfNeeded("special://masterprofile/", "RssFeeds.xml");
  CUtil::CopyUserDataIfNeeded("special://masterprofile/", "favourites.xml");
  CUtil::CopyUserDataIfNeeded("special://masterprofile/", "Lircmap.xml");

  CLog::Init(CSpecialProtocol::TranslatePath("special://logpath"));

#ifdef TARGET_POSIX //! @todo Win32 has no special://home/ mapping by default, so we
  //!       must create these here. Ideally this should be using special://home/ and
  //!      be platform agnostic (i.e. unify the InitDirectories*() functions)
  if (!CServiceBroker::GetAppParams()->HasPlatformDirectories())
#endif
  {
    CDirectory::Create("special://xbmc/addons");
  }

  PrintStartupLog();

  // TODO: initialize network adapter and network protocols

  CLog::Log(LOGINFO, "loading settings");
  const auto settingsComponent = CServiceBroker::GetSettingsComponent();
  if (!settingsComponent->Load())
    return false;

  CLog::Log(LOGINFO, "creating subdirectories");
  const std::shared_ptr<CProfileManager> profileManager = settingsComponent->GetProfileManager();
  const std::shared_ptr<CSettings> settings = settingsComponent->GetSettings();
  CLog::Log(LOGINFO, "userdata folder: {}",
            CURL::GetRedacted(profileManager->GetProfileUserDataFolder()));
  CLog::Log(LOGINFO, "recording folder: {}",
            CURL::GetRedacted(settings->GetString(CSettings::SETTING_AUDIOCDS_RECORDINGPATH)));
  CLog::Log(LOGINFO, "screenshots folder: {}",
            CURL::GetRedacted(settings->GetString(CSettings::SETTING_DEBUG_SCREENSHOTPATH)));
  CDirectory::Create(profileManager->GetUserDataFolder());
  CDirectory::Create(profileManager->GetProfileUserDataFolder());
  profileManager->CreateProfileFolders();

  if (!m_ServiceManager->InitStageTwo(
          settingsComponent->GetProfileManager()->GetProfileUserDataFolder()))
  {
    return false;
  }

  // load the keyboard layouts
  if (!keyboardLayoutManager->Load())
  {
    CLog::Log(LOGFATAL, "CApplication::Create: Unable to load keyboard layouts");
    return false;
  }

  CUtil::InitRandomSeed();

  m_lastRenderTime = std::chrono::steady_clock::now();
  return true;
}

bool CApplication::CreateGUI()
{
  // TODO: implement this
  return false;
}

bool CApplication::Initialize()
{
#if defined(HAS_DVD_DRIVE) && !defined(TARGET_WINDOWS) // somehow this throws an "unresolved external symbol" on win32
  // turn off cdio logging
  cdio_loglevel_default = CDIO_LOG_ERROR;
#endif

  // load the language and its translated strings
  if (!LoadLanguage(false))
    return false;

  // load media manager sources (e.g. root addon type sources depend on language strings to be available)
  CServiceBroker::GetMediaManager().LoadSources();

  const std::shared_ptr<CProfileManager> profileManager = CServiceBroker::GetSettingsComponent()->GetProfileManager();

  // TODO: wait for network connection

  // initialize (and update as needed) our databases
  CDatabaseManager &databaseManager = m_ServiceManager->GetDatabaseManager();

  CEvent event(true);
  CServiceBroker::GetJobManager()->Submit([&databaseManager, &event]() {
    databaseManager.Initialize();
    event.Set();
  });

  std::string localizedStr = g_localizeStrings.Get(24150);
  int iDots = 1;
  while (!event.Wait(1000ms))
  {
    if (databaseManager.IsUpgrading())
      CSplash::GetInstance().Show(std::string(iDots, ' ') + localizedStr + std::string(iDots, '.'));

    if (iDots == 3)
      iDots = 1;
    else
      ++iDots;
  }
  CSplash::GetInstance().Show("");

  // GUI depends on seek handler
  GetComponent<CApplicationPlayer>()->GetSeekHandler().Configure();

  const auto skinHandling = GetComponent<CApplicationSkinHandling>();

  bool uiInitializationFinished = false;

  if (CServiceBroker::GetGUI()->GetWindowManager().Initialized())
  {
    const auto settings = CServiceBroker::GetSettingsComponent()->GetSettings();

    CServiceBroker::GetGUI()->GetWindowManager().CreateWindows();

    skinHandling->m_confirmSkinChange = false;

    std::vector<AddonInfoPtr> incompatibleAddons;
    event.Reset();

    // Addon migration
    if (CServiceBroker::GetAddonMgr().GetIncompatibleEnabledAddonInfos(incompatibleAddons))
    {
      if (CAddonSystemSettings::GetInstance().GetAddonAutoUpdateMode() == AUTO_UPDATES_ON)
      {
        CServiceBroker::GetJobManager()->Submit(
            [&event, &incompatibleAddons]() {
              if (CServiceBroker::GetRepositoryUpdater().CheckForUpdates())
                CServiceBroker::GetRepositoryUpdater().Await();

              incompatibleAddons = CServiceBroker::GetAddonMgr().MigrateAddons();
              event.Set();
            },
            CJob::PRIORITY_DEDICATED);
        localizedStr = g_localizeStrings.Get(24151);
        iDots = 1;
        while (!event.Wait(1000ms))
        {
          CSplash::GetInstance().Show(std::string(iDots, ' ') + localizedStr +
                                      std::string(iDots, '.'));
          if (iDots == 3)
            iDots = 1;
          else
            ++iDots;
        }
        m_incompatibleAddons = incompatibleAddons;
      }
      else
      {
        // If no update is active disable all incompatible addons during start
        m_incompatibleAddons =
            CServiceBroker::GetAddonMgr().DisableIncompatibleAddons(incompatibleAddons);
      }
    }

    // Start splashscreen and load skin
    CSplash::GetInstance().Show("");
    skinHandling->m_confirmSkinChange = true;

    auto setting = settings->GetSetting(CSettings::SETTING_LOOKANDFEEL_SKIN);
    if (!setting)
    {
      CLog::Log(LOGFATAL, "Failed to load setting for: {}", CSettings::SETTING_LOOKANDFEEL_SKIN);
      return false;
    }

    CServiceBroker::RegisterTextureCache(std::make_shared<CTextureCache>());

    std::string skinId = settings->GetString(CSettings::SETTING_LOOKANDFEEL_SKIN);
    if (!skinHandling->LoadSkin(skinId))
    {
      CLog::Log(LOGERROR, "Failed to load skin '{}'", skinId);
      std::string defaultSkin =
          std::static_pointer_cast<const CSettingString>(setting)->GetDefault();
      if (!skinHandling->LoadSkin(defaultSkin))
      {
        CLog::Log(LOGFATAL, "Default skin '{}' could not be loaded! Terminating..", defaultSkin);
        return false;
      }
    }

    // initialize splash window after splash screen disappears
    // because we need a real window in the background which gets
    // rendered while we load the main window or enter the master lock key
    CServiceBroker::GetGUI()->GetWindowManager().ActivateWindow(WINDOW_SPLASH);

    if (settings->GetBool(CSettings::SETTING_MASTERLOCK_STARTUPLOCK) &&
        profileManager->GetMasterProfile().getLockMode() != LOCK_MODE_EVERYONE &&
        !profileManager->GetMasterProfile().getLockCode().empty())
    {
      g_passwordManager.CheckStartUpLock();
    }

    // check if we should use the login screen
    if (profileManager->UsingLoginScreen())
    {
      CServiceBroker::GetGUI()->GetWindowManager().ActivateWindow(WINDOW_LOGIN_SCREEN);
    }
    else
    {
      // activate the configured start window
      int firstWindow = g_SkinInfo->GetFirstWindow();
      CServiceBroker::GetGUI()->GetWindowManager().ActivateWindow(firstWindow);

      if (CServiceBroker::GetGUI()->GetWindowManager().IsWindowActive(WINDOW_STARTUP_ANIM))
      {
        CLog::Log(LOGWARNING, "CApplication::Initialize - startup.xml taints init process");
      }

      // the startup window is considered part of the initialization as it most likely switches to the final window
      uiInitializationFinished = firstWindow != WINDOW_STARTUP_ANIM;
    }
  }
  else //No GUI Created
  {
    uiInitializationFinished = true;
  }

  if (!m_ServiceManager->InitStageThree(profileManager))
  {
    CLog::Log(LOGERROR, "Application - Init3 failed");
  }

  g_sysinfo.Refresh();

  CLog::Log(LOGINFO, "removing tempfiles");
  CUtil::RemoveTempFiles();

  if (!profileManager->UsingLoginScreen())
  {
    UpdateLibraries();
    SetLoggingIn(false);
  }

  m_slowTimer.StartZero();

  // register action listeners
  const auto appListener = GetComponent<CApplicationActionListeners>();
  const auto appPlayer = GetComponent<CApplicationPlayer>();
  appListener->RegisterActionListener(&appPlayer->GetSeekHandler());
  appListener->RegisterActionListener(&CPlayerController::GetInstance());

  CServiceBroker::GetRepositoryUpdater().Start();
  if (!profileManager->UsingLoginScreen())
    CServiceBroker::GetServiceAddons().Start();

  CLog::Log(LOGINFO, "initialize done");

  const auto appPower = GetComponent<CApplicationPowerHandling>();
  appPower->CheckOSScreenSaverInhibitionSetting();
  // reset our screensaver (starts timers etc.)
  appPower->ResetScreenSaver();

  // if the user interfaces has been fully initialized let everyone know
  if (uiInitializationFinished)
  {
    CGUIMessage msg(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UI_READY);
    CServiceBroker::GetGUI()->GetWindowManager().SendThreadMessage(msg);
  }

  return true;
}

void CApplication::Render()
{
  // Handle rendering
}

bool CApplication::OnAction(const CAction &action)
{
  return false;
}

int CApplication::GetMessageMask()
{
  return 0;
}

void CApplication::OnApplicationMessage(ThreadMessage* pMsg)
{
}

void CApplication::FrameMove(bool processEvents, bool processGUI)
{
}

int CApplication::Run()
{
  // TODO: implement this
  return 0;
}

bool CApplication::Cleanup()
{
  // TODO: implement this
  return false;
}

bool CApplication::Stop(int exitCode)
{
  // TODO: implement this
  return false;
}

bool CApplication::PlayMedia(CFileItem& item, const std::string& player, PLAYLIST::Id playlistId)
{
  //nothing special just play
  return PlayFile(item, player, false);
}

bool CApplication::PlayFile(CFileItem item, const std::string& player, bool bRestart)
{
  return false;
}

void CApplication::StopPlaying()
{
}

bool CApplication::OnMessage(CGUIMessage& message)
{
  return false;
}

void CApplication::Process()
{
}

void CApplication::Restart(bool bSamePosition)
{
}

const std::string& CApplication::CurrentFile()
{
  return "";
}

std::shared_ptr<CFileItem> CApplication::CurrentFileItemPtr()
{
  return m_itemCurrentFile;
}

CFileItem& CApplication::CurrentFileItem()
{
  return *m_itemCurrentFile;
}

CFileItem& CApplication::CurrentUnstackedItem()
{
  return *CFileItemPtr();
}

// Returns the total time in seconds of the current media.  Fractional
// portions of a second are possible - but not necessarily supported by the
// player class.  This returns a double to be consistent with GetTime() and
// SeekTime().
double CApplication::GetTotalTime() const
{
  return 0.0;
}

// Returns the current time in seconds of the currently playing media.
// Fractional portions of a second are possible.  This returns a double to
// be consistent with GetTotalTime() and SeekTime().
double CApplication::GetTime() const
{
  return 0.0;
}

// Sets the current position of the currently playing media to the specified
// time in seconds.  Fractional portions of a second are valid.  The passed
// time is the time offset from the beginning of the file as opposed to a
// delta from the current position.  This method accepts a double to be
// consistent with GetTime() and GetTotalTime().
void CApplication::SeekTime( double dTime )
{
}

float CApplication::GetPercentage() const
{
  return 0.0f;
}

float CApplication::GetCachePercentage() const
{
  return 0.0f;
}

void CApplication::SeekPercentage(float percent)
{
}

// SwitchToFullScreen() returns true if a switch is made, else returns false
bool CApplication::SwitchToFullScreen(bool force /* = false */)
{
  return false;
}

bool CApplication::IsMuted() const
{
  return false;
}

void CApplication::ToggleMute(void)
{
}

void CApplication::SetVolume(long iValue, bool isPercentage /* = true */)
{
}

int CApplication::GetVolume(bool percentage /* = true */) const
{
  return 0;
}

bool CApplication::IsCurrentThread() const
{
  return true;
}

void CApplication::UpdateCurrentPlayArt()
{

}

bool CApplication::ProcessAndStartPlaylist(const std::string& strPlayList,
                                           PLAYLIST::CPlayList& playlist,
                                           PLAYLIST::Id playlistId,
                                           int track)
{
  CLog::Log(LOGDEBUG, "CApplication::ProcessAndStartPlaylist({}, {})", strPlayList, playlistId);

  // initial exit conditions
  // no songs in playlist just return
  if (playlist.size() == 0)
    return false;

  // illegal playlist
  if (playlistId == PLAYLIST::TYPE_NONE || playlistId == PLAYLIST::TYPE_PICTURE)
    return false;

  // setup correct playlist
  CServiceBroker::GetPlaylistPlayer().ClearPlaylist(playlistId);

  // if the playlist contains an internet stream, this file will be used
  // to generate a thumbnail for musicplayer.cover
  m_strPlayListFile = strPlayList;

  // add the items to the playlist player
  CServiceBroker::GetPlaylistPlayer().Add(playlistId, playlist);

  // if we have a playlist
  if (CServiceBroker::GetPlaylistPlayer().GetPlaylist(playlistId).size())
  {
    // start playing it
    CServiceBroker::GetPlaylistPlayer().SetCurrentPlaylist(playlistId);
    CServiceBroker::GetPlaylistPlayer().Reset();
    CServiceBroker::GetPlaylistPlayer().Play(track, "");
    return true;
  }
  return false;
}

std::string CApplication::GetCurrentPlayer()
{
  return m_pPlayer->GetCurrentPlayer();
}

void CApplication::UpdateLibraries()
{
}

bool CApplication::SetLanguage(const std::string &strLanguage)
{
  return false;
}

bool CApplication::LoadLanguage(bool reload)
{
  return false;
}

void CApplication::SetLoggingIn(bool switchingProfiles)
{
}

void CApplication::PrintStartupLog()
{
  CLog::Log(LOGINFO, "-----------------------------------------------------------------------");
  CLog::Log(LOGNOTICE, "Starting XBMC. Built on {}", __DATE__);
  CSpecialProtocol::LogPaths();
  CLog::Log(LOGINFO, "-----------------------------------------------------------------------");
}
