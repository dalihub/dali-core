#ifndef __DALI_INTERNAL_RESOURCE_TICKET_H__
#define __DALI_INTERNAL_RESOURCE_TICKET_H__

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/map-wrapper.h>
#include <dali/public-api/common/loading-state.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/event/resources/resource-type-path.h>
#include <dali/integration-api/resource-cache.h>

namespace Dali
{

namespace Internal
{

class ResourceTicket;
typedef IntrusivePtr<ResourceTicket> ResourceTicketPtr;
typedef std::vector<ResourceTicketPtr> ResourceTicketContainer;

class ResourceTicketObserver;
class ResourceTicketLifetimeObserver;

/**
 * @brief ResourceTicket records the lifetime of a resource loading request.
 *
 * When a resource is requested from ResourceClient, you get a ticket.
 * The state of the loading operation, can be observed using the ticket.
 * After the ticket is destroyed, the resource will be discarded.
 *
 * Multi-threading note: this class should be used in the main thread only
 * i.e. not from within Dali::Integration::Core::Render().
 */
class ResourceTicket : public RefObject
{
public:

  /**
   * Create a resource request.
   * This should only be done by the ResourceTicketRegistry.
   * @param [in] observer The object which observes ticket lifetimes.
   * @param [in] id A unique ID for this ticket (loading request).
   * @param [in] typePath The resource Type & Path pair.
   */
  ResourceTicket( ResourceTicketLifetimeObserver& observer, unsigned int id, ResourceTypePath& typePath );

  /**
   * Retrieve the unique ID of the resource request.
   * This can be shared with Nodes etc. which will require the resource when rendering.
   * @return The unique ID for this request.
   */
  unsigned int GetId() const { return mId; }

  /**
   * Retrieve the type and path of the resource request.
   * @return The type and path.
   */
  const ResourceTypePath& GetTypePath() const { return mTypePath; }

  /**
   * Query the state of the resource loading request.
   * @return The state.
   */
  Dali::LoadingState GetLoadingState() const { return mLoadingState; }

  /**
   * Adds an observer.
   * @param[in] observer The observer.
   */
  void AddObserver(ResourceTicketObserver& observer);

  /**
   * Removes an observer.
   * @param[in] observer The observer.
   */
  void RemoveObserver(ResourceTicketObserver& observer);

public: // Used by the ResourceClient

  /**
   * Called when a resource loads successfully.
   * The ResourceTicketObservers will be notified.
   */
  void LoadingSucceeded();

  /**
   * Called when a resource fails to load.
   * The ResourceTicketObservers will be notified.
   */
  void LoadingFailed();

  /**
   * Reset state to ResourceLoading.
   */
  void Loading();

  /**
   * Notification when a resource has been uploaded to GL.
   * The ResourceTicketObservers will be notified.
   */
  void Uploaded();

  /**
   * Called when a resource is saved successfully.
   * The ResourceTicketObservers will be notified.
   */
  void SavingSucceeded();

  /**
   * Called when a resource fails to save.
   * The ResourceTicketObservers will be notified.
   */
  void SavingFailed();

  /**
   * Called when the ResourceTicketLifetimeObserver is being destroyed.
   * This method should only be called during destruction of the Dali core.
   */
  void StopLifetimeObservation();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ResourceTicket();

private:

  /**
   * Undefined copy constructor.
   */
  ResourceTicket(const ResourceTicket&);

  /**
   * Undefined assignment operator.
   */
  ResourceTicket& operator=(const ResourceTicket& rhs);

private:

  ResourceTicketLifetimeObserver* mLifetimeObserver; ///< reference to the lifetime-observer; not owned

  unsigned int mId;
  ResourceTypePath mTypePath;

  Dali::LoadingState mLoadingState;

  // These ticket observers are not owned by the ticket.
  // Observers must remove themselves by calling RemoveObserver e.g. during destruction.
  typedef std::vector<ResourceTicketObserver*> ObserverContainer;
  typedef ObserverContainer::iterator ObserverIter;

  ObserverContainer mObservers;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RESOURCE_MANAGER_H__
