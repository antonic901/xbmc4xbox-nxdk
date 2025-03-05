/*
 *  Copyright (C) 2010-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "FileUtils.h"

#include "FileOperationJob.h"
#include "URIUtils.h"
#include "filesystem/File.h"

using namespace XFILE;

bool CFileUtils::DeleteItem(const std::string &strPath)
{
  CFileItemPtr item(new CFileItem(strPath));
  item->SetPath(strPath);
  item->m_bIsFolder = URIUtils::HasSlashAtEnd(strPath);
  item->Select(true);
  return DeleteItem(item);
}

bool CFileUtils::DeleteItem(const std::shared_ptr<CFileItem>& item)
{
  if (!item || item->IsParentFolder())
    return false;

  // Create a temporary item list containing the file/folder for deletion
  CFileItemPtr pItemTemp(new CFileItem(*item));
  pItemTemp->Select(true);
  CFileItemList items;
  items.Add(pItemTemp);

  // grab the real filemanager window, set up the progress bar,
  // and process the delete action
  CFileOperationJob op(CFileOperationJob::ActionDelete, items, "");

  return op.DoWork();
}

bool CFileUtils::Exists(const std::string& strFileName, bool bUseCache)
{
  return CFile::Exists(strFileName, bUseCache);
}
