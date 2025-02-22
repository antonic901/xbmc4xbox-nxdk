/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "URL.h"

#include <vector>

class CXBOXUtil
{
public:
  CXBOXUtil(void);
  virtual ~CXBOXUtil(void);

  static bool ShortenFileName(std::string& strFileNameAndPath);
  static void RemoveIllegalChars(std::string& strText);
  static std::string GetFatXQualifiedPath(const std::string& strPath);
  static std::string GetFatXQualifiedPath(const CURL& url);
};
