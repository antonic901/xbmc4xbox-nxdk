/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <memory>

namespace ADDON
{
class CAddonMgr;
} // namespace ADDON

class CFileExtensionProvider;

class CServiceManager
{
public:
  CServiceManager();
  ~CServiceManager();

  bool InitForTesting();
  bool InitStageOne();
  bool InitStageTwo(const std::string& profilesUserDataFolder);
  bool InitStageThree();
  void DeinitTesting();
  void DeinitStageThree();
  void DeinitStageTwo();
  void DeinitStageOne();

  ADDON::CAddonMgr& GetAddonMgr();

  int init_level = 0;

  CFileExtensionProvider& GetFileExtensionProvider();

protected:
  std::unique_ptr<ADDON::CAddonMgr> m_addonMgr;
  std::unique_ptr<CFileExtensionProvider> m_fileExtensionProvider;
};
