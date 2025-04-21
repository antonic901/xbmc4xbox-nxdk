/*
 *  Copyright (C) 2023-2025 Team XBMC
 *  This file is part of XBMC - https://xbmc.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "platform/xbmc.h"

int main()
{
  int status = XBMC_Run(true);
  
  return status;
}
