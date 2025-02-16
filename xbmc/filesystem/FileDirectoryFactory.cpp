/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "FileDirectoryFactory.h"

#include "Directory.h"
#include "FileItem.h"
#include "ServiceBroker.h"
#include "URL.h"
#include "utils/URIUtils.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

using namespace XFILE;

CFileDirectoryFactory::CFileDirectoryFactory(void) = default;

CFileDirectoryFactory::~CFileDirectoryFactory(void) = default;

// return NULL + set pItem->m_bIsFolder to remove it completely from list.
IFileDirectory* CFileDirectoryFactory::Create(const CURL& url, CFileItem* pItem, const std::string& strMask)
{
  if (url.IsProtocol("stack")) // disqualify stack as we need to work with each of the parts instead
    return NULL;

  /**
   * Check available binary addons which can contain files with underlaid
   * folders / files.
   * Currently in vfs and audiodecoder addons.
   *
   * @note The file extensions are absolutely necessary for these in order to
   * identify the associated add-on.
   */
  /**@{*/

  // Get file extensions to find addon related to it.
  std::string strExtension = URIUtils::GetExtension(url);
  StringUtils::ToLower(strExtension);

  return NULL;
}

