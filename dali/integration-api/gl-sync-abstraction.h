#ifndef __DALI_INTEGRATION_GL_SYNC_ABSTRACTION_H__
#define __DALI_INTEGRATION_GL_SYNC_ABSTRACTION_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Integration
{

/**
 * This abstraction defines an API for syncing CPU with GPU.
 * A typical use case is to determine when GL draw calls have finished drawing
 * to a framebuffer.
 */
class GlSyncAbstraction
{
protected:
  /**
   * Virtual protected destructor, no deletion through this interface
   */
  virtual ~GlSyncAbstraction() {}

public:

  class SyncObject
  {
  protected:
    /**
     * Virtual protected destructor, no deletion through this interface. This prevents
     * Core from deleting SyncObjects - only Adaptor implementation is able to delete
     * them.
     */
    virtual ~SyncObject() {}

  public:

    /**
     * Determine if the synchronisation object has been signalled.
     * @return false if the sync object has not been signalled, true if it has been signalled (and
     * can now be destroyed)
     */
    virtual bool IsSynced() = 0;
  };

  /**
   * Create a synchronisation object based on the resource id, typically that of
   * a framebuffer texture. It can then be polled using the same resource id.
   * @return A pointer to an opaque sync object
   */
  virtual SyncObject* CreateSyncObject() = 0;

  /**
   * Destroy the synchronisation object.
   * @param[in] syncObject The sync object to destroy
   */
  virtual void DestroySyncObject(SyncObject* syncObject) = 0;
};

} // namespace Integration
} // namespace Dali

#endif // __DALI_INTEGRATION_GL_SYNC_ABSTRACTION_H__
