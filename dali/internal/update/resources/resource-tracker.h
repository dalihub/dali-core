#ifndef __DALI_INTERNAL_RESOURCE_TRACKER_H__
#define __DALI_INTERNAL_RESOURCE_TRACKER_H__

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

namespace SceneGraph
{
class RenderTracker;
class RenderMessageDispatcher;
}

/**
 * Class to track completion of a resource.
 * Created for resource objects that can change completion status during update passes,
 * such as chained framebuffers. This base class is designed to be used on resources that
 * change completion in the update thread.
 */
class ResourceTracker
{
public:
  /**
   * Constructor
   */
  ResourceTracker();

  /**
   * Destructor
   */
  virtual ~ResourceTracker();

  /**
   * Second stage initialization
   */
  virtual void Initialize();

  /**
   * first stage destruction
   * Called from CompleteStatusManager when the object is about to be deleted
   */
  virtual void OnDestroy();

  /**
   * Reset the tracker
   */
  void Reset();

  /**
   * Set the tracker to be complete (Derived types may ignore this)
   */
  void SetComplete( );

  /**
   * @return TRUE if the resource is complete
   */
  bool IsComplete();

protected:
  /**
   * Do the reset. Allows derived types to handle the reset themselves.
   */
  virtual void DoReset();

  /**
   * Do the SetComplete. Allows derived types to handle the completeness themselves.
   */
  virtual void DoSetComplete();

  /**
   * Do the IsComplete. Allows derived types to perform their own handling
   */
  virtual bool DoIsComplete();

  bool mComplete; ///< TRUE if tracked resource has completed

private:
  /**
   * Undefined Copy Constructor and assignment operator
   */
  ResourceTracker(const ResourceTracker& rhs);
  ResourceTracker& operator=(const ResourceTracker& rhs);
};



} // Internal
} // Dali

#endif // __DALI_INTERNAL_RESOURCE_TRACKER_H__
