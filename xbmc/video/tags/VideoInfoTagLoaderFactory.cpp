/*
 *  Copyright (C) 2017-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VideoInfoTagLoaderFactory.h"

#include "FileItem.h"
#include "ServiceBroker.h"
#include "VideoTagLoaderNFO.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"

using namespace VIDEO;

IVideoInfoTagLoader* CVideoInfoTagLoaderFactory::CreateLoader(const CFileItem& item,
                                                              const ADDON::ScraperPtr& info,
                                                              bool lookInFolder,
                                                              bool forceRefresh)
{
  CVideoTagLoaderNFO* nfo = new CVideoTagLoaderNFO(item, info, lookInFolder);
  if (nfo->HasInfo())
    return nfo;
  delete nfo;

  return nullptr;
}
