/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "Exception.h"

namespace XbmcCommons
{
  Exception::~Exception() = default;

  void Exception::LogThrowMessage(const char* prefix) const
  {
  }
}

