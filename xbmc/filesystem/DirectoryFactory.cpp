/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include <stdlib.h>
#include "DirectoryFactory.h"
#include "utils/log.h"

#include "URL.h"
#include "utils/StringUtils.h"

using namespace XFILE;

/*!
 \brief Create a IDirectory object of the share type specified in \e strPath .
 \param strPath Specifies the share type to access, can be a share or share with path.
 \return IDirectory object to access the directories on the share.
 \sa IDirectory
 */
IDirectory* CDirectoryFactory::Create(const CURL& url)
{
  CLog::Log(LOGWARNING, "{} - unsupported protocol({}) in {}", __FUNCTION__, url.GetProtocol(),
            url.GetRedacted());
  return NULL;
}

