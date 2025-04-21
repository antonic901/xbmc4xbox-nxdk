/*
 *  Copyright (C) 2023-2025 Team XBMC
 *  This file is part of XBMC - https://xbmc.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "platform/xbmc.h"

#include <hal/video.h>

int main()
{
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

  int status = XBMC_Run(true);
  
  return status;
}
