/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */
#include "XboxStorageProvider.h"

#include "ServiceBroker.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/LocalizeStrings.h"
#include "storage/MediaManager.h"
#include "utils/JobManager.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

#include "platform/win32/CharsetConverter.h"

bool CXboxStorageProvider::xbevent = false;

std::unique_ptr<IStorageProvider> IStorageProvider::CreateInstance()
{
  return std::make_unique<CXboxStorageProvider>();
}

void CXboxStorageProvider::Initialize()
{
  // check for a DVD drive
  VECSOURCES vShare;
  GetDrivesByType(vShare, DVD_DRIVES);
  if(!vShare.empty())
    CServiceBroker::GetMediaManager().SetHasOpticalDrive(true);
  else
    CLog::Log(LOGDEBUG, "{}: No optical drive found.", __FUNCTION__);

#ifdef HAS_DVD_DRIVE
  // Can be removed once the StorageHandler supports optical media
  for (const auto& it : vShare)
    if (CServiceBroker::GetMediaManager().GetDriveStatus(it.strPath) ==
        DriveState::CLOSED_MEDIA_PRESENT)
      CServiceBroker::GetJobManager()->AddJob(new CDetectDisc(it.strPath, false), nullptr);
      // remove end
#endif
}

void CXboxStorageProvider::GetLocalDrives(VECSOURCES &localDrives)
{
  // TODO: implement this
}

void CXboxStorageProvider::GetRemovableDrives(VECSOURCES &removableDrives)
{
  GetDrivesByType(removableDrives, REMOVABLE_DRIVES, true);
}

std::string CXboxStorageProvider::GetFirstOpticalDeviceFileName()
{
  VECSOURCES vShare;
  std::string strdevice = "\\\\.\\";
  GetDrivesByType(vShare, DVD_DRIVES);

  if(!vShare.empty())
    return strdevice.append(vShare.front().strPath);
  else
    return "";
}

bool CXboxStorageProvider::Eject(const std::string& mountpath)
{
  // TODO: implemen this
  return false;
}

std::vector<std::string > CXboxStorageProvider::GetDiskUsage()
{
  std::vector<std::string> result;

  // TODO: implement this
  return result;
}

bool CXboxStorageProvider::PumpDriveChangeEvents(IStorageEventsCallback *callback)
{
  bool b = xbevent;
  xbevent = false;
  return b;
}

void CXboxStorageProvider::GetDrivesByType(VECSOURCES &localDrives, Drive_Types eDriveType, bool bonlywithmedia)
{
  // TODO: implement this
}

CDetectDisc::CDetectDisc(const std::string &strPath, const bool bautorun)
  : m_strPath(strPath), m_bautorun(bautorun)
{
}

bool CDetectDisc::DoWork()
{
#ifdef HAS_DVD_DRIVE
  CLog::Log(LOGDEBUG, "{}: Optical media found in drive {}", __FUNCTION__, m_strPath);
  CMediaSource share;
  share.strPath = m_strPath;
  share.strStatus = CServiceBroker::GetMediaManager().GetDiskLabel(share.strPath);
  share.strDiskUniqueId = CServiceBroker::GetMediaManager().GetDiskUniqueId(share.strPath);
  if (CServiceBroker::GetMediaManager().IsAudio(share.strPath))
    share.strStatus = "Audio-CD";
  else if(share.strStatus == "")
    share.strStatus = g_localizeStrings.Get(446);
  share.strName = share.strPath;
  share.m_ignore = true;
  share.m_iDriveType = CMediaSource::SOURCE_TYPE_DVD;
  CServiceBroker::GetMediaManager().AddAutoSource(share, m_bautorun);
#endif
  return true;
}
