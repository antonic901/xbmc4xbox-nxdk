/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "FileFactory.h"
#include "URL.h"
#include "utils/log.h"
#include "utils/StringUtils.h"

using namespace XFILE;

CFileFactory::CFileFactory() = default;

CFileFactory::~CFileFactory() = default;

IFile* CFileFactory::CreateLoader(const std::string& strFileName)
{
  CURL url(strFileName);
  return CreateLoader(url);
}

IFile* CFileFactory::CreateLoader(const CURL& url)
{
  CLog::Log(LOGWARNING, "%s - unsupported protocol(%s) in %s", __FUNCTION__, url.GetProtocol().c_str(),
            url.GetRedacted().c_str());
  return NULL;
}
