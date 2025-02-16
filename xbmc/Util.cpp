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
  std::string name = StringUtils::Format(fn_template.c_str(), 0);

  CFileItemList items;
  if (!CDirectory::GetDirectory(searchPath, items, mask, DIR_FLAG_NO_FILE_DIRS))
    return name;

  items.SetFastLookup(true);
  for (int i = 0; i <= max; i++)
  {
    std::string name = StringUtils::Format(fn_template.c_str(), i);
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