/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "platform/Filesystem.h"

#include "utils/StringUtils.h"

#include "platform/win32/CharsetConverter.h"

#include <windows.h>

namespace win = KODI::PLATFORM::WINDOWS;

namespace KODI
{
namespace PLATFORM
{
namespace FILESYSTEM
{
space_info space(const std::string& path, std::error_code& ec)
{

  ec.clear();
  space_info sp;
#if NXDK
  auto pathW = path;
#else
  auto pathW = win::ToW(path);
#endif

  ULARGE_INTEGER capacity;
  ULARGE_INTEGER available;
  ULARGE_INTEGER free;
#ifdef NXDK
  auto result = GetDiskFreeSpaceEx(pathW.c_str(), &available, &capacity, &free);
#else
  auto result = GetDiskFreeSpaceExW(pathW.c_str(), &available, &capacity, &free);
#endif

  if (result == FALSE)
  {
    ec.assign(GetLastError(), std::system_category());
    sp.available = static_cast<uintmax_t>(-1);
    sp.capacity = static_cast<uintmax_t>(-1);
    sp.free = static_cast<uintmax_t>(-1);
    return sp;
  }

  sp.available = static_cast<uintmax_t>(available.QuadPart);
  sp.capacity = static_cast<uintmax_t>(capacity.QuadPart);
  sp.free = static_cast<uintmax_t>(free.QuadPart);

  return sp;
}
} // namespace FILESYSTEM
} // namespace PLATFORM
} // namespace KODI
