/*
 *  Copyright (C) 2015-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GUIWindowSplash.h"

#include "Util.h"
#include "guilib/GUIImage.h"
#include "guilib/GUIWindowManager.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"

CGUIWindowSplash::CGUIWindowSplash(void) : CGUIWindow(WINDOW_SPLASH, ""), m_image(nullptr)
{
  m_loadType = LOAD_ON_GUI_INIT;
}

CGUIWindowSplash::~CGUIWindowSplash(void) = default;

void CGUIWindowSplash::OnInitWindow()
{
  if (!CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_splashImage)
    return;

  m_image = std::unique_ptr<CGUIImage>(new CGUIImage(0, 0, 0, 0, g_graphicsContext.GetWidth(), g_graphicsContext.GetHeight(), CTextureInfo(CUtil::GetSplashPath())));
  m_image->SetAspectRatio(CAspectRatio::AR_SCALE);
}

void CGUIWindowSplash::Render()
{
  g_graphicsContext.SetRenderingResolution(g_graphicsContext.GetResInfo(), true);

  if (!m_image)
    return;

  m_image->SetWidth(g_graphicsContext.GetWidth());
  m_image->SetHeight(g_graphicsContext.GetHeight());
  m_image->AllocResources();
  m_image->Render();
  m_image->FreeResources();
}
