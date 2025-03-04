/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "FileExtensionProvider.h"

#include "ServiceBroker.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/URIUtils.h"

#include <string>
#include <vector>

CFileExtensionProvider::CFileExtensionProvider()
  : m_advancedSettings(CServiceBroker::GetSettingsComponent()->GetAdvancedSettings())
{

}

CFileExtensionProvider::~CFileExtensionProvider()
{
  m_advancedSettings.reset();
}

std::string CFileExtensionProvider::GetDiscStubExtensions() const
{
  return m_advancedSettings->m_discStubExtensions;
}

std::string CFileExtensionProvider::GetMusicExtensions() const
{
  std::string extensions(m_advancedSettings->m_musicExtensions);

  return extensions;
}

std::string CFileExtensionProvider::GetPictureExtensions() const
{
  std::string extensions(m_advancedSettings->m_pictureExtensions);

  return extensions;
}

std::string CFileExtensionProvider::GetSubtitleExtensions() const
{
  std::string extensions(m_advancedSettings->m_subtitlesExtensions);

  return extensions;
}

std::string CFileExtensionProvider::GetVideoExtensions() const
{
  std::string extensions(m_advancedSettings->m_videoExtensions);

  return extensions;
}

std::string CFileExtensionProvider::GetFileFolderExtensions() const
{
  return "";
}

bool CFileExtensionProvider::CanOperateExtension(const std::string& path) const
{
  /*!
   * If no file extensions present, mark it as not supported.
   */
  return false;
}
