/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "Util.h"

#include "FileItem.h"
#include "filesystem/Directory.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

using namespace XFILE;

/*!
  \brief Finds next unused filename that matches padded int format identifier provided
  \param[in]  fn_template    filename template consisting of a padded int format identifier (eg screenshot%03d)
  \param[in]  max            maximum number to search for available name
  \return "" on failure, string next available name matching format identifier on success
*/

std::string CUtil::GetNextFilename(const std::string &fn_template, int max)
{
  std::string searchPath = URIUtils::GetDirectory(fn_template);
  std::string mask = URIUtils::GetExtension(fn_template);
  std::string name = StringUtils::Format(fn_template, 0);

  CFileItemList items;
  if (!CDirectory::GetDirectory(searchPath, items, mask, DIR_FLAG_NO_FILE_DIRS))
    return name;

  items.SetFastLookup(true);
  for (int i = 0; i <= max; i++)
  {
    std::string name = StringUtils::Format(fn_template, i);
    if (!items.Get(name))
      return name;
  }
  return "";
}

std::string CUtil::ValidatePath(const std::string &path, bool bFixDoubleSlashes /* = false */)
{
  std::string result = path;

  // Don't do any stuff on URLs containing %-characters or protocols that embed
  // filenames. NOTE: Don't use IsInZip or IsInRar here since it will infinitely
  // recurse and crash XBMC
  if (URIUtils::IsURL(path) &&
      (path.find('%') != std::string::npos ||
      StringUtils::StartsWithNoCase(path, "apk:") ||
      StringUtils::StartsWithNoCase(path, "zip:") ||
      StringUtils::StartsWithNoCase(path, "rar:") ||
      StringUtils::StartsWithNoCase(path, "stack:") ||
      StringUtils::StartsWithNoCase(path, "bluray:") ||
      StringUtils::StartsWithNoCase(path, "multipath:") ))
    return result;

  // check the path for incorrect slashes
#if defined(TARGET_WINDOWS) || defined(NXDK)
  if (URIUtils::IsDOSPath(path))
  {
    StringUtils::Replace(result, '/', '\\');
    /* The double slash correction should only be used when *absolutely*
       necessary! This applies to certain DLLs or use from Python DLLs/scripts
       that incorrectly generate double (back) slashes.
    */
    if (bFixDoubleSlashes && !result.empty())
    {
      // Fixup for double back slashes (but ignore the \\ of unc-paths)
      for (size_t x = 1; x < result.size() - 1; x++)
      {
        if (result[x] == '\\' && result[x+1] == '\\')
          result.erase(x, 1);
      }
    }
  }
  else if (path.find("://") != std::string::npos || path.find(":\\\\") != std::string::npos)
#endif
  {
    StringUtils::Replace(result, '\\', '/');
    /* The double slash correction should only be used when *absolutely*
       necessary! This applies to certain DLLs or use from Python DLLs/scripts
       that incorrectly generate double (back) slashes.
    */
    if (bFixDoubleSlashes && !result.empty())
    {
      // Fixup for double forward slashes(/) but don't touch the :// of URLs
      for (size_t x = 2; x < result.size() - 1; x++)
      {
        if ( result[x] == '/' && result[x + 1] == '/' && !(result[x - 1] == ':' || (result[x - 1] == '/' && result[x - 2] == ':')) )
          result.erase(x, 1);
      }
    }
  }
  return result;
}

void CUtil::SplitParams(const std::string &paramString, std::vector<std::string> &parameters)
{
  bool inQuotes = false;
  bool lastEscaped = false; // only every second character can be escaped
  int inFunction = 0;
  size_t whiteSpacePos = 0;
  std::string parameter;
  parameters.clear();
  for (size_t pos = 0; pos < paramString.size(); pos++)
  {
    char ch = paramString[pos];
    bool escaped = (pos > 0 && paramString[pos - 1] == '\\' && !lastEscaped);
    lastEscaped = escaped;
    if (inQuotes)
    { // if we're in a quote, we accept everything until the closing quote
      if (ch == '"' && !escaped)
      { // finished a quote - no need to add the end quote to our string
        inQuotes = false;
      }
    }
    else
    { // not in a quote, so check if we should be starting one
      if (ch == '"' && !escaped)
      { // start of quote - no need to add the quote to our string
        inQuotes = true;
      }
      if (inFunction && ch == ')')
      { // end of a function
        inFunction--;
      }
      if (ch == '(')
      { // start of function
        inFunction++;
      }
      if (!inFunction && ch == ',')
      { // not in a function, so a comma signfies the end of this parameter
        if (whiteSpacePos)
          parameter = parameter.substr(0, whiteSpacePos);
        // trim off start and end quotes
        if (parameter.length() > 1 && parameter[0] == '"' && parameter[parameter.length() - 1] == '"')
          parameter = parameter.substr(1, parameter.length() - 2);
        else if (parameter.length() > 3 && parameter[parameter.length() - 1] == '"')
        {
          // check name="value" style param.
          size_t quotaPos = parameter.find('"');
          if (quotaPos > 1 && quotaPos < parameter.length() - 1 && parameter[quotaPos - 1] == '=')
          {
            parameter.erase(parameter.length() - 1);
            parameter.erase(quotaPos);
          }
        }
        parameters.push_back(parameter);
        parameter.clear();
        whiteSpacePos = 0;
        continue;
      }
    }
    if ((ch == '"' || ch == '\\') && escaped)
    { // escaped quote or backslash
      parameter[parameter.size()-1] = ch;
      continue;
    }
    // whitespace handling - we skip any whitespace at the left or right of an unquoted parameter
    if (ch == ' ' && !inQuotes)
    {
      if (parameter.empty()) // skip whitespace on left
        continue;
      if (!whiteSpacePos) // make a note of where whitespace starts on the right
        whiteSpacePos = parameter.size();
    }
    else
      whiteSpacePos = 0;
    parameter += ch;
  }
  if (inFunction || inQuotes)
    CLog::Log(LOGWARNING, "%s(%s) - end of string while searching for ) or \"", __FUNCTION__, paramString.c_str());
  if (whiteSpacePos)
    parameter.erase(whiteSpacePos);
  // trim off start and end quotes
  if (parameter.size() > 1 && parameter[0] == '"' && parameter[parameter.size() - 1] == '"')
    parameter = parameter.substr(1,parameter.size() - 2);
  else if (parameter.size() > 3 && parameter[parameter.size() - 1] == '"')
  {
    // check name="value" style param.
    size_t quotaPos = parameter.find('"');
    if (quotaPos > 1 && quotaPos < parameter.length() - 1 && parameter[quotaPos - 1] == '=')
    {
      parameter.erase(parameter.length() - 1);
      parameter.erase(quotaPos);
    }
  }
  if (!parameter.empty() || parameters.size())
    parameters.push_back(parameter);
}

int CUtil::GetRandomNumber()
{
#if defined(TARGET_WINDOWS) || defined(NXDK)
  unsigned int number;
  if (rand_s(&number) == 0)
    return (int)number;
#else
  return rand_r(&s_randomSeed);
#endif

  return rand();
}
