/*
 *      Copyright (C) 2005-2015 Team XBMC
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "Application.h"

#include "ServiceManager.h"

#include "input/Key.h"

CApplication::CApplication(void)
  : m_pPlayer(new CApplicationPlayer)
  , m_itemCurrentFile(new CFileItem)
{
}

CApplication::~CApplication(void)
{
}

bool CApplication::OnAction(const CAction &action)
{
  return false;
}

void CApplication::ResetScreenSaver()
{
}

std::shared_ptr<CFileItem> CApplication::CurrentFileItemPtr()
{
  return m_itemCurrentFile;
}

CFileItem& CApplication::CurrentFileItem()
{
  return *m_itemCurrentFile;
}

bool CApplication::IsCurrentThread() const
{
  return true;
}
