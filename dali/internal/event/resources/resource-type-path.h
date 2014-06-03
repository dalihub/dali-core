#ifndef __DALI_INTERNAL_RESOURCE_TYPE_PATH_H__
#define __DALI_INTERNAL_RESOURCE_TYPE_PATH_H__

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

// EXTERNAL INCLUDES
#include <memory>
// INTERNAL INCLUDES
#include <dali/integration-api/resource-types.h>

namespace Dali
{

namespace Internal
{

/**
 * A resource Type & Path pair. This is used by ResourceManager when creating tickets.
 * A new ticket will only be needed, if the same resource Type & Path has not already been requested.
 */
class ResourceTypePath
{
public:

  /**
   * Create a resource Type and Path pair.
   * @param[in] requestType The type of resource.
   * @param[in] requestPath The path of the resource.
   */
  ResourceTypePath(const Integration::ResourceType& requestType,
                   const std::string& requestPath)
  : type(requestType.Clone()),
    path(requestPath)
  {
  }

  /**
   * Copy constructor.
   * @param[in] typePath The Type & Path to copy.
   */
  ResourceTypePath(const ResourceTypePath& typePath)
  : type(typePath.type->Clone()),
    path(typePath.path)
  {
  }

  ~ResourceTypePath()
  {
    delete type;
  }

  /**
   * Less than operator.
   * @param[in] rhs The request to compare with.
   */
  bool operator<(const ResourceTypePath& rhs) const;

private:

  /**
   * Undefined assignment operator.
   * @param[in] rhs The resource request to copy.
   */
  ResourceTypePath& operator=(const ResourceTypePath& rhs);

public:

  const Integration::ResourceType* type;
  const std::string path;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RESOURCE_TYPE_PATH_H__
