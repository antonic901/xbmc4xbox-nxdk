#pragma once

/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "playlists/PlayListTypes.h"
#include "utils/GlobalsHandling.h"

#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <vector>

class CAction;
class CFileItem;
class CFileItemList;
class CKey;
class CServiceManager;

namespace PLAYLIST
{
  class CPlayList;
}

#include "ApplicationPlayer.h"
#include "FileItem.h"

#define VOLUME_MINIMUM -6000  // -60dB
#define VOLUME_MAXIMUM 0      // 0dB

class CApplication
{
  friend class CApplicationPlayer;
public:
  CApplication(void);
  virtual ~CApplication(void);

  bool IsCurrentThread() const;
  const std::string& CurrentFile();
  CFileItem& CurrentFileItem();
  std::shared_ptr<CFileItem> CurrentFileItemPtr();
  CFileItem& CurrentUnstackedItem();
  std::string GetCurrentPlayer();

  bool PlayMedia(CFileItem& item, const std::string& player, PLAYLIST::Id playlistId);
  bool ProcessAndStartPlaylist(const std::string& strPlayList,
                               PLAYLIST::CPlayList& playlist,
                               PLAYLIST::Id playlistId,
                               int track = 0);
  bool PlayFile(CFileItem item, const std::string& player, bool bRestart = false);
  void StopPlaying();

  /*!
   \brief Returns the total time in fractional seconds of the currently playing media

   Beware that this method returns fractional seconds whereas IPlayer::GetTotalTime() returns milliseconds.
   */
  double GetTotalTime() const;
  /*!
   \brief Returns the current time in fractional seconds of the currently playing media

   Beware that this method returns fractional seconds whereas IPlayer::GetTime() returns milliseconds.
   */
  double GetTime() const;

  void SeekTime( double dTime = 0.0 );

  int GetVolume(bool percentage = true) const;
  void SetVolume(long iValue, bool isPercentage = true);

  bool SetLanguage(const std::string &strLanguage);
  bool LoadLanguage(bool reload);

  void SetLoggingIn(bool switchingProfiles);

  void UpdateLibraries();

  void UpdateCurrentPlayArt();

  bool OnAction(const CAction &action);

  void ResetScreenSaver();

  CApplicationPlayer* m_pPlayer;

  bool IsStandAlone()
  {
    return true;
  }

  // should be part of XBApplicationEx.h
  bool m_bStop = false;

  std::unique_ptr<CServiceManager> m_ServiceManager;

protected:
  CFileItemPtr m_itemCurrentFile;
};

XBMC_GLOBAL_REF(CApplication,g_application);
#define g_application XBMC_GLOBAL_USE(CApplication)
