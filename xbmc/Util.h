/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <climits>
#include <cmath>
#include <stdint.h>
#include <string.h>
#include <vector>

#define ARRAY_SIZE(X)         (sizeof(X)/sizeof((X)[0]))

class CFileItem;
class CFileItemList;

class CUtil
{
  CUtil() = delete;
public:
  static std::string ValidatePath(const std::string &path, bool bFixDoubleSlashes = false); ///< return a validated path, with correct directory separators.

  static std::string GetNextFilename(const std::string &fn_template, int max);

};


