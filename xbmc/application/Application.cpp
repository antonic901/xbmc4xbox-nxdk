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

#include "application/Application.h"

#include "PlayListPlayer.h"
#include "ServiceManager.h"
#include "application/ApplicationActionListeners.h"
#include "application/ApplicationPowerHandling.h"
#include "application/ApplicationSkinHandling.h"
#include "application/ApplicationStackHelper.h"
#include "playlists/PlayListFactory.h"
#include "ServiceBroker.h"
#include "messaging/ApplicationMessenger.h"
#include "messaging/ThreadMessage.h"

#include "playlists/PlayList.h"
#include "playlists/SmartPlayList.h"

#include "input/Key.h"

using namespace KODI::MESSAGING;

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

void CApplication::Render()
{
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
