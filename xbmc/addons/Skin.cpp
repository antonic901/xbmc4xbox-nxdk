/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "Skin.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/WindowIDs.h"
#include "utils/URIUtils.h"
#include "utils/StringUtils.h"
#include "utils/log.h"
#include "utils/XMLUtils.h"

using namespace std;
using namespace XFILE;

#define SKIN_MIN_VERSION 2.1f

std::shared_ptr<CSkinInfo> g_SkinInfo;

CSkinInfo::CSkinInfo()
{
  SetDefaults();
}

CSkinInfo::~CSkinInfo()
{}

void CSkinInfo::SetDefaults()
{
  m_strBaseDir = "";
  m_DefaultResolution = RES_PAL_4x3;
  m_DefaultResolutionWide = RES_INVALID;
  m_effectsSlowDown = 1.0f;
  m_Version = 1.0;
  m_debugging = false;
  m_iNumCreditLines = 0;
  m_skinzoom = 1.0f;
  m_bLegacy = false;
}

void CSkinInfo::Load(const std::string& strSkinDir, bool loadIncludes)
{
  SetDefaults();
  m_strBaseDir = strSkinDir;

  // Load from skin.xml
  TiXmlDocument xmlDoc;
  std::string strFile = m_strBaseDir + "\\skin.xml";
  if (xmlDoc.LoadFile(strFile))
  { // ok - get the default skin folder out of it...
    const TiXmlNode* root = xmlDoc.RootElement();
    if (root && root->ValueStr() == "skin")
    {
      GetResolution(root, "defaultresolution", m_DefaultResolution);
      if (!GetResolution(root, "defaultresolutionwide", m_DefaultResolutionWide))
        m_DefaultResolutionWide = m_DefaultResolution;

      CLog::Log(LOGINFO, "Default 4:3 resolution directory is %s", URIUtils::AddFileToFolder(m_strBaseDir, GetDirFromRes(m_DefaultResolution)).c_str());
      CLog::Log(LOGINFO, "Default 16:9 resolution directory is %s", URIUtils::AddFileToFolder(m_strBaseDir, GetDirFromRes(m_DefaultResolutionWide)).c_str());

      XMLUtils::GetDouble(root, "version", m_Version);
      XMLUtils::GetFloat(root, "effectslowdown", m_effectsSlowDown);
      XMLUtils::GetBoolean(root, "debugging", m_debugging);
      XMLUtils::GetFloat(root, "zoom", m_skinzoom);

      // get the legacy parameter to tweak the control behaviour for old skins such as PM3
      XMLUtils::GetBoolean(root, "legacy", m_bLegacy);   

#if 0
      // now load the credits information
      const TiXmlNode *pChild = root->FirstChild("credits");
      if (pChild)
      { // ok, run through the credits
        const TiXmlNode *pGrandChild = pChild->FirstChild("skinname");
        if (pGrandChild && pGrandChild->FirstChild())
        {
          std::string strName = pGrandChild->FirstChild()->Value();
          swprintf(credits[0], L"%S Skin", strName.Left(44).c_str());
        }
        pGrandChild = pChild->FirstChild("name");
        m_iNumCreditLines = 1;
        while (pGrandChild && pGrandChild->FirstChild() && m_iNumCreditLines < 6)
        {
          std::string strName = pGrandChild->FirstChild()->Value();
          swprintf(credits[m_iNumCreditLines], L"%S", strName.Left(49).c_str());
          m_iNumCreditLines++;
          pGrandChild = pGrandChild->NextSibling("name");
        }
      }
#endif

      // now load the startupwindow information
      LoadStartupWindows(root->FirstChildElement("startupwindows"));
    }
    else
      CLog::Log(LOGERROR, "%s - %s doesnt contain <skin>", __FUNCTION__, strFile.c_str());
  }
  // Load the skin includes
  if (loadIncludes)
    LoadIncludes();
}

bool CSkinInfo::Check(const std::string& strSkinDir)
{
  CSkinInfo info;
  info.Load(strSkinDir, false);
  if (info.GetVersion() < GetMinVersion())
  {
    CLog::Log(LOGERROR, "%s(%s) version is to old (%f versus %f)", __FUNCTION__, strSkinDir.c_str(), info.GetVersion(), GetMinVersion());
    return false;
  }
  if (!info.HasSkinFile("Home.xml") || !info.HasSkinFile("Font.xml"))
  {
    CLog::Log(LOGERROR, "%s(%s) does not contain Home.xml or Font.xml", __FUNCTION__, strSkinDir.c_str());
    return false;
  }
  return true;
}

std::string CSkinInfo::GetSkinPath(const std::string& strFile, RESOLUTION_INFO *resInfo, const std::string& strBaseDir /* = "" */) const
{
  RESOLUTION res = RES_INVALID;
  if (resInfo->iWidth == 1080)
    res = RES_HDTV_1080i;
  else if (resInfo->iWidth == 1280)
    res = RES_HDTV_720p;
  else if (resInfo->iWidth == 480)
    res = RES_NTSC_16x9;
  return GetSkinPath(strFile, &res, strBaseDir);
}

std::string CSkinInfo::GetSkinPath(const std::string& strFile, RESOLUTION *res, const std::string& strBaseDir /* = "" */) const
{
  std::string strPathToUse = m_strBaseDir;
  if (!strBaseDir.empty())
    strPathToUse = strBaseDir;

  // if the caller doesn't care about the resolution just use a temporary
  RESOLUTION tempRes = RES_INVALID;
  if (!res)
    res = &tempRes;

  // first try and load from the current resolution's directory
  if (*res == RES_INVALID)
    *res = g_graphicsContext.GetVideoResolution();
  std::string strPath = URIUtils::AddFileToFolder(strPathToUse, GetDirFromRes(*res));
  strPath = URIUtils::AddFileToFolder(strPath, strFile);
  if (CFile::Exists(strPath))
    return strPath;
  // if we're in 1080i mode, try 720p next
  if (*res == RES_HDTV_1080i)
  {
    *res = RES_HDTV_720p;
    strPath = URIUtils::AddFileToFolder(strPathToUse, GetDirFromRes(*res));
    strPath = URIUtils::AddFileToFolder(strPath, strFile);
    if (CFile::Exists(strPath))
      return strPath;
  }
  // that failed - drop to the default widescreen resolution if where in a widemode
  if (*res == RES_PAL_16x9 || *res == RES_NTSC_16x9 || *res == RES_HDTV_480p_16x9 || *res == RES_HDTV_720p)
  {
    *res = m_DefaultResolutionWide;
    strPath = URIUtils::AddFileToFolder(strPathToUse, GetDirFromRes(*res));
    strPath = URIUtils::AddFileToFolder(strPath, strFile);
    if (CFile::Exists(strPath))
      return strPath;
  }
  // that failed - drop to the default resolution
  *res = m_DefaultResolution;
  strPath = URIUtils::AddFileToFolder(strPathToUse, GetDirFromRes(*res));
  strPath = URIUtils::AddFileToFolder(strPath, strFile);
  // check if we don't have any subdirectories
  if (*res == RES_INVALID) *res = RES_PAL_4x3;
  return strPath;
}

bool CSkinInfo::HasSkinFile(const std::string &strFile) const
{
  return CFile::Exists(GetSkinPath(strFile));
}

std::string CSkinInfo::GetDirFromRes(RESOLUTION res) const
{
  std::string strRes;
  switch (res)
  {
  case RES_PAL_4x3:
    strRes = "PAL";
    break;
  case RES_PAL_16x9:
    strRes = "PAL16x9";
    break;
  case RES_NTSC_4x3:
  case RES_HDTV_480p_4x3:
    strRes = "NTSC";
    break;
  case RES_NTSC_16x9:
  case RES_HDTV_480p_16x9:
    strRes = "ntsc16x9";
    break;
  case RES_HDTV_720p:
    strRes = "720p";
    break;
  case RES_HDTV_1080i:
    strRes = "1080i";
    break;
  case RES_INVALID:
  default:
    strRes = "";
    break;
  }
  return strRes;
}

RESOLUTION CSkinInfo::TranslateResolution(const std::string &res, RESOLUTION def)
{
  if (res == "pal")
    return RES_PAL_4x3;
  else if (res == "pal16x9")
    return RES_PAL_16x9;
  else if (res == "ntsc")
    return RES_NTSC_4x3;
  else if (res == "ntsc16x9")
    return RES_NTSC_16x9;
  else if (res == "720p")
    return RES_HDTV_720p;
  else if (res == "1080i")
    return RES_HDTV_1080i;
  CLog::Log(LOGERROR, "%s invalid resolution specified for %s", __FUNCTION__, res.c_str());
  return def;
}

std::string CSkinInfo::GetBaseDir() const
{
  return m_strBaseDir;
}

wchar_t* CSkinInfo::GetCreditsLine(int i)
{
  if (i < m_iNumCreditLines)
    return credits[i];
  else
    return NULL;
}

double CSkinInfo::GetMinVersion()
{
  return SKIN_MIN_VERSION;
}

void CSkinInfo::LoadIncludes()
{
  std::string includesPath = GetSkinPath("includes.xml");
  CLog::Log(LOGINFO, "Loading skin includes from %s", includesPath.c_str());
  m_includes.ClearIncludes();
  m_includes.LoadIncludes(includesPath);
}

void CSkinInfo::ResolveIncludes(TiXmlElement *node, std::map<INFO::InfoPtr, bool>* xmlIncludeConditions /* = NULL */)
{
  if(xmlIncludeConditions)
    xmlIncludeConditions->clear();

  m_includes.ResolveIncludes(node, xmlIncludeConditions);
}

int CSkinInfo::GetStartWindow() const
{
  int windowID = WINDOW_HOME;
  assert(m_startupWindows.size());
  for (vector<CStartupWindow>::const_iterator it = m_startupWindows.begin(); it != m_startupWindows.end(); it++)
  {
    if (windowID == (*it).m_id)
      return windowID;
  }
  // return our first one
  return m_startupWindows[0].m_id;
}

bool CSkinInfo::LoadStartupWindows(const TiXmlElement *startup)
{
  m_startupWindows.clear();
  if (startup)
  { // yay, run through and grab the startup windows
    const TiXmlElement *window = startup->FirstChildElement("window");
    while (window && window->FirstChild())
    {
      int id;
      window->Attribute("id", &id);
      std::string name = window->FirstChild()->Value();
      m_startupWindows.push_back(CStartupWindow(id + WINDOW_HOME, name));
      window = window->NextSiblingElement("window");
    }
  }

  // ok, now see if we have any startup windows
  if (!m_startupWindows.size())
  { // nope - add the default ones
    m_startupWindows.push_back(CStartupWindow(WINDOW_HOME, "513"));
    m_startupWindows.push_back(CStartupWindow(WINDOW_PROGRAMS, "0"));
    m_startupWindows.push_back(CStartupWindow(WINDOW_PICTURES, "1"));
    m_startupWindows.push_back(CStartupWindow(WINDOW_MUSIC_NAV, "2"));
    m_startupWindows.push_back(CStartupWindow(WINDOW_VIDEO_NAV, "3"));
    m_startupWindows.push_back(CStartupWindow(WINDOW_FILES, "7"));
    m_startupWindows.push_back(CStartupWindow(WINDOW_SETTINGS_MENU, "Settings"));
  }
  return true;
}

bool CSkinInfo::IsWide(RESOLUTION res) const
{
  return (res == RES_PAL_16x9 || res == RES_NTSC_16x9 || res == RES_HDTV_480p_16x9 || res == RES_HDTV_720p || res == RES_HDTV_1080i);
}

void CSkinInfo::GetSkinPaths(std::vector<std::string> &paths) const
{
  RESOLUTION resToUse = RES_INVALID;
  GetSkinPath("Home.xml", &resToUse);
  paths.push_back(URIUtils::AddFileToFolder(m_strBaseDir, GetDirFromRes(resToUse)));
  // see if we need to add other paths
  if (resToUse != m_DefaultResolutionWide && IsWide(resToUse))
    paths.push_back(URIUtils::AddFileToFolder(m_strBaseDir, GetDirFromRes(m_DefaultResolutionWide)));
  if (resToUse != m_DefaultResolution && (!IsWide(resToUse) || m_DefaultResolutionWide != m_DefaultResolution))
    paths.push_back(URIUtils::AddFileToFolder(m_strBaseDir, GetDirFromRes(m_DefaultResolution)));
}

bool CSkinInfo::GetResolution(const TiXmlNode *root, const char *tag, RESOLUTION &res) const
{
  std::string strRes;
  if (XMLUtils::GetString(root, tag, strRes))
  {
    StringUtils::ToLower(strRes);
    if (strRes == "pal")
      res = RES_PAL_4x3;
    else if (strRes == "pal16x9")
      res = RES_PAL_16x9;
    else if (strRes == "ntsc")
      res = RES_NTSC_4x3;
    else if (strRes == "ntsc16x9")
      res = RES_NTSC_16x9;
    else if (strRes == "720p")
      res = RES_HDTV_720p;
    else if (strRes == "1080i")
      res = RES_HDTV_1080i;
    else
    {
      CLog::Log(LOGERROR, "%s invalid resolution specified for <%s>, %s", __FUNCTION__, tag, strRes.c_str());
      return false;
    }
    return true;
  }
  return false;
}

int CSkinInfo::GetFirstWindow() const
{
  int startWindow = GetStartWindow();
  if (HasSkinFile("Startup.xml"))
    startWindow = WINDOW_STARTUP_ANIM;
  return startWindow;
}

const INFO::CSkinVariableString* CSkinInfo::CreateSkinVariable(const std::string& name, int context)
{
  return m_includes.CreateSkinVariable(name, context);
}
