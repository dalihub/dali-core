#ifndef __DALI_INTEGRATION_RESOURCE_DECLARATIONS_H__
#define __DALI_INTEGRATION_RESOURCE_DECLARATIONS_H__

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

namespace Dali
{

namespace Integration
{

/**
 * @brief Used to identify a resource loading operation.
 *
 * These unique ResourceId values can be used to identify a resource loading
 * transaction in core-adaptor communication.
 * A resource transaction is asynchronous and many can be in-flight
 * concurrently.
 * A ResourceId allows the core to track a resource transaction over its
 * lifetime and match an asynchronous completion notification to the
 * corresponding load request or to cancel the operation early.
 *
 * A resource transaction begins with a call to PlatformAbstraction::LoadResource()
 * Later asynchronous status notifications obtained by polling
 * PlatformAbstraction::GetResources() can be mapped to corresponding
 * LoadResource() invocations using the ResourceId value.
 * It is the core's responsibility to ensure that each invocation of
 * PlatformAbstraction::LoadResource() passes in a Request object with a unique
 * integer ResourceId.
 *
 * @sa Dali::Integration::PlatformAbstraction::LoadResource
 * Dali::Integration::PlatformAbstraction::GetResources
 * Dali::Integration::ResourceCache
 * Dali::Internal::ImageFactoryCache::RequestId
 */
typedef unsigned int ResourceId;
const ResourceId InvalidResourceId = (ResourceId)-1;

/**
 * Used to inform the current loading status
 */
enum LoadStatus
{
  RESOURCE_LOADING,            ///< There are missing resources, being loaded
  RESOURCE_PARTIALLY_LOADED,   ///< Enough resource has been loaded to start low quality rendering
  RESOURCE_COMPLETELY_LOADED,  ///< The resource has been completely loaded
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_RESOURCE_DECLARATIONS_H__
