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
#include "playlists/PlayListFactory.h"

#include "playlists/PlayList.h"
#include "playlists/SmartPlayList.h"

#include "input/Key.h"

CApplication::CApplication(void)
  : m_pPlayer(new CApplicationPlayer)
  , m_itemCurrentFile(new CFileItem)
{
}

CApplication::~CApplication(void)
{
}

bool CApplication::OnAction(const CAction &action)
{
  return false;
}

void CApplication::ResetScreenSaver()
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

std::shared_ptr<CFileItem> CApplication::CurrentFileItemPtr()
{
  return m_itemCurrentFile;
}

CFileItem& CApplication::CurrentFileItem()
{
  return *m_itemCurrentFile;
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

#if 0
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
#endif
  return false;
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
