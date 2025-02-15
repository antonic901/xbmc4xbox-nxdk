/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

// Guarantee that CSpecialProtocol is initialized before and uninitialized after ZipManager
#include "filesystem/SpecialProtocol.h"
std::map<std::string, std::string> CSpecialProtocol::m_pathMap;
