/*
 *  Copyright (c) 2002 Frodo
 *      Portions Copyright (c) by the authors of ffmpeg and xvid
 *  Copyright (C) 2002-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

// File.h: interface for the CFile class.
//
//////////////////////////////////////////////////////////////////////

#include "IFileTypes.h"
#include "URL.h"

#include <iostream>
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

#include "PlatformDefs.h"

namespace XFILE
{

class IFile;

class CFile
{
public:
  CFile();
  ~CFile();

  /**
   * Attempt to read bufSize bytes from currently opened file into buffer bufPtr.
   * @param bufPtr  pointer to buffer
   * @param bufSize size of the buffer
   * @return number of successfully read bytes if any bytes were read and stored in
   *         buffer, zero if no bytes are available to read (end of file was reached)
   *         or undetectable error occur, -1 in case of any explicit error
   */
  ssize_t Read(void* bufPtr, size_t bufSize);
  /**
   * Attempt to write bufSize bytes from buffer bufPtr into currently opened file.
   * @param bufPtr  pointer to buffer
   * @param bufSize size of the buffer
   * @return number of successfully written bytes if any bytes were written,
   *         zero if no bytes were written and no detectable error occur,
   *         -1 in case of any explicit error
   */
  ssize_t Write(const void* bufPtr, size_t bufSize);

  void Close();

  IFile* GetImplementation() const { return m_pFile.get(); }

  // CURL interface
  static bool Exists(const CURL& file, bool bUseCache = true);

  /**
  * Fills struct __stat64 with information about file specified by filename
  * For st_mode function will set correctly _S_IFDIR (directory) flag and may set
  * _S_IREAD (read permission), _S_IWRITE (write permission) flags if such
  * information is available. Function may set st_size (file size), st_atime,
  * st_mtime, st_ctime (access, modification, creation times).
  * Any other flags and members of __stat64 that didn't updated with actual file
  * information will be set to zero (st_nlink can be set ether to 1 or zero).
  * @param file        specifies requested file
  * @param buffer      pointer to __stat64 buffer to receive information about file
  * @return zero of success, -1 otherwise.
  */
  static int  Stat(const CURL& file, struct __stat64* buffer);

  // string interface
  static bool Exists(const std::string& strFileName, bool bUseCache = true);
  /**
  * Fills struct __stat64 with information about file specified by filename
  * For st_mode function will set correctly _S_IFDIR (directory) flag and may set
  * _S_IREAD (read permission), _S_IWRITE (write permission) flags if such
  * information is available. Function may set st_size (file size), st_atime,
  * st_mtime, st_ctime (access, modification, creation times).
  * Any other flags and members of __stat64 that didn't updated with actual file
  * information will be set to zero (st_nlink can be set ether to 1 or zero).
  * @param strFileName specifies requested file
  * @param buffer      pointer to __stat64 buffer to receive information about file
  * @return zero of success, -1 otherwise.
  */
  static int  Stat(const std::string& strFileName, struct __stat64* buffer);
  /**
  * Fills struct __stat64 with information about currently open file
  * For st_mode function will set correctly _S_IFDIR (directory) flag and may set
  * _S_IREAD (read permission), _S_IWRITE (write permission) flags if such
  * information is available. Function may set st_size (file size), st_atime,
  * st_mtime, st_ctime (access, modification, creation times).
  * Any other flags and members of __stat64 that didn't updated with actual file
  * information will be set to zero (st_nlink can be set ether to 1 or zero).
  * @param buffer      pointer to __stat64 buffer to receive information about file
  * @return zero of success, -1 otherwise.
  */
  int Stat(struct __stat64 *buffer);

private:
  unsigned int m_flags = 0;
  CURL                m_curl;
  std::unique_ptr<IFile> m_pFile;
};

}
