/*
 *  Copyright (C) 2014-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "Win32Directory.h"

#include "FileItem.h"
#include "URL.h"
#include "utils/CharsetConverter.h"
#ifndef NXDK
#include "utils/SystemInfo.h"
#endif
#include "utils/XTimeUtils.h"
#include "utils/log.h"

#ifndef NXDK
#include "platform/win32/WIN32Util.h"
#endif

#include <windows.h>

using namespace XFILE;

// check for empty string, remove trailing slash if any, convert to win32 form
#ifdef NXDK
inline static std::string prepareWin32DirectoryName(const std::string& strPath)
#else
inline static std::wstring prepareWin32DirectoryName(const std::string& strPath)
#endif
{
  if (strPath.empty())
#ifdef NXDK
    return std::string(); // empty string
#else
    return std::wstring(); // empty string
#endif

#ifdef NXDK
  std::string nameW(strPath);
#else
  std::wstring nameW(CWIN32Util::ConvertPathToWin32Form(strPath));
#endif
  if (!nameW.empty())
  {
    if (nameW.back() == L'\\')
      nameW.pop_back(); // remove slash at the end if any
    if (nameW.length() == 6 && nameW.back() == L':') // 6 is the length of "\\?\x:"
      nameW.push_back(L'\\'); // always add backslash for root folders
  }
  return nameW;
}

CWin32Directory::CWin32Directory(void)
{}

CWin32Directory::~CWin32Directory(void)
{}

bool CWin32Directory::GetDirectory(const CURL& url, CFileItemList &items)
{
  std::string pathWithSlash(url.Get());
  if (!pathWithSlash.empty() && pathWithSlash.back() != '\\')
    pathWithSlash.push_back('\\');

#ifdef NXDK
  std::string searchMask(pathWithSlash);
#else
  std::wstring searchMask(CWIN32Util::ConvertPathToWin32Form(pathWithSlash));
#endif
  if (searchMask.empty())
    return false;

  //! @todo support m_strFileMask, require rewrite of internal caching
  searchMask += L'*';

  HANDLE hSearch;
#ifdef NXDK
  WIN32_FIND_DATA findData = {};
#else
  WIN32_FIND_DATAW findData = {};
#endif

#ifdef NXDK
  hSearch = FindFirstFileA(searchMask.c_str(), &findData);
#else
  hSearch = FindFirstFileExW(searchMask.c_str(), FindExInfoBasic, &findData, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
#endif

  if (hSearch == INVALID_HANDLE_VALUE)
    return GetLastError() == ERROR_FILE_NOT_FOUND ? Exists(url) : false; // return true if directory exist and empty

  do
  {
#ifdef NXDK
    std::string itemNameW(findData.cFileName);
    if (itemNameW == "." || itemNameW == "..")
#else
    std::wstring itemNameW(findData.cFileName);
    if (itemNameW == L"." || itemNameW == L"..")
#endif
      continue;

    std::string itemName;
#ifdef NXDK
    itemName = itemNameW;
#else
    if (!g_charsetConverter.wToUTF8(itemNameW, itemName, true) || itemName.empty())
    {
      CLog::Log(LOGERROR, "{}: Can't convert wide string name to UTF-8 encoding", __FUNCTION__);
      continue;
    }
#endif

    CFileItemPtr pItem(new CFileItem(itemName));

    pItem->m_bIsFolder = ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    if (pItem->m_bIsFolder)
      pItem->SetPath(pathWithSlash + itemName + '\\');
    else
      pItem->SetPath(pathWithSlash + itemName);

    if ((findData.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) != 0
          || itemName.front() == '.') // mark files starting from dot as hidden
      pItem->SetProperty("file:hidden", true);

    // calculation of size and date costs a little on win32
    // so DIR_FLAG_NO_FILE_INFO flag is ignored
    KODI::TIME::FileTime fileTime;
    fileTime.lowDateTime = findData.ftLastWriteTime.dwLowDateTime;
    fileTime.highDateTime = findData.ftLastWriteTime.dwHighDateTime;
    KODI::TIME::FileTime localTime;
    if (KODI::TIME::FileTimeToLocalFileTime(&fileTime, &localTime) == TRUE)
      pItem->m_dateTime = localTime;
    else
      pItem->m_dateTime = 0;

    if (!pItem->m_bIsFolder)
        pItem->m_dwSize = (__int64(findData.nFileSizeHigh) << 32) + findData.nFileSizeLow;

    items.Add(pItem);
#ifdef NXDK
  } while (FindNextFileA(hSearch, &findData));
#else
  } while (FindNextFileW(hSearch, &findData));
#endif

  FindClose(hSearch);

  return true;
}

bool CWin32Directory::Create(const CURL& url)
{
  auto nameW(prepareWin32DirectoryName(url.Get()));
  if (nameW.empty())
    return false;

  if (!Create(nameW))
    return Exists(url);

  return true;
}

bool CWin32Directory::Exists(const CURL& url)
{
#ifdef NXDK
  std::string nameW(url.Get());
#else
  std::wstring nameW(prepareWin32DirectoryName(url.Get()));
#endif
  if (nameW.empty())
    return false;

#ifdef NXDK
  DWORD fileAttrs = GetFileAttributesA(nameW.c_str());
#else
  DWORD fileAttrs = GetFileAttributesW(nameW.c_str());
#endif
  if (fileAttrs == INVALID_FILE_ATTRIBUTES || (fileAttrs & FILE_ATTRIBUTE_DIRECTORY) == 0)
    return false;

  return true;
}

bool CWin32Directory::Remove(const CURL& url)
{
#ifdef NXDK
  std::string nameW(prepareWin32DirectoryName(url.Get()));
#else
  std::wstring nameW(prepareWin32DirectoryName(url.Get()));
#endif
  if (nameW.empty())
    return false;

#ifdef NXDK
  if (RemoveDirectoryA(nameW.c_str()))
#else
  if (RemoveDirectoryW(nameW.c_str()))
#endif
    return true;

  return !Exists(url);
}

bool CWin32Directory::RemoveRecursive(const CURL& url)
{
  std::string pathWithSlash(url.Get());
  if (!pathWithSlash.empty() && pathWithSlash.back() != '\\')
    pathWithSlash.push_back('\\');

#ifdef NXDK
  auto basePath = pathWithSlash;
#else
  auto basePath = CWIN32Util::ConvertPathToWin32Form(pathWithSlash);
#endif
  if (basePath.empty())
    return false;

#ifdef NXDK
  auto searchMask = basePath + '*';
#else
  auto searchMask = basePath + L'*';
#endif

  HANDLE hSearch;
#ifdef NXDK
  WIN32_FIND_DATA findData = {};
#else
  WIN32_FIND_DATAW findData = {};
#endif

#ifdef NXDK
  hSearch = FindFirstFileA(searchMask.c_str(), &findData);
#else
  if (g_sysinfo.IsWindowsVersionAtLeast(CSysInfo::WindowsVersionWin7))
    hSearch = FindFirstFileExW(searchMask.c_str(), FindExInfoBasic, &findData, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
  else
    hSearch = FindFirstFileExW(searchMask.c_str(), FindExInfoStandard, &findData, FindExSearchNameMatch, nullptr, 0);
#endif

  if (hSearch == INVALID_HANDLE_VALUE)
    return GetLastError() == ERROR_FILE_NOT_FOUND ? Exists(url) : false; // return true if directory exist and empty

  bool success = true;
  do
  {
#ifdef NXDK
    std::string itemNameW(findData.cFileName);
    if (itemNameW == "." || itemNameW == "..")
#else
    std::wstring itemNameW(findData.cFileName);
    if (itemNameW == L"." || itemNameW == L"..")
#endif
      continue;

    auto pathW = basePath + itemNameW;
    if (0 != (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
      std::string path;
#ifdef NXDK
      path = pathW;
#else
      if (!g_charsetConverter.wToUTF8(pathW, path, true))
      {
        CLog::Log(LOGERROR, "{}: Can't convert wide string name to UTF-8 encoding", __FUNCTION__);
        continue;
      }
#endif

      if (!RemoveRecursive(CURL{ path }))
      {
        success = false;
        break;
      }
    }
    else
    {
#ifdef NXDK
      if (FALSE == DeleteFileA(pathW.c_str()))
#else
      if (FALSE == DeleteFileW(pathW.c_str()))
#endif
      {
        success = false;
        break;
      }
    }
#ifdef NXDK
  } while (FindNextFileA(hSearch, &findData));
#else
  } while (FindNextFileW(hSearch, &findData));
#endif

  FindClose(hSearch);

  if (success)
  {
#ifdef NXDK
    if (FALSE == RemoveDirectoryA(basePath.c_str()))
#else
    if (FALSE == RemoveDirectoryW(basePath.c_str()))
#endif
      success = false;
  }

  return success;
}

#ifdef NXDK
bool CWin32Directory::Create(std::string path) const
{
  if (!CreateDirectoryA(path.c_str(), nullptr))
#else
bool CWin32Directory::Create(std::wstring path) const
{
  if (!CreateDirectoryW(path.c_str(), nullptr))
#endif
  {
    if (GetLastError() == ERROR_ALREADY_EXISTS)
      return true;

    if (GetLastError() != ERROR_PATH_NOT_FOUND)
      return false;

    auto sep = path.rfind(L'\\');
    if (sep == std::wstring::npos)
      return false;

    if (Create(path.substr(0, sep)))
      return Create(path);

    return false;
  }

  // if directory name starts from dot, make it hidden
  const auto lastSlashPos = path.rfind(L'\\');
  if (lastSlashPos < path.length() - 1 && path[lastSlashPos + 1] == L'.')
  {
#ifdef NXDK
    DWORD dirAttrs = GetFileAttributesA(path.c_str());
    if (dirAttrs != INVALID_FILE_ATTRIBUTES && SetFileAttributes(path.c_str(), dirAttrs | FILE_ATTRIBUTE_HIDDEN))
#else
    DWORD dirAttrs = GetFileAttributesW(path.c_str());
    if (dirAttrs != INVALID_FILE_ATTRIBUTES && SetFileAttributesW(path.c_str(), dirAttrs | FILE_ATTRIBUTE_HIDDEN))
#endif
      return true;
  }

  return true;
}
