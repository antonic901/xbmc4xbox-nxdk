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

#include "threads/SystemClock.h"
#include "GUILargeTextureManager.h"
#include "ServiceBroker.h"
#include "guilib/Texture.h"
#include "threads/SingleLock.h"
#include "utils/TimeUtils.h"
#include "utils/JobManager.h"
#include "guilib/GraphicContext.h"
#include "utils/log.h"

#include <cassert>
#include <mutex>

CImageLoader::CImageLoader(const std::string &path, const bool useCache):
  m_path(path)
{
  m_texture = nullptr;
  m_use_cache = useCache;
}

CImageLoader::~CImageLoader()
{
  m_texture.reset();
}

bool CImageLoader::DoWork()
{
  bool needsChecking = false;
  std::string loadPath;

  std::string texturePath = CServiceBroker::GetGUI()->GetTextureManager().GetTexturePath(m_path);
  if (texturePath.empty())
    return false;

#if 0
  if (m_use_cache)
    loadPath = CTextureCache::GetInstance().CheckCachedImage(texturePath, needsChecking);
  else
#endif
    loadPath = texturePath;

  if (!loadPath.empty())
  {
    // direct route - load the image
    auto start = std::chrono::steady_clock::now();
    m_texture = CTexture::LoadFromFile(loadPath, g_graphicsContext.GetWidth(), g_graphicsContext.GetHeight());

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (duration.count() > 100)
      CLog::Log(LOGDEBUG, "%s - took %u ms to load %s", __FUNCTION__, duration.count(), loadPath.c_str());

    if (m_texture)
    {
#if 0
      if (needsChecking)
        CTextureCache::GetInstance().BackgroundCacheImage(texturePath);
#endif

      return true;
    }

    // Fallthrough on failure:
    CLog::Log(LOGERROR, "%s - Direct texture file loading failed for %s", __FUNCTION__, loadPath.c_str());
  }

  if (!m_use_cache)
    return false; // We're done

#if 0
  // not in our texture cache or it failed to load from it, so try and load directly and then cache the result
  CTextureCache::GetInstance().CacheImage(texturePath, &m_texture);
#endif
  return (m_texture != NULL);
}

CGUILargeTextureManager::CLargeTexture::CLargeTexture(const std::string &path):
  m_path(path)
{
  m_refCount = 1;
  m_timeToDelete = 0;
}

CGUILargeTextureManager::CLargeTexture::~CLargeTexture()
{
  assert(m_refCount == 0);
  m_texture.Free();
}

void CGUILargeTextureManager::CLargeTexture::AddRef()
{
  m_refCount++;
}

bool CGUILargeTextureManager::CLargeTexture::DecrRef(bool deleteImmediately)
{
  assert(m_refCount);
  m_refCount--;
  if (m_refCount == 0)
  {
    if (deleteImmediately)
      delete this;
    else
      m_timeToDelete = CTimeUtils::GetFrameTime() + TIME_TO_DELETE;
    return true;
  }
  return false;
}

bool CGUILargeTextureManager::CLargeTexture::DeleteIfRequired(bool deleteImmediately)
{
  if (m_refCount == 0 && (deleteImmediately || m_timeToDelete < CTimeUtils::GetFrameTime()))
  {
    delete this;
    return true;
  }
  return false;
}

void CGUILargeTextureManager::CLargeTexture::SetTexture(std::unique_ptr<CTexture> texture)
{
  assert(!m_texture.size());
  if (texture)
    m_texture.Set(std::move(texture), texture->GetWidth(), texture->GetHeight());
}

CGUILargeTextureManager::CGUILargeTextureManager()
{
}

CGUILargeTextureManager::~CGUILargeTextureManager()
{
}

void CGUILargeTextureManager::CleanupUnusedImages(bool immediately)
{
  std::unique_lock<CCriticalSection> lock(m_listSection);
  // check for items to remove from allocated list, and remove
  listIterator it = m_allocated.begin();
  while (it != m_allocated.end())
  {
    CLargeTexture *image = *it;
    if (image->DeleteIfRequired(immediately))
      it = m_allocated.erase(it);
    else
      ++it;
  }
}

// if available, increment reference count, and return the image.
// else, add to the queue list if appropriate.
bool CGUILargeTextureManager::GetImage(const std::string &path, CTextureArray &texture, bool firstRequest, const bool useCache)
{
  std::unique_lock<CCriticalSection> lock(m_listSection);
  for (listIterator it = m_allocated.begin(); it != m_allocated.end(); ++it)
  {
    CLargeTexture *image = *it;
    if (image->GetPath() == path)
    {
      if (firstRequest)
        image->AddRef();
      texture = image->GetTexture();
      return texture.size() > 0;
    }
  }

  if (firstRequest)
    QueueImage(path, useCache);

  return true;
}

void CGUILargeTextureManager::ReleaseImage(const std::string &path, bool immediately)
{
  std::unique_lock<CCriticalSection> lock(m_listSection);
  for (listIterator it = m_allocated.begin(); it != m_allocated.end(); ++it)
  {
    CLargeTexture *image = *it;
    if (image->GetPath() == path)
    {
      if (image->DecrRef(immediately) && immediately)
        m_allocated.erase(it);
      return;
    }
  }
  for (queueIterator it = m_queued.begin(); it != m_queued.end(); ++it)
  {
    unsigned int id = it->first;
    CLargeTexture *image = it->second;
    if (image->GetPath() == path && image->DecrRef(true))
    {
      // cancel this job
      CServiceBroker::GetJobManager()->CancelJob(id);
      m_queued.erase(it);
      return;
    }
  }
}

// queue the image, and start the background loader if necessary
void CGUILargeTextureManager::QueueImage(const std::string &path, bool useCache)
{
  if (path.empty())
    return;

  std::unique_lock<CCriticalSection> lock(m_listSection);
  for (queueIterator it = m_queued.begin(); it != m_queued.end(); ++it)
  {
    CLargeTexture *image = it->second;
    if (image->GetPath() == path)
    {
      image->AddRef();
      return; // already queued
    }
  }

  // queue the item
  CLargeTexture *image = new CLargeTexture(path);
  unsigned int jobID = CServiceBroker::GetJobManager()->AddJob(new CImageLoader(path, useCache), this, CJob::PRIORITY_NORMAL);
  m_queued.push_back(std::make_pair(jobID, image));
}

void CGUILargeTextureManager::OnJobComplete(unsigned int jobID, bool success, CJob *job)
{
  // see if we still have this job id
  std::unique_lock<CCriticalSection> lock(m_listSection);
  for (queueIterator it = m_queued.begin(); it != m_queued.end(); ++it)
  {
    if (it->first == jobID)
    { // found our job
      CImageLoader *loader = (CImageLoader *)job;
      CLargeTexture *image = it->second;
      image->SetTexture(std::move(loader->m_texture));
      loader->m_texture = NULL; // we want to keep the texture, and jobs are auto-deleted.
      m_queued.erase(it);
      m_allocated.push_back(image);
      return;
    }
  }
}
