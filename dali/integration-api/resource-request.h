#ifndef __DALI_INTEGRATION_RESOURCE_REQUEST_H__
#define __DALI_INTEGRATION_RESOURCE_REQUEST_H__

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

// INTERNAL INCLUDES
#include <dali/integration-api/resource-types.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{

class RefObject;

namespace Integration
{

// Resource Requests

/**
 * Used to return loaded resources for rendering etc.
 */
typedef IntrusivePtr<Dali::RefObject> ResourcePointer;

/**
 * Used to prioritize between loading operations.
 */
enum LoadResourcePriority
{
  LoadPriorityLowest,
  LoadPriorityLow,
  LoadPriorityNormal,
  LoadPriorityHigh,
  LoadPriorityHighest,
};

/**
 * Used to request a resource from the native filesystem.
 */
class ResourceRequest
{
public:

  /**
   * Used to request a resource be accessed from the native filesystem.
   * @param[in] newId         A unique ID for this request.
   * @param[in] resourceType  The type of resource requested. The implementation of
   *                          PlatformAbstraction::LoadResource() is responsible for
   *                          converting the native file(s) to this type
   *                          e.g. decoding a jpeg to a bitmap.
   * @param[in] resourcePath  The path of the resource; typically a filename.
   * @param[in] loadPriority  The priority of the request.
   */
  ResourceRequest(ResourceId newId,
                  const ResourceType& resourceType,
                  const std::string& resourcePath,
                  LoadResourcePriority loadPriority = LoadPriorityNormal)
  : id(newId),
    type(NULL),
    path(resourcePath),
    priority(loadPriority)
  {
    type = resourceType.Clone();
  }

  /**
   * Used to request or save a resource from/to the native filesystem.
   * @param[in] newId         A unique ID for this request.
   * @param[in] resourceType  The type of resource.
   * @param[in] resourcePath  The location of the resource / where the resource should be saved.
   * @param[in] resourcePtr   The resource to decode / save.
   * @param[in] savePriority  The priority of the request.
   */
  ResourceRequest(ResourceId newId,
                  const ResourceType& resourceType,
                  const std::string& resourcePath,
                  ResourcePointer resourcePtr,
                  LoadResourcePriority savePriority = LoadPriorityNormal)
  : id(newId),
    type(NULL),
    path(resourcePath),
    resource(resourcePtr),
    priority(savePriority)
  {
    type = resourceType.Clone();
  }

  /**
   * Copy constructor.
   * @param[in] request The resource request to copy.
   */
  ResourceRequest(const ResourceRequest& request)
  : id(request.id),
    type(NULL),
    path(request.path),
    resource(request.resource),
    priority(request.priority)
  {
    type = request.type->Clone();
  }

  /**
   * Assignment operator.
   * @param[in] rhs The resource request to copy.
   */
  ResourceRequest& operator=(const ResourceRequest& rhs)
  {
    if( this != &rhs )
    {
      id = rhs.id;
      if( type )
      {
        delete type;
      }
      type = rhs.type->Clone();
      path = rhs.path;
      resource = rhs.resource;
      priority = rhs.priority;
    }

    return *this;
  }

  /**
   * Non-virtual destructor; not intended as a base class
   */
  ~ResourceRequest()
  {
    delete type;
  }

  /**
   * Retrieve the resource ID
   * @return The ID
   */
  ResourceId GetId() const
  {
    return id;
  }

  /**
   * Retrieve the resource type
   * @return The type
   */
  ResourceType* GetType() const
  {
    return type;
  }

  /**
   * Retrieve the resource path
   * @return The path
   */
  const std::string& GetPath() const
  {
    return path;
  }

  /**
   * Retrieve the resource (for save and decode requests)
   * @return The resource
   */
  ResourcePointer GetResource() const
  {
    return resource;
  }

  /**
   * Retrieve the load priority
   * @return The priority
   */
  LoadResourcePriority GetPriority() const
  {
    return priority;
  }

private:

  ResourceId           id;
  ResourceType*        type;
  std::string          path;
  /** When saving resources or decoding them, the resource data will be passed
   *  through in a reference counted object here. When Loading, it will be null. */
  ResourcePointer      resource;
  LoadResourcePriority priority;
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_RESOURCE_REQUEST_H__
