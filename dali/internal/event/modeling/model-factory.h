#ifndef __DALI_INTERNAL_MODEL_FACTORY_H__
#define __DALI_INTERNAL_MODEL_FACTORY_H__

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
#include <dali/internal/event/modeling/model-impl.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/resources/resource-type-path-id-map.h>

namespace Dali
{

namespace Internal
{

/**
 * ModelFactory is an object that manages Model resource load requests
 */
class ModelFactory
{
public:

  /**
   * Create a ModelFactory
   * @param[in] resourceClient This is used to request new model resources.
   */
  ModelFactory(ResourceClient& resourceClient);

  /**
   * Default destructor
   */
  virtual ~ModelFactory();

  /**
   * Initialises an Model object and an associated resource load request
   * @param [in] filename   The path of the model on the filesystem
   * @return                A ticket for the resource
   */
  ResourceTicketPtr Load(const std::string& filename);

private:

  // Undefined
  ModelFactory( const ModelFactory& );

  // Undefined
  ModelFactory& operator=( const ModelFactory& rhs );

private:
  ResourceClient& mResourceClient;
  ResourceTypePathIdMap mResourceTypePathIdMap; ///< A map of resource IDs sorted by ResourceTypePath
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_MODEL_FACTORY_H__
