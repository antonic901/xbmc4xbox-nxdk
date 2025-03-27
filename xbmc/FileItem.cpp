/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "FileItem.h"

#include "URL.h"
#include "Util.h"
#include "filesystem/File.h"
#include "music/tags/MusicInfoTag.h"
#include "pictures/PictureInfoTag.h"
#include "settings/AdvancedSettings.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/Variant.h"
#include "utils/log.h"
#include "video/VideoDatabase.h"
#include "video/VideoInfoTag.h"

#include <algorithm>
#include <cstdlib>
#include <mutex>

using namespace XFILE;
using namespace MUSIC_INFO;

CFileItem::CFileItem(const CSong& song)
{
  Initialize();
}

CFileItem::CFileItem(const CSong& song, const CMusicInfoTag& music)
{
  Initialize();
  *GetMusicInfoTag() = music;
}

CFileItem::CFileItem(const CURL &url, const CAlbum& album)
{
  Initialize();

  m_strPath = url.Get();
  URIUtils::AddSlashAtEnd(m_strPath);
}

CFileItem::CFileItem(const std::string &path, const CAlbum& album)
{
  Initialize();

  m_strPath = path;
  URIUtils::AddSlashAtEnd(m_strPath);
}

CFileItem::CFileItem(const CMusicInfoTag& music)
{
  Initialize();
  SetLabel(music.GetTitle());
  m_strPath = music.GetURL();
  m_bIsFolder = URIUtils::HasSlashAtEnd(m_strPath);
  *GetMusicInfoTag() = music;
  FillInDefaultIcon();
  FillInMimeType(false);
}

CFileItem::CFileItem(const CVideoInfoTag& movie)
{
  Initialize();
}

CFileItem::CFileItem(const CFileItem& item)
{
  *this = item;
}

CFileItem::CFileItem(const CGUIListItem& item)
{
  Initialize();
  // not particularly pretty, but it gets around the issue of Initialize() defaulting
  // parameters in the CGUIListItem base class.
  *((CGUIListItem *)this) = item;

  FillInMimeType(false);
}

CFileItem::CFileItem(void)
{
  Initialize();
}

CFileItem::CFileItem(const std::string& strLabel)
{
  Initialize();
  SetLabel(strLabel);
}

CFileItem::CFileItem(const char* strLabel)
{
  Initialize();
  SetLabel(std::string(strLabel));
}

CFileItem::CFileItem(const CURL& path, bool bIsFolder)
{
  Initialize();
  m_strPath = path.Get();
  m_bIsFolder = bIsFolder;
  if (m_bIsFolder && !m_strPath.empty() && !IsFileFolder())
    URIUtils::AddSlashAtEnd(m_strPath);
  FillInMimeType(false);
}

CFileItem::CFileItem(const std::string& strPath, bool bIsFolder)
{
  Initialize();
  m_strPath = strPath;
  m_bIsFolder = bIsFolder;
  if (m_bIsFolder && !m_strPath.empty() && !IsFileFolder())
    URIUtils::AddSlashAtEnd(m_strPath);
  FillInMimeType(false);
}

CFileItem::CFileItem(const CMediaSource& share)
{
  Initialize();
  m_bIsFolder = true;
  m_bIsShareOrDrive = true;
  m_strPath = share.strPath;
  if (!IsRSS()) // no slash at end for rss feeds
    URIUtils::AddSlashAtEnd(m_strPath);
  std::string label = share.strName;
  if (!share.strStatus.empty())
    label = StringUtils::Format("{} ({})", share.strName, share.strStatus);
  SetLabel(label);
  m_iLockMode = share.m_iLockMode;
  m_strLockCode = share.m_strLockCode;
  m_iHasLock = share.m_iHasLock;
  m_iBadPwdCount = share.m_iBadPwdCount;
  m_iDriveType = share.m_iDriveType;
  SetArt("thumb", share.m_strThumbnailImage);
  SetLabelPreformatted(true);
  if (IsDVD())
    GetVideoInfoTag()->m_strFileNameAndPath = share.strDiskUniqueId; // share.strDiskUniqueId contains disc unique id
  FillInMimeType(false);
}

CFileItem::CFileItem(std::shared_ptr<const ADDON::IAddon> addonInfo) : m_addonInfo(std::move(addonInfo))
{
  Initialize();
}

CFileItem::~CFileItem(void)
{
}

CFileItem& CFileItem::operator=(const CFileItem& item)
{
  if (this == &item)
    return *this;

  CGUIListItem::operator=(item);
  m_bLabelPreformatted=item.m_bLabelPreformatted;
  m_strPath = item.m_strPath;
  m_strDynPath = item.m_strDynPath;
  m_bIsParentFolder = item.m_bIsParentFolder;
  m_iDriveType = item.m_iDriveType;
  m_bIsShareOrDrive = item.m_bIsShareOrDrive;
  m_dwSize = item.m_dwSize;

  m_lStartOffset = item.m_lStartOffset;
  m_lStartPartNumber = item.m_lStartPartNumber;
  m_lEndOffset = item.m_lEndOffset;
  m_strDVDLabel = item.m_strDVDLabel;
  m_strTitle = item.m_strTitle;
  m_iprogramCount = item.m_iprogramCount;
  m_idepth = item.m_idepth;
  m_iLockMode = item.m_iLockMode;
  m_strLockCode = item.m_strLockCode;
  m_iHasLock = item.m_iHasLock;
  m_iBadPwdCount = item.m_iBadPwdCount;
  m_bCanQueue=item.m_bCanQueue;
  m_mimetype = item.m_mimetype;
  m_extrainfo = item.m_extrainfo;
  m_bIsAlbum = item.m_bIsAlbum;
  m_doContentLookup = item.m_doContentLookup;
  return *this;
}

void CFileItem::Initialize()
{
  m_bLabelPreformatted = false;
  m_bIsAlbum = false;
  m_dwSize = 0;
  m_bIsParentFolder = false;
  m_bIsShareOrDrive = false;
  m_lStartOffset = 0;
  m_lStartPartNumber = 1;
  m_lEndOffset = 0;
  m_iprogramCount = 0;
  m_idepth = 1;
  m_iLockMode = LOCK_MODE_EVERYONE;
  m_iBadPwdCount = 0;
  m_bCanQueue = true;
  m_doContentLookup = true;
}

void CFileItem::Reset()
{
  // CGUIListItem members...
  SetLabel("");
  m_bIsFolder = false;

  m_strDVDLabel.clear();
  m_strTitle.clear();
  m_strPath.clear();
  m_strDynPath.clear();
  m_strLockCode.clear();
  m_mimetype.clear();
  m_extrainfo.clear();

  Initialize();
}

// do not archive dynamic path
void CFileItem::Archive(CArchive& ar)
{
  CGUIListItem::Archive(ar);
}

void CFileItem::Serialize(CVariant& value) const
{
  //CGUIListItem::Serialize(value["CGUIListItem"]);

  value["strPath"] = m_strPath;
  value["size"] = m_dwSize;
  value["DVDLabel"] = m_strDVDLabel;
  value["title"] = m_strTitle;
  value["mimetype"] = m_mimetype;
  value["extrainfo"] = m_extrainfo;
}

bool CFileItem::Exists(bool bUseCache /* = true */) const
{
  return false;
}

bool CFileItem::IsVideo() const
{
  return false;
}

bool CFileItem::IsAudio() const
{
  return false;
}

bool CFileItem::IsDeleted() const
{
  return false;
}

bool CFileItem::IsAudioBook() const
{
  return IsType(".m4b") || IsType(".mka");
}

bool CFileItem::IsGame() const
{
  return false;
}

bool CFileItem::IsPicture() const
{
  return false;
}

bool CFileItem::IsLyrics() const
{
  return URIUtils::HasExtension(m_strPath, ".cdg|.lrc");
}

bool CFileItem::IsSubtitle() const
{
  return false;
}

bool CFileItem::IsCUESheet() const
{
  return URIUtils::HasExtension(m_strPath, ".cue");
}

bool CFileItem::IsInternetStream(const bool bStrictCheck /* = false */) const
{
  return false;
}

bool CFileItem::IsStreamedFilesystem() const
{
  if (!m_strDynPath.empty())
    return URIUtils::IsStreamedFilesystem(m_strDynPath);

  return URIUtils::IsStreamedFilesystem(m_strPath);
}

bool CFileItem::IsFileFolder(EFileFolderType types) const
{
  return false;
}

bool CFileItem::IsSmartPlayList() const
{
  return false;
}

bool CFileItem::IsLibraryFolder() const
{
  return false;
}

bool CFileItem::IsPlayList() const
{
  return false;
}

bool CFileItem::IsPythonScript() const
{
  return URIUtils::HasExtension(m_strPath, ".py");
}

bool CFileItem::IsType(const char *ext) const
{
  if (!m_strDynPath.empty())
    return URIUtils::HasExtension(m_strDynPath, ext);

  return URIUtils::HasExtension(m_strPath, ext);
}

bool CFileItem::IsNFO() const
{
  return URIUtils::HasExtension(m_strPath, ".nfo");
}

bool CFileItem::IsDiscImage() const
{
  return URIUtils::HasExtension(GetDynPath(), ".img|.iso|.nrg|.udf");
}

bool CFileItem::IsOpticalMediaFile() const
{
  if (IsDVDFile(false, true))
    return true;

  return IsBDFile();
}

bool CFileItem::IsDVDFile(bool bVobs /*= true*/, bool bIfos /*= true*/) const
{
  std::string strFileName = URIUtils::GetFileName(GetDynPath());
  if (bIfos)
  {
    if (StringUtils::EqualsNoCase(strFileName, "video_ts.ifo"))
      return true;
    if (StringUtils::StartsWithNoCase(strFileName, "vts_") && StringUtils::EndsWithNoCase(strFileName, "_0.ifo") && strFileName.length() == 12)
      return true;
  }
  if (bVobs)
  {
    if (StringUtils::EqualsNoCase(strFileName, "video_ts.vob"))
      return true;
    if (StringUtils::StartsWithNoCase(strFileName, "vts_") && StringUtils::EndsWithNoCase(strFileName, ".vob"))
      return true;
  }

  return false;
}

bool CFileItem::IsBDFile() const
{
  std::string strFileName = URIUtils::GetFileName(GetDynPath());
  return (StringUtils::EqualsNoCase(strFileName, "index.bdmv") || StringUtils::EqualsNoCase(strFileName, "MovieObject.bdmv")
          || StringUtils::EqualsNoCase(strFileName, "INDEX.BDM") || StringUtils::EqualsNoCase(strFileName, "MOVIEOBJ.BDM"));
}

bool CFileItem::IsRAR() const
{
  return URIUtils::IsRAR(m_strPath);
}

bool CFileItem::IsAPK() const
{
  return URIUtils::IsAPK(m_strPath);
}

bool CFileItem::IsZIP() const
{
  return URIUtils::IsZIP(m_strPath);
}

bool CFileItem::IsCBZ() const
{
  return URIUtils::HasExtension(m_strPath, ".cbz");
}

bool CFileItem::IsCBR() const
{
  return URIUtils::HasExtension(m_strPath, ".cbr");
}

bool CFileItem::IsRSS() const
{
  return StringUtils::StartsWithNoCase(m_strPath, "rss://") || URIUtils::HasExtension(m_strPath, ".rss")
      || StringUtils::StartsWithNoCase(m_strPath, "rsss://")
      || m_mimetype == "application/rss+xml";
}

bool CFileItem::IsAndroidApp() const
{
  return URIUtils::IsAndroidApp(m_strPath);
}

bool CFileItem::IsStack() const
{
  return URIUtils::IsStack(m_strPath);
}

bool CFileItem::IsFavourite() const
{
  return URIUtils::IsFavourite(m_strPath);
}

bool CFileItem::IsPlugin() const
{
  return URIUtils::IsPlugin(m_strPath);
}

bool CFileItem::IsScript() const
{
  return URIUtils::IsScript(m_strPath);
}

bool CFileItem::IsAddonsPath() const
{
  return URIUtils::IsAddonsPath(m_strPath);
}

bool CFileItem::IsSourcesPath() const
{
  return URIUtils::IsSourcesPath(m_strPath);
}

bool CFileItem::IsMultiPath() const
{
  return URIUtils::IsMultiPath(m_strPath);
}

bool CFileItem::IsBluray() const
{
  if (URIUtils::IsBluray(m_strPath))
    return true;

  CFileItem item = CFileItem(GetOpticalMediaPath(), false);

  return item.IsBDFile();
}

bool CFileItem::IsProtectedBlurayDisc() const
{
  return false;
}

bool CFileItem::IsCDDA() const
{
  return URIUtils::IsCDDA(m_strPath);
}

bool CFileItem::IsDVD() const
{
  return URIUtils::IsDVD(m_strPath);
}

bool CFileItem::IsOnDVD() const
{
  return URIUtils::IsOnDVD(m_strPath);
}

bool CFileItem::IsNfs() const
{
  return false;
}

bool CFileItem::IsOnLAN() const
{
  return false;
}

bool CFileItem::IsISO9660() const
{
  return URIUtils::IsISO9660(m_strPath);
}

bool CFileItem::IsRemote() const
{
  return URIUtils::IsRemote(m_strPath);
}

bool CFileItem::IsSmb() const
{
  return URIUtils::IsSmb(m_strPath);
}

bool CFileItem::IsURL() const
{
  return URIUtils::IsURL(m_strPath);
}

bool CFileItem::IsPVR() const
{
  return URIUtils::IsPVR(m_strPath);
}

bool CFileItem::IsLiveTV() const
{
  return URIUtils::IsLiveTV(m_strPath);
}

bool CFileItem::IsHD() const
{
  return URIUtils::IsHD(m_strPath);
}

bool CFileItem::IsMusicDb() const
{
  return URIUtils::IsMusicDb(m_strPath);
}

bool CFileItem::IsVideoDb() const
{
  return URIUtils::IsVideoDb(m_strPath);
}

bool CFileItem::IsEPG() const
{
  return HasEPGInfoTag();
}

bool CFileItem::IsPVRChannel() const
{
  return HasPVRChannelInfoTag();
}

bool CFileItem::IsPVRChannelGroup() const
{
  return false;
}

bool CFileItem::IsPVRRecording() const
{
  return HasPVRRecordingInfoTag();
}

bool CFileItem::IsUsablePVRRecording() const
{
  return false;
}

bool CFileItem::IsDeletedPVRRecording() const
{
  return false;
}

bool CFileItem::IsInProgressPVRRecording() const
{
  return false;
}

bool CFileItem::IsPVRTimer() const
{
  return HasPVRTimerInfoTag();
}

bool CFileItem::IsVirtualDirectoryRoot() const
{
  return (m_bIsFolder && m_strPath.empty());
}

bool CFileItem::IsRemovable() const
{
  return IsOnDVD() || IsCDDA();
}

bool CFileItem::IsReadOnly() const
{
  if (IsParentFolder())
    return true;

  if (m_bIsShareOrDrive)
    return true;

  return !URIUtils::IsHD(m_strPath);
}

void CFileItem::FillInDefaultIcon()
{
}

void CFileItem::RemoveExtension()
{
  if (m_bIsFolder)
    return;
}

void CFileItem::CleanString()
{
}

void CFileItem::SetLabel(const std::string &strLabel)
{
  if (strLabel == "..")
  {
    m_bIsParentFolder = true;
    m_bIsFolder = true;
  }
  CGUIListItem::SetLabel(strLabel);
}

void CFileItem::SetFileSizeLabel()
{
}

bool CFileItem::CanQueue() const
{
  return m_bCanQueue;
}

void CFileItem::SetCanQueue(bool bYesNo)
{
  m_bCanQueue = bYesNo;
}

bool CFileItem::IsParentFolder() const
{
  return m_bIsParentFolder;
}

void CFileItem::FillInMimeType(bool lookup /*= true*/)
{
  //! @todo adapt this to use CMime::GetMimeType()
  if (m_mimetype.empty())
  {
    if (m_bIsFolder)
      m_mimetype = "x-directory/normal";

    // if it's still empty set to an unknown type
    if (m_mimetype.empty())
      m_mimetype = "application/octet-stream";
  }

  // change protocol to mms for the following mime-type.  Allows us to create proper FileMMS.
  if(StringUtils::StartsWithNoCase(m_mimetype, "application/vnd.ms.wms-hdr.asfv1") ||
     StringUtils::StartsWithNoCase(m_mimetype, "application/x-mms-framed"))
  {
    if (m_strDynPath.empty())
      m_strDynPath = m_strPath;

    StringUtils::Replace(m_strDynPath, "http:", "mms:");
  }
}

void CFileItem::SetMimeTypeForInternetFile()
{
  if (m_doContentLookup && IsInternetStream())
  {
    SetMimeType("");
    FillInMimeType(true);
  }
}

bool CFileItem::IsSamePath(const CFileItem *item) const
{
  if (!item)
    return false;

  return false;
}

bool CFileItem::IsAlbum() const
{
  return m_bIsAlbum;
}

void CFileItem::UpdateInfo(const CFileItem &item, bool replaceLabels /*=true*/)
{
}

void CFileItem::MergeInfo(const CFileItem& item)
{
}

void CFileItem::SetFromVideoInfoTag(const CVideoInfoTag &video)
{
}

void CFileItem::SetFromMusicInfoTag(const MUSIC_INFO::CMusicInfoTag& music)
{
}

void CFileItem::SetFromAlbum(const CAlbum &album)
{
}

void CFileItem::SetFromSong(const CSong &song)
{
}

std::string CFileItem::GetOpticalMediaPath() const
{
  return std::string();
}

/**
* @todo Ideally this (and SetPath) would not be available outside of construction
* for CFileItem objects, or at least restricted to essentially be equivalent
* to construction. This would require re-formulating a bunch of CFileItem
* construction, and also allowing CFileItemList to have its own (public)
* SetURL() function, so for now we give direct access.
*/
void CFileItem::SetURL(const CURL& url)
{
  m_strPath = url.Get();
}

const CURL CFileItem::GetURL() const
{
  CURL url(m_strPath);
  return url;
}

bool CFileItem::IsURL(const CURL& url) const
{
  return IsPath(url.Get());
}

bool CFileItem::IsPath(const std::string& path, bool ignoreURLOptions /* = false */) const
{
  return URIUtils::PathEquals(m_strPath, path, false, ignoreURLOptions);
}

void CFileItem::SetDynURL(const CURL& url)
{
  m_strDynPath = url.Get();
}

const CURL CFileItem::GetDynURL() const
{
  if (!m_strDynPath.empty())
  {
    CURL url(m_strDynPath);
    return url;
  }
  else
  {
    CURL url(m_strPath);
    return url;
  }
}

const std::string &CFileItem::GetDynPath() const
{
  if (!m_strDynPath.empty())
    return m_strDynPath;
  else
    return m_strPath;
}

void CFileItem::SetDynPath(const std::string &path)
{
  m_strDynPath = path;
}

void CFileItem::LoadEmbeddedCue()
{
}

bool CFileItem::HasCueDocument() const
{
  return false;
}

bool CFileItem::LoadTracksFromCueDocument(CFileItemList& scannedItems)
{
  return false;
}

/////////////////////////////////////////////////////////////////////////////////
/////
///// CFileItemList
/////
//////////////////////////////////////////////////////////////////////////////////

CFileItemList::CFileItemList()
: CFileItem("", true)
{
}

CFileItemList::CFileItemList(const std::string& strPath)
: CFileItem(strPath, true)
{
}

CFileItemList::~CFileItemList()
{
  Clear();
}

CFileItemPtr CFileItemList::operator[] (int iItem)
{
  return Get(iItem);
}

const CFileItemPtr CFileItemList::operator[] (int iItem) const
{
  return Get(iItem);
}

CFileItemPtr CFileItemList::operator[] (const std::string& strPath)
{
  return Get(strPath);
}

const CFileItemPtr CFileItemList::operator[] (const std::string& strPath) const
{
  return Get(strPath);
}

void CFileItemList::SetIgnoreURLOptions(bool ignoreURLOptions)
{
  m_ignoreURLOptions = ignoreURLOptions;

  if (m_fastLookup)
  {
    m_fastLookup = false; // Force SetFastlookup to clear map
    SetFastLookup(true);  // and regenerate map
  }
}

void CFileItemList::SetFastLookup(bool fastLookup)
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (fastLookup && !m_fastLookup)
  { // generate the map
    m_map.clear();
    for (unsigned int i=0; i < m_items.size(); i++)
    {
      CFileItemPtr pItem = m_items[i];
      m_map.insert(MAPFILEITEMSPAIR(m_ignoreURLOptions ? CURL(pItem->GetPath()).GetWithoutOptions() : pItem->GetPath(), pItem));
    }
  }
  if (!fastLookup && m_fastLookup)
    m_map.clear();
  m_fastLookup = fastLookup;
}

bool CFileItemList::Contains(const std::string& fileName) const
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (m_fastLookup)
    return m_map.find(m_ignoreURLOptions ? CURL(fileName).GetWithoutOptions() : fileName) != m_map.end();

  // slow method...
  for (unsigned int i = 0; i < m_items.size(); i++)
  {
    const CFileItemPtr pItem = m_items[i];
    if (pItem->IsPath(m_ignoreURLOptions ? CURL(fileName).GetWithoutOptions() : fileName))
      return true;
  }
  return false;
}

void CFileItemList::Clear()
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  ClearItems();
  m_sortIgnoreFolders = false;
  m_cacheToDisc = CACHE_IF_SLOW;
  m_replaceListing = false;
  m_content.clear();
}

void CFileItemList::ClearItems()
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  // make sure we free the memory of the items (these are GUIControls which may have allocated resources)
  for (unsigned int i = 0; i < m_items.size(); i++)
  {
    CFileItemPtr item = m_items[i];
  }
  m_items.clear();
  m_map.clear();
}

void CFileItemList::Add(CFileItemPtr pItem)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  if (m_fastLookup)
    m_map.insert(MAPFILEITEMSPAIR(m_ignoreURLOptions ? CURL(pItem->GetPath()).GetWithoutOptions() : pItem->GetPath(), pItem));
  m_items.emplace_back(std::move(pItem));
}

void CFileItemList::Add(CFileItem&& item)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  auto ptr = std::make_shared<CFileItem>(std::move(item));
  if (m_fastLookup)
    m_map.insert(MAPFILEITEMSPAIR(m_ignoreURLOptions ? CURL(ptr->GetPath()).GetWithoutOptions() : ptr->GetPath(), ptr));
  m_items.emplace_back(std::move(ptr));
}

void CFileItemList::AddFront(const CFileItemPtr &pItem, int itemPosition)
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (itemPosition >= 0)
  {
    m_items.insert(m_items.begin()+itemPosition, pItem);
  }
  else
  {
    m_items.insert(m_items.begin()+(m_items.size()+itemPosition), pItem);
  }
  if (m_fastLookup)
  {
    m_map.insert(MAPFILEITEMSPAIR(m_ignoreURLOptions ? CURL(pItem->GetPath()).GetWithoutOptions() : pItem->GetPath(), pItem));
  }
}

void CFileItemList::Remove(CFileItem* pItem)
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  for (IVECFILEITEMS it = m_items.begin(); it != m_items.end(); ++it)
  {
    if (pItem == it->get())
    {
      m_items.erase(it);
      if (m_fastLookup)
      {
        m_map.erase(m_ignoreURLOptions ? CURL(pItem->GetPath()).GetWithoutOptions() : pItem->GetPath());
      }
      break;
    }
  }
}

VECFILEITEMS::iterator CFileItemList::erase(VECFILEITEMS::iterator first,
                                            VECFILEITEMS::iterator last)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  return m_items.erase(first, last);
}

void CFileItemList::Remove(int iItem)
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (iItem >= 0 && iItem < Size())
  {
    CFileItemPtr pItem = *(m_items.begin() + iItem);
    if (m_fastLookup)
    {
      m_map.erase(m_ignoreURLOptions ? CURL(pItem->GetPath()).GetWithoutOptions() : pItem->GetPath());
    }
    m_items.erase(m_items.begin() + iItem);
  }
}

void CFileItemList::Append(const CFileItemList& itemlist)
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  for (int i = 0; i < itemlist.Size(); ++i)
    Add(itemlist[i]);
}

void CFileItemList::Assign(const CFileItemList& itemlist, bool append)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  if (!append)
    Clear();
  Append(itemlist);
  SetPath(itemlist.GetPath());
  SetLabel("");
  m_replaceListing = itemlist.m_replaceListing;
  m_content = itemlist.m_content;
  m_cacheToDisc = itemlist.m_cacheToDisc;
}

bool CFileItemList::Copy(const CFileItemList& items, bool copyItems /* = true */)
{
  // assign all CFileItem parts
  *static_cast<CFileItem*>(this) = static_cast<const CFileItem&>(items);

  // assign the rest of the CFileItemList properties
  m_replaceListing  = items.m_replaceListing;
  m_content         = items.m_content;
  m_cacheToDisc     = items.m_cacheToDisc;
  m_sortIgnoreFolders = items.m_sortIgnoreFolders;

  if (copyItems)
  {
    // make a copy of each item
    for (int i = 0; i < items.Size(); i++)
    {
      CFileItemPtr newItem(new CFileItem(*items[i]));
      Add(newItem);
    }
  }

  return true;
}

CFileItemPtr CFileItemList::Get(int iItem) const
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (iItem > -1 && iItem < (int)m_items.size())
    return m_items[iItem];

  return CFileItemPtr();
}

CFileItemPtr CFileItemList::Get(const std::string& strPath) const
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  if (m_fastLookup)
  {
    MAPFILEITEMS::const_iterator it =
        m_map.find(m_ignoreURLOptions ? CURL(strPath).GetWithoutOptions() : strPath);
    if (it != m_map.end())
      return it->second;

    return CFileItemPtr();
  }
  // slow method...
  for (unsigned int i = 0; i < m_items.size(); i++)
  {
    CFileItemPtr pItem = m_items[i];
    if (pItem->IsPath(m_ignoreURLOptions ? CURL(strPath).GetWithoutOptions() : strPath))
      return pItem;
  }

  return CFileItemPtr();
}

int CFileItemList::Size() const
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  return (int)m_items.size();
}

bool CFileItemList::IsEmpty() const
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  return m_items.empty();
}

void CFileItemList::Reserve(size_t iCount)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  m_items.reserve(iCount);
}

void CFileItemList::Sort(FILEITEMLISTCOMPARISONFUNC func)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  std::stable_sort(m_items.begin(), m_items.end(), func);
}

void CFileItemList::FillSortFields(FILEITEMFILLFUNC func)
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  std::for_each(m_items.begin(), m_items.end(), func);
}

void CFileItemList::Sort(SortBy sortBy, SortOrder sortOrder, SortAttribute sortAttributes /* = SortAttributeNone */)
{
}

void CFileItemList::Sort(SortDescription sortDescription)
{
}

void CFileItemList::Randomize()
{
}

void CFileItemList::FillInDefaultIcons()
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  for (int i = 0; i < (int)m_items.size(); ++i)
  {
    CFileItemPtr pItem = m_items[i];
    pItem->FillInDefaultIcon();
  }
}

int CFileItemList::GetFolderCount() const
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  int nFolderCount = 0;
  for (int i = 0; i < (int)m_items.size(); i++)
  {
    CFileItemPtr pItem = m_items[i];
    if (pItem->m_bIsFolder)
      nFolderCount++;
  }

  return nFolderCount;
}

int CFileItemList::GetObjectCount() const
{
  std::unique_lock<CCriticalSection> lock(m_lock);

  int numObjects = (int)m_items.size();
  if (numObjects && m_items[0]->IsParentFolder())
    numObjects--;

  return numObjects;
}

int CFileItemList::GetFileCount() const
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  int nFileCount = 0;
  for (int i = 0; i < (int)m_items.size(); i++)
  {
    CFileItemPtr pItem = m_items[i];
    if (!pItem->m_bIsFolder)
      nFileCount++;
  }

  return nFileCount;
}

int CFileItemList::GetSelectedCount() const
{
  return 0;
}

void CFileItemList::FilterCueItems()
{
}

// Remove the extensions from the filenames
void CFileItemList::RemoveExtensions()
{
  std::unique_lock<CCriticalSection> lock(m_lock);
  for (int i = 0; i < Size(); ++i)
    m_items[i]->RemoveExtension();
}

void CFileItemList::Stack(bool stackFiles /* = true */)
{
}

void CFileItemList::StackFolders()
{
}

void CFileItemList::StackFiles()
{
}

bool CFileItemList::Load(int windowID)
{
  return false;
}

bool CFileItemList::Save(int windowID)
{
  return false;
}

void CFileItemList::RemoveDiscCache(int windowID) const
{
  RemoveDiscCache(GetDiscFileCache(windowID));
}

void CFileItemList::RemoveDiscCache(const std::string& cacheFile) const
{
}

void CFileItemList::RemoveDiscCacheCRC(const std::string& crc) const
{
  std::string cachefile = StringUtils::Format("special://temp/archive_cache/{}.fi", crc);
  RemoveDiscCache(cachefile);
}

std::string CFileItemList::GetDiscFileCache(int windowID) const
{
  return "";
}

bool CFileItemList::AlwaysCache() const
{
  return false;
}

std::string CFileItem::GetUserMusicThumb(bool alwaysCheckRemote /* = false */, bool fallbackToFolder /* = false */) const
{
  // No thumb found
  return "";
}

// Gets the .tbn filename from a file or folder name.
// <filename>.ext -> <filename>.tbn
// <foldername>/ -> <foldername>.tbn
std::string CFileItem::GetTBNFile() const
{
  std::string thumbFile;
  std::string strFile = m_strPath;

  CURL url(strFile);
  strFile = url.GetFileName();

  if (m_bIsFolder && !IsFileFolder())
    URIUtils::RemoveSlashAtEnd(strFile);

  if (!strFile.empty())
  {
    if (m_bIsFolder && !IsFileFolder())
      thumbFile = strFile + ".tbn"; // folder, so just add ".tbn"
    else
      thumbFile = URIUtils::ReplaceExtension(strFile, ".tbn");
    url.SetFileName(thumbFile);
    thumbFile = url.Get();
  }
  return thumbFile;
}

bool CFileItem::SkipLocalArt() const
{
  return (m_strPath.empty()
       || StringUtils::StartsWithNoCase(m_strPath, "newsmartplaylist://")
       || StringUtils::StartsWithNoCase(m_strPath, "newplaylist://")
       || m_bIsShareOrDrive
       || IsInternetStream()
       || URIUtils::IsUPnP(m_strPath)
       || IsPlugin()
       || IsAddonsPath()
       || IsLibraryFolder()
       || IsParentFolder()
       || IsLiveTV()
       || IsPVRRecording()
       || IsDVD());
}

std::string CFileItem::GetThumbHideIfUnwatched(const CFileItem* item) const
{
  return "";
}

std::string CFileItem::FindLocalArt(const std::string &artFile, bool useFolder) const
{
  if (SkipLocalArt())
    return "";

  std::string thumb;
  if (!m_bIsFolder)
  {
    thumb = GetLocalArt(artFile, false);
    if (!thumb.empty() && CFile::Exists(thumb))
      return thumb;
  }
  if ((useFolder || (m_bIsFolder && !IsFileFolder())) && !artFile.empty())
  {
    std::string thumb2 = GetLocalArt(artFile, true);
    if (!thumb2.empty() && thumb2 != thumb && CFile::Exists(thumb2))
      return thumb2;
  }
  return "";
}

std::string CFileItem::GetLocalArtBaseFilename() const
{
  bool useFolder = false;
  return GetLocalArtBaseFilename(useFolder);
}

std::string CFileItem::GetLocalArtBaseFilename(bool& useFolder) const
{
  std::string strFile = m_strPath;

  if (URIUtils::IsInRAR(strFile) || URIUtils::IsInZIP(strFile))
  {
    std::string strPath = URIUtils::GetDirectory(strFile);
    std::string strParent;
    URIUtils::GetParentPath(strPath,strParent);
    strFile = URIUtils::AddFileToFolder(strParent, URIUtils::GetFileName(strFile));
  }

  if (IsOpticalMediaFile())
  { // optical media files should be treated like folders
    useFolder = true;
    strFile = GetLocalMetadataPath();
  }
  else if (useFolder && !(m_bIsFolder && !IsFileFolder()))
    strFile = URIUtils::GetDirectory(strFile);

  return strFile;
}

std::string CFileItem::GetLocalArt(const std::string& artFile, bool useFolder) const
{
  // no retrieving of empty art files from folders
  if (useFolder && artFile.empty())
    return "";

  std::string strFile = GetLocalArtBaseFilename(useFolder);
  if (strFile.empty()) // empty filepath -> nothing to find
    return "";

  if (useFolder)
  {
    if (!artFile.empty())
      return URIUtils::AddFileToFolder(strFile, artFile);
  }
  else
  {
    if (artFile.empty()) // old thumbnail matching
      return URIUtils::ReplaceExtension(strFile, ".tbn");
    else
      return URIUtils::ReplaceExtension(strFile, "-" + artFile);
  }
  return "";
}

std::string CFileItem::GetFolderThumb(const std::string &folderJPG /* = "folder.jpg" */) const
{
  std::string strFolder = m_strPath;

  if (IsStack() ||
      URIUtils::IsInRAR(strFolder) ||
      URIUtils::IsInZIP(strFolder))
  {
    URIUtils::GetParentPath(m_strPath,strFolder);
  }

  if (IsPlugin())
    return "";

  return URIUtils::AddFileToFolder(strFolder, folderJPG);
}

std::string CFileItem::GetMovieName(bool bUseFolderNames /* = false */) const
{
  std::string strMovieName;
  strMovieName = GetBaseMoviePath(bUseFolderNames);

  URIUtils::RemoveSlashAtEnd(strMovieName);

  return CURL::Decode(URIUtils::GetFileName(strMovieName));
}

std::string CFileItem::GetBaseMoviePath(bool bUseFolderNames) const
{
  std::string strMovieName = m_strPath;

  if (IsOpticalMediaFile())
    return GetLocalMetadataPath();

  if (bUseFolderNames &&
     (!m_bIsFolder || URIUtils::IsInArchive(m_strPath)))
  {
    std::string name2(strMovieName);
    URIUtils::GetParentPath(name2,strMovieName);
    if (URIUtils::IsInArchive(m_strPath))
    {
      // Try to get archive itself, if empty take path before
      name2 = CURL(m_strPath).GetHostName();
      if (name2.empty())
        name2 = strMovieName;

      URIUtils::GetParentPath(name2, strMovieName);
    }
  }

  return strMovieName;
}

std::string CFileItem::GetLocalFanart() const
{
  return "";
}

std::string CFileItem::GetLocalMetadataPath() const
{
  if (m_bIsFolder && !IsFileFolder())
    return m_strPath;

  std::string parent(URIUtils::GetParentPath(m_strPath));
  std::string parentFolder(parent);
  URIUtils::RemoveSlashAtEnd(parentFolder);
  parentFolder = URIUtils::GetFileName(parentFolder);
  if (StringUtils::EqualsNoCase(parentFolder, "VIDEO_TS") || StringUtils::EqualsNoCase(parentFolder, "BDMV"))
  { // go back up another one
    parent = URIUtils::GetParentPath(parent);
  }
  return parent;
}

bool CFileItem::LoadMusicTag()
{
  return false;
}

bool CFileItem::LoadGameTag()
{
  return false;
}

bool CFileItem::LoadDetails()
{
  return false;
}

void CFileItemList::Swap(unsigned int item1, unsigned int item2)
{
  if (item1 != item2 && item1 < m_items.size() && item2 < m_items.size())
    std::swap(m_items[item1], m_items[item2]);
}

bool CFileItemList::UpdateItem(const CFileItem *item)
{
  if (!item)
    return false;

  std::unique_lock<CCriticalSection> lock(m_lock);
  for (unsigned int i = 0; i < m_items.size(); i++)
  {
    CFileItemPtr pItem = m_items[i];
    if (pItem->IsSamePath(item))
    {
      pItem->UpdateInfo(*item);
      return true;
    }
  }
  return false;
}

void CFileItemList::SetReplaceListing(bool replace)
{
  m_replaceListing = replace;
}

bool CFileItem::HasVideoInfoTag() const
{
  return false;
}

CVideoInfoTag* CFileItem::GetVideoInfoTag()
{
  if (!m_videoInfoTag)
    m_videoInfoTag = new CVideoInfoTag;

  return m_videoInfoTag;
}

const CVideoInfoTag* CFileItem::GetVideoInfoTag() const
{
  return m_videoInfoTag;
}

CPictureInfoTag* CFileItem::GetPictureInfoTag()
{
  if (!m_pictureInfoTag)
    m_pictureInfoTag = new CPictureInfoTag;

  return m_pictureInfoTag;
}

MUSIC_INFO::CMusicInfoTag* CFileItem::GetMusicInfoTag()
{
  if (!m_musicInfoTag)
    m_musicInfoTag = new MUSIC_INFO::CMusicInfoTag;

  return m_musicInfoTag;
}

bool CFileItem::HasPVRChannelInfoTag() const
{
  return false;
}

std::string CFileItem::FindTrailer() const
{
  return "";
}

VideoDbContentType CFileItem::GetVideoContentType() const
{
  return VideoDbContentType::MOVIES;
}

CFileItem CFileItem::GetItemToPlay() const
{
  return *this;
}

bool CFileItem::IsResumePointSet() const
{
  return false;
}

double CFileItem::GetCurrentResumeTime() const
{
  return 0;
}

bool CFileItem::GetCurrentResumeTimeAndPartNumber(int64_t& startOffset, int& partNumber) const
{
  return false;
}

bool CFileItem::IsResumable() const
{
  return (!IsNFO() && !IsPlayList()) || IsType(".strm");
}
