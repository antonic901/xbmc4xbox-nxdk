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

class CAdvancedSettings
{
  public:
    CAdvancedSettings();

    static CAdvancedSettings* getInstance();

    typedef std::vector< std::pair<std::string, std::string> > StringMapping;
    StringMapping m_pathSubstitutions;

    // runtime settings which cannot be set from advancedsettings.xml
    std::string m_videoExtensions;
    std::string m_discStubExtensions;
    std::string m_subtitlesExtensions;
    std::string m_musicExtensions;
    std::string m_pictureExtensions;
};

XBMC_GLOBAL(CAdvancedSettings,g_advancedSettings);
