/*
 *  Copyright (c) 2002 Frodo
 *      Portions Copyright (c) by the authors of ffmpeg and xvid
 *  Copyright (C) 2002-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "File.h"

#include "IFile.h"
#include "commons/Exception.h"
#include "settings/AdvancedSettings.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

using namespace XFILE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifndef __GNUC__
#pragma warning (disable:4244)
#endif

//*********************************************************************************************
CFile::CFile() = default;

//*********************************************************************************************
CFile::~CFile()
{
  Close();
}

bool CFile::Exists(const std::string& strFileName, bool bUseCache /* = true */)
{
  const CURL pathToUrl(strFileName);
  return Exists(pathToUrl, bUseCache);
}

bool CFile::Exists(const CURL& file, bool bUseCache /* = true */)
{
  return false;
}

int CFile::Stat(struct __stat64 *buffer)
{
  if (!buffer)
    return -1;

  if (!m_pFile)
  {
    *buffer = {};
    errno = ENOENT;
    return -1;
  }

  return m_pFile->Stat(buffer);
}

int CFile::Stat(const std::string& strFileName, struct __stat64* buffer)
{
  const CURL pathToUrl(strFileName);
  return Stat(pathToUrl, buffer);
}

int CFile::Stat(const CURL& file, struct __stat64* buffer)
{
  // TODO: implement this
  return -1;
}

//*********************************************************************************************
void CFile::Close()
{
  try
  {
    if (m_pFile)
      m_pFile->Close();

    m_pFile.reset();
  }
  XBMCCOMMONS_HANDLE_UNCHECKED
  catch (...) { CLog::Log(LOGERROR, "%s - Unhandled exception", __FUNCTION__); }
}
