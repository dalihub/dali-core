#ifndef __DALI_INTERNAL_COMPLETE_STATUS_MANAGER_H__
#define __DALI_INTERNAL_COMPLETE_STATUS_MANAGER_H__

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

#include <dali/devel-api/common/map-wrapper.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/update/common/scene-graph-buffers.h>

namespace Dali
{

namespace Integration
{
class GlSyncAbstraction;
}

namespace Internal
{
class ResourceManager;
class ResourceTracker;

namespace SceneGraph
{
class RenderMessageDispatcher;
}

/**
 * Class to manage resource tracking and completion status.
 *
 * Resources that are tracked are usually Framebuffer objects that are
 * being rendered to by a RenderOnce render task.
 *
 * These need to change completion status either when all resources used by
 * the framebuffer are complete, or when the framebuffer is backed by a native
 * image and the native image has been written to by GL.
 *
 * This class uses ResourceManager to determine the complete status
 * of non-tracked resources.
 */
class CompleteStatusManager
{
public:
  /**
   * Resource readiness state
   */
  enum CompleteState
  {
    NOT_READY, ///< Resource is not ready yet
    COMPLETE,  ///< Resource has finished loading, or is otherwise complete
    NEVER      ///< Resource will never be complete, e.g. load failed.
  };

  /**
   * Constructor.
   *
   * @param[in] glSyncAbstraction The GlSyncObject abstraction (for creating RenderTrackers)
   * @param[in] renderQueue The render queue (For passing ownership of RenderTrackers to RenderManager)
   * @param[in] renderManager The render manager
   * @param[in] query Update buffer query
   * @param[in] resourceManager The resource manager (For handling untracked resources)
   */
  CompleteStatusManager( Integration::GlSyncAbstraction& glSyncAbstraction,
                         SceneGraph::RenderMessageDispatcher& renderMessageDispatcher,
                         ResourceManager& resourceManager );

  /**
   * Destructor
   */
  ~CompleteStatusManager();

  /**
   * @param[in] id The resource id to track
   */
  void TrackResource( Integration::ResourceId id );

  /**
   * Stop tracking the resource ID. Will remove any resource / render trackers for this ID.
   * @param[in] id The resource id to stop tracking
   */
  void StopTrackingResource ( Integration::ResourceId id );

  /**
   * Get the resource tracker associated with this id
   * @param[in] id The resource id
   * @return a valid tracker if this resource is being tracked, or NULL.
   */
  ResourceTracker* FindResourceTracker( Integration::ResourceId id );

  /**
   * Gets the complete status of the resource.  If it has a tracker,
   * it returns the status from the resource tracker, otherwise it
   * returns the load status from the resource manager
   *
   * @param[in] id The resource id @return The complete state of the
   * resource
   */
  CompleteState GetStatus( Integration::ResourceId id );

private:
  /**
   * Factory method to create a ResourceTracker or GlResourceTracker for this resource id.
   * It creates a ResourceTracker for framebuffers without native images, or a GlResourceTracker
   * for framebuffers with native images that require Gl FenceSync.
   * @param[in] id The resource id
   */
  ResourceTracker* CreateResourceTracker(Integration::ResourceId id);

  typedef std::map< Integration::ResourceId, ResourceTracker* > TrackedResources;
  typedef TrackedResources::iterator TrackedResourcesIter;

  Integration::GlSyncAbstraction& mGlSyncAbstraction; ///< The synchronisation interface
  SceneGraph::RenderMessageDispatcher& mRenderMessageDispatcher; ///< Render thread message dispatcher
  ResourceManager& mResourceManager;   ///< The resource manager
  TrackedResources mTrackedResources;  ///< Tracked resources
};

} // Internal
} // Dali

#endif // __DALI_INTERNAL_COMPLETE_STATUS_MANAGER_H__
