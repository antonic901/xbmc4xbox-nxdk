/*
 *  Copyright (C) 2010-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "FileUtils.h"

#include "filesystem/File.h"

using namespace XFILE;

bool CFileUtils::Exists(const std::string& strFileName, bool bUseCache)
{
  return CFile::Exists(strFileName, bUseCache);
}
