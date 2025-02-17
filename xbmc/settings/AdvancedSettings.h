/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <set>
#include <string>
#include <vector>

#include "utils/GlobalsHandling.h"

#define CACHE_BUFFER_MODE_INTERNET 0
#define CACHE_BUFFER_MODE_ALL 1
#define CACHE_BUFFER_MODE_TRUE_INTERNET 2
#define CACHE_BUFFER_MODE_NONE 3
#define CACHE_BUFFER_MODE_NETWORK 4

class CAdvancedSettings
{
  public:
    CAdvancedSettings();

    static CAdvancedSettings* getInstance();

    typedef std::vector< std::pair<std::string, std::string> > StringMapping;
    StringMapping m_pathSubstitutions;

    unsigned int m_cacheMemSize;
    unsigned int m_cacheBufferMode;
    unsigned int m_cacheChunkSize;
    float m_cacheReadFactor;

    // runtime settings which cannot be set from advancedsettings.xml
    std::string m_videoExtensions;
    std::string m_discStubExtensions;
    std::string m_subtitlesExtensions;
    std::string m_musicExtensions;
    std::string m_pictureExtensions;
};

XBMC_GLOBAL(CAdvancedSettings,g_advancedSettings);
