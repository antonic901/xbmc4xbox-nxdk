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

class DatabaseSettings
{
public:
  DatabaseSettings() { Reset(); }
  void Reset()
  {
    type.clear();
    host.clear();
    port.clear();
    user.clear();
    pass.clear();
    name.clear();
    key.clear();
    cert.clear();
    ca.clear();
    capath.clear();
    ciphers.clear();
    compression = false;
  };
  std::string type;
  std::string host;
  std::string port;
  std::string user;
  std::string pass;
  std::string name;
  std::string key;
  std::string cert;
  std::string ca;
  std::string capath;
  std::string ciphers;
  bool compression;
};

class CAdvancedSettings
{
  public:
    CAdvancedSettings();

    static CAdvancedSettings* getInstance();

    bool m_videoUseTimeSeeking;
    int m_videoTimeSeekForward;
    int m_videoTimeSeekBackward;
    int m_videoTimeSeekForwardBig;
    int m_videoTimeSeekBackwardBig;
    int m_videoPercentSeekForward;
    int m_videoPercentSeekForwardBig;
    int m_videoPercentSeekBackward;
    int m_videoPercentSeekBackwardBig;

    typedef std::vector< std::pair<std::string, std::string> > StringMapping;
    StringMapping m_pathSubstitutions;

    bool m_guiVisualizeDirtyRegions;
    int  m_guiAlgorithmDirtyRegions;

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
