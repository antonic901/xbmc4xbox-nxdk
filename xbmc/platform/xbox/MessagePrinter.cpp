/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "platform/MessagePrinter.h"

#include <hal/debug.h>

void CMessagePrinter::DisplayMessage(const std::string& message)
{
  debugPrint("%s", message.c_str());
}

void CMessagePrinter::DisplayWarning(const std::string& warning)
{
  debugPrint("%s", warning.c_str());
}

void CMessagePrinter::DisplayError(const std::string& error)
{
  debugPrint("%s", error.c_str());
}

void CMessagePrinter::DisplayHelpMessage(const std::vector<std::pair<std::string, std::string>>& help)
{
  //very crude implementation, pretty it up when possible
  std::string message;
  for (const auto& line : help)
  {
    message.append(line.first + "\t" + line.second + "\r\n");
  }
  debugPrint("%s", message.c_str());
}
