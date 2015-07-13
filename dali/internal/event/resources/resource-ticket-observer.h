#ifndef __DALI_INTERNAL_RESOURCE_TICKET_OBSERVER_H__
#define __DALI_INTERNAL_RESOURCE_TICKET_OBSERVER_H__

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
#include <dali/public-api/object/ref-object.h>

namespace Dali
{

namespace Internal
{

class ResourceTicket;

typedef IntrusivePtr<ResourceTicket> ResourceTicketPtr;

/**
 * ResourceTicketObserver is used to observe the status of resource loading requests.
 */
class ResourceTicketObserver
{
public:

  /**
   * Called when a resource loads successfully.
   * @param[in] ticket The ticket for this resource.
   */
  virtual void ResourceLoadingSucceeded( const ResourceTicket& ticket ) = 0;

  /**
   * Called when a resource fails to load.
   * @param[in] ticket The ticket for this resource.
   */
  virtual void ResourceLoadingFailed( const ResourceTicket& ticket ) = 0;

  /**
   * Called when a resource is uploaded to graphics memory.
   * @param[in] ticket The ticket for this resource.
   */
  virtual void ResourceUploaded( const ResourceTicket& ticket ) = 0;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RESOURCE_TICKET_OBSERVER_H__
