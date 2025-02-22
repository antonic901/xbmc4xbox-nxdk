/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "XBOXUtil.h"

#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/RegExp.h"

CXBOXUtil::CXBOXUtil(void)
{
}

CXBOXUtil::~CXBOXUtil(void)
{
}

bool CXBOXUtil::ShortenFileName(std::string& strFileNameAndPath)
{
  std::string strFile = URIUtils::GetFileName(strFileNameAndPath);
  if (strFile.size() > 42)
  {
    std::string strExtension = URIUtils::GetExtension(strFileNameAndPath);
    std::string strPath = strFileNameAndPath.substr(0, strFileNameAndPath.size() - strFile.size());

    CRegExp reg;
    std::string strSearch=strFile; StringUtils::ToLower(strSearch);
    reg.RegComp("([_\\-\\. ](cd|part)[0-9]*)[_\\-\\. ]");          // this is to ensure that cd1, cd2 or partXXX. do not
    int matchPos = reg.RegFind(strSearch.c_str());                 // get cut from filenames when they are shortened.

    std::string strPartNumber = reg.GetReplaceString("\\1");

    int partPos = 42 - strPartNumber.size() - strExtension.size();

    if (matchPos > partPos )
    {
       strFile = strFile.substr(0, partPos);
       strFile += strPartNumber;
    }
    else
    {
       strFile = strFile.substr(0, 42 - strExtension.size());
    }
    strFile += strExtension;

    std::string strNewFile = strPath;
    if (!URIUtils::HasSlashAtEnd(strNewFile) && !strNewFile.empty())
      strNewFile += "\\";

    strNewFile += strFile;
    strFileNameAndPath = strNewFile;

    // We shortened the file:
    return true;
  }

  return false;
}

void CXBOXUtil::RemoveIllegalChars(std::string& strText)
{
  char szRemoveIllegal [1024];
  strcpy(szRemoveIllegal , strText.c_str());
  static char legalChars[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!#$%&'()-@[]^_`{}~.��������������������������������";

  char *cursor;
  for (cursor = szRemoveIllegal; *(cursor += strspn(cursor, legalChars)); /**/ )
  {
    // Convert FatX illegal characters, if possible, to the closest "looking" character:
    if (strchr("������", (int) *cursor)) *cursor = 'A';
    else
    if (strchr("������", (int) *cursor)) *cursor = 'a';
    else
    if (strchr("�����", (int) *cursor)) *cursor = 'O';
    else
    if (strchr("�����", (int) *cursor)) *cursor = 'o';
    else
    if (strchr("����", (int) *cursor)) *cursor = 'U';
    else
    if (strchr("�����", (int) *cursor)) *cursor = 'u';
    else
    if (strchr("����", (int) *cursor)) *cursor = 'E';
    else
    if (strchr("����", (int) *cursor)) *cursor = 'e';
    else
    if (strchr("����", (int) *cursor)) *cursor = 'I';
    else
    if (strchr("����", (int) *cursor)) *cursor = 'i';
    else
    *cursor = '_';
  }

  strText = szRemoveIllegal;
}

std::string CXBOXUtil::GetFatXQualifiedPath(const std::string& strPath)
{
  std::string strFileNameAndPath(strPath);
  // This routine gets rid of any "\\"'s at the start of the path.
  // Should this be the case?
  std::string strBasePath, strFileName;

  // We need to check whether we must use forward (ie. special://)
  // or backslashes (ie. Q:\)
  std::string sep;
  if (strFileNameAndPath.c_str()[1] == ':' || strFileNameAndPath.find('\\')>=0)
  {
    StringUtils::Replace(strFileNameAndPath, '/', '\\');
    sep="\\";
  }
  else
  {
//    strFileNameAndPath.Replace('\\', '/');
    sep="/";
  }

  if(strFileNameAndPath.substr(std::max(0, (int)strFileNameAndPath.size() - 1)) == sep)
  {
    strBasePath = strFileNameAndPath;
    strFileName = "";
  }
  else
  {
    strBasePath = URIUtils::GetDirectory(strFileNameAndPath);
    // TODO: GETDIR - is this required?  What happens to the tokenize below otherwise?
    strFileName = URIUtils::GetFileName(strFileNameAndPath);
  }

  std::vector<std::string> tokens = StringUtils::Split(strBasePath, sep);
  if (tokens.empty())
    return strPath; // nothing to do here (invalid path)

  strFileNameAndPath = tokens.front();
  for (std::vector<std::string>::iterator token=tokens.begin()+1;token != tokens.end();++token)
  {
    std::string strToken = token->substr(0, 42);
    if (token->size() > 42)
    {
      // remove any spaces as a result of truncation (only):
      while (strToken[strToken.size()-1] == ' ')
        strToken.erase(strToken.size()-1);
    }
    CXBOXUtil::RemoveIllegalChars(strToken);
    strFileNameAndPath += sep+strToken;
  }

  if (!strFileName.empty())
  {
    CXBOXUtil::RemoveIllegalChars(strFileName);

    if (strFileName.substr(0, 1) == sep)
      strFileName.erase(0,1);

    if (CXBOXUtil::ShortenFileName(strFileName))
    {
      std::string strExtension = URIUtils::GetExtension(strFileName);
      std::string strNoExt(URIUtils::ReplaceExtension(strFileName, ""));
      // remove any spaces as a result of truncation (only):
      while (strNoExt[strNoExt.size()-1] == ' ')
        strNoExt.erase(strNoExt.size()-1);

      strFileNameAndPath += strNoExt+strExtension;
    }
    else
      strFileNameAndPath += strFileName;
  }

  return strFileNameAndPath;
}

std::string CXBOXUtil::GetFatXQualifiedPath(const CURL& url)
{
  return CXBOXUtil::GetFatXQualifiedPath(url.Get());
}
