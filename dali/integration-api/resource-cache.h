#ifndef __DALI_INTEGRATION_RESOURCE_CACHE_H__
#define __DALI_INTEGRATION_RESOURCE_CACHE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/integration-api/resource-request.h>
#include <dali/public-api/text/font-parameters.h>

namespace Dali
{

namespace Integration
{
/**
 * Used to determine why a resource IO operation has failed.
 */
enum ResourceFailure
{
  FailureUnknown,
  FailureFileNotFound,
  FailureInvalidPath
};

/**
 * Used to return loaded resources for rendering etc.
 */
typedef IntrusivePtr<Dali::RefObject> ResourcePointer;

/**
 * Abstract interface to receive notifications of resource IO operations.
 * This is used when pulling loaded resources from the PlatformAbstraction.
 */
class DALI_IMPORT_API ResourceCache
{
protected:

  /**
   * Virtual protected destructor, no deletion through this interface
   */
  virtual ~ResourceCache() {}

public:

  /**
   * Provide the results of a resource loading operation to the cache.
   * @param[in] id The unique ID of the load request.
   *               This should match an ID previously passed into PlatformAbstraction::LoadResource().
   *               LoadResponse() may be called multiple times with the same ID, when results are available
   *               at different stages e.g. a thumbnail image may be provided, before the full image is loaded.
   * @param[in] type The type of the resource.
   * @param[in] resource A pointer to a resource (Bitmap etc).
   * @param[in] status The current loading status. RESOURCE_LOADING and RESOURCE_PARTIALLY_LOADED indicate there are more responses to come, RESOURCE_COMPLETELY_LOADED indicates this is the last response for this id.
   */
  virtual void LoadResponse(ResourceId id, ResourceTypeId type, ResourcePointer resource, LoadStatus status) = 0;

  /**
   * Provide the results of a resource saving operation.
   * @param[in] id The unique ID of the resource.
   *               This should match an ID previously passed into PlatformAbstraction::SaveResource().
   * @param[in] type The type of the resource.
   */
  virtual void SaveComplete(ResourceId id, ResourceTypeId type) = 0;

  /**
   * Report that a resource loading operation has failed.
   * @param[in] id The unique ID of the load request.
   *               This should match an ID previously passed into PlatformAbstraction::LoadResource().
   * @param[in] failure An error code, used to determine why the load failed.
   */
  virtual void LoadFailed(ResourceId id, ResourceFailure failure) = 0;

  /**
   * Report that a resource saving operation has failed.
   * @param[in] id The unique ID of the save request.
   *               This should match an ID previously passed into PlatformAbstraction::LoadResource().
   * @param[in] failure An error code, used to determine why the save failed.
   */
  virtual void SaveFailed(ResourceId id, ResourceFailure failure) = 0;
};


} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_RESOURCE_CACHE_H__
