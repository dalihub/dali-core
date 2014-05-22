#ifndef __DALI_INTERNAL_SYNC_RESOURCE_TRACKER_H__
#define __DALI_INTERNAL_SYNC_RESOURCE_TRACKER_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali/internal/update/resources/resource-tracker.h>

namespace Dali
{
namespace Integration
{
class GlSyncAbstraction;
}

namespace Internal
{

namespace SceneGraph
{
class RenderTracker;
class RenderMessageDispatcher;
}

/**
 * Class to track completion of a resource.
 * This class is designed to be used on resources that change completion in the render thread.
 * Used by RenderTasks that render to a framebuffer backed by a native image, i.e. it
 * needs GlFenceSync to determine when GL has written to the native image.
 */
class SyncResourceTracker : public ResourceTracker
{
public:
  /**
   * Constructor
   */
  SyncResourceTracker( Integration::GlSyncAbstraction& glSyncAbstraction,
                       SceneGraph::RenderMessageDispatcher& renderMessageDispatcher);

  /**
   * Destructor
   */
  virtual ~SyncResourceTracker();

  /**
   * Second stage initialization.
   * Creates a RenderTracker object to handle fence sync
   */
  virtual void Initialize();

  /**
   * first stage destruction
   * Called from CompleteStatusManager when the object is about to be deleted
   */
  virtual void OnDestroy();

   /**
   * Get the render tracker.
   * @return The render tracker
   */
  SceneGraph::RenderTracker* GetRenderTracker();

protected:
  /**
   * Do the reset. Resets the RenderTracker
   */
  virtual void DoReset();

  /**
   * Ignore complete status from update thread
   */
  virtual void DoSetComplete();

  /**
   * Do the IsComplete. Allows derived types to perform their own handling
   */
  virtual bool DoIsComplete();

private:

  /**
   * Undefined Copy Constructor
   */
  SyncResourceTracker(const SyncResourceTracker& rhs);

  /**
   * @brief Undefined Assignment Operator
   */
  SyncResourceTracker& operator=(const SyncResourceTracker& rhs);

private:
  Integration::GlSyncAbstraction& mGlSyncAbstraction; ///< The synchronisation interface
  SceneGraph::RenderMessageDispatcher& mRenderMessageDispatcher; ///< RenderManager message dispatcher
  SceneGraph::RenderTracker* mRenderTracker; ///< The GL Fence Sync tracker object
};

} // Internal
} // Dali

#endif // __DALI_INTERNAL_SYNC_RESOURCE_TRACKER_H__
