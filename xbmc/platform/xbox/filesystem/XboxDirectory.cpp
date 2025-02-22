/*
 *  Copyright (C) 2014-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "XboxDirectory.h"

#include "FileItem.h"
#include "URL.h"
#include "utils/CharsetConverter.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

#include "platform/xbox/XBOXUtil.h"

#include <nxdk/mount.h>

#include <windows.h>

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD) -1)
#endif

using namespace XFILE;

CXboxDirectory::CXboxDirectory(void)
{}

CXboxDirectory::~CXboxDirectory(void)
{}

bool CXboxDirectory::GetDirectory(const CURL& url, CFileItemList &items)
{
  WIN32_FIND_DATA wfd;

  std::string strPath=url.Get();
  g_charsetConverter.utf8ToStringCharset(strPath);

  std::string strRoot = strPath;

  memset(&wfd, 0, sizeof(wfd));
  URIUtils::AddSlashAtEnd(strRoot);
  StringUtils::Replace(strRoot, "/", "\\");
  if (URIUtils::IsDVD(strRoot))
  {
    nxUnmountDrive('D');
    nxMountDrive('D', "\\Device\\Cdrom0");
  }

  std::string strSearchMask = strRoot;
  strSearchMask += "*.*";

  KODI::TIME::FileTime localTime;
  HANDLE hFind = FindFirstFile(strSearchMask.c_str(), &wfd);

  // on error, check if path exists at all, this will return true if empty folder
  if (hFind == INVALID_HANDLE_VALUE)
    return Exists(url);

  do
  {
    if (wfd.cFileName[0] != 0)
    {
      if ( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
      {
        std::string strDir = wfd.cFileName;
        if (strDir != "." && strDir != "..")
        {
          std::string strLabel=wfd.cFileName;
          g_charsetConverter.unknownToUTF8(strLabel);
          CFileItemPtr pItem(new CFileItem(strLabel));
          std::string itemPath = strRoot + wfd.cFileName;
          g_charsetConverter.unknownToUTF8(itemPath);
          pItem->m_bIsFolder = true;
          URIUtils::AddSlashAtEnd(itemPath);
          pItem->SetPath(itemPath);
          KODI::TIME::FileTime fileTime;
          fileTime.lowDateTime = wfd.ftLastWriteTime.dwLowDateTime;
          fileTime.highDateTime = wfd.ftLastWriteTime.dwHighDateTime;
          if (KODI::TIME::FileTimeToLocalFileTime(&fileTime, &localTime) == TRUE)
            pItem->m_dateTime=localTime;

          items.Add(pItem);
        }
      }
      else
      {
        std::string strLabel=wfd.cFileName;
        g_charsetConverter.unknownToUTF8(strLabel);
        CFileItemPtr pItem(new CFileItem(strLabel));
        std::string itemPath = strRoot + wfd.cFileName;
        g_charsetConverter.unknownToUTF8(itemPath);
        pItem->SetPath(itemPath);
        pItem->m_bIsFolder = false;
        pItem->m_dwSize = (__int64(wfd.nFileSizeHigh) << 32) + wfd.nFileSizeLow;
        KODI::TIME::FileTime fileTime;
        fileTime.lowDateTime = wfd.ftLastWriteTime.dwLowDateTime;
        fileTime.highDateTime = wfd.ftLastWriteTime.dwHighDateTime;
        if (KODI::TIME::FileTimeToLocalFileTime(&fileTime, &localTime) == TRUE)
          pItem->m_dateTime=localTime;

        items.Add(pItem);
      }
    }
  }
  while (FindNextFile((HANDLE)hFind, &wfd));

  FindClose(hFind);

  return true;
}

bool CXboxDirectory::Create(const CURL& url)
{
  std::string strPath1 = url.Get();
  g_charsetConverter.utf8ToStringCharset(strPath1);
  URIUtils::AddSlashAtEnd(strPath1);

#if 0
  // okey this is really evil, since the create will succeed
  // the caller will have no idea that a different directory was created
  if (CSettings::GetInstance().GetBool("services.ftpautofatx"))
  {
    std::string strPath2(strPath1);
    CXBOXUtil::GetFatXQualifiedPath(strPath1);
    if(strPath2 != strPath1)
      CLog::Log(LOGNOTICE,"fatxq: %s -> %s",strPath2.c_str(), strPath1.c_str());
  }
#endif

  if(::CreateDirectory(strPath1.c_str(), NULL))
    return true;
  else if(GetLastError() == ERROR_ALREADY_EXISTS)
    return true;

  return false;
}

bool CXboxDirectory::Remove(const CURL& url)
{
  std::string strPath1 = url.Get();
  g_charsetConverter.utf8ToStringCharset(strPath1);
  return (::RemoveDirectory(strPath1.c_str()) || GetLastError() == ERROR_PATH_NOT_FOUND) ? true : false;
}

bool CXboxDirectory::Exists(const CURL& url)
{
  std::string strReplaced=url.Get();
  g_charsetConverter.utf8ToStringCharset(strReplaced);
  StringUtils::Replace(strReplaced, "/", "\\");
  CXBOXUtil::GetFatXQualifiedPath(strReplaced);
  URIUtils::AddSlashAtEnd(strReplaced);
  DWORD attributes = GetFileAttributes(strReplaced.c_str());
  if(attributes == INVALID_FILE_ATTRIBUTES)
    return false;
  if (FILE_ATTRIBUTE_DIRECTORY & attributes) return true;
  return false;
}

bool CXboxDirectory::RemoveRecursive(const CURL& url)
{
  std::string pathWithSlash(url.Get());
  if (!pathWithSlash.empty() && pathWithSlash.at(pathWithSlash.size() - 1) != '\\')
    pathWithSlash.push_back('\\');

  // Do we need this here? If we get to this point we are already in Q:\\path\to\file format
  std::string basePath = pathWithSlash/*CWIN32Util::ConvertPathToWin32Form(pathWithSlash)*/;
  if (basePath.empty())
    return false;

  std::string searchMask = basePath + '*';

  HANDLE hSearch;
  WIN32_FIND_DATA findData = {};

  hSearch = FindFirstFile(searchMask.c_str(), &findData);

  if (hSearch == INVALID_HANDLE_VALUE)
    return GetLastError() == ERROR_FILE_NOT_FOUND ? Exists(url) : false; // return true if directory exist and empty

  do
  {
    std::string itemName(findData.cFileName);
    if (itemName == "." || itemName == "..")
      continue;

    std::string path = basePath + itemName;
    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      CURL url(path);
      if (!RemoveRecursive(url))
        return false;

      if (FALSE == RemoveDirectory(path.c_str()))
        return false;
    }
    else
    {
      if (FALSE == DeleteFile(path.c_str()))
        return false;
    }
  } while (FindNextFile(hSearch, &findData));

  FindClose(hSearch);

  return true;
}
