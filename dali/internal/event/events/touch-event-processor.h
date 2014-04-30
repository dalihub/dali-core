#ifndef __DALI_INTERNAL_TOUCH_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_TOUCH_EVENT_PROCESSOR_H__

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
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/internal/event/common/proxy-object.h>

namespace Dali
{

class Actor;
struct Vector2;
struct Vector4;

namespace Integration
{
struct TouchEvent;
}

namespace Internal
{

class Actor;
class Stage;

/**
 * <h3>Multi-Touch Event Processing:</h3>
 *
 * The TouchEventProcessor processes touch events and emits the Touched signal on the hit actor (and
 * its parents).
 *
 * - Hit Testing & Touch Event Delivery are described in Dali::Actor.
 */
class TouchEventProcessor
{
public:

  /**
   * Create an event processor.
   * @param[in] stage The stage.
   */
  TouchEventProcessor( Stage& stage );

  /**
   * Non-virtual destructor; TouchEventProcessor is not a base class
   */
  ~TouchEventProcessor();

  /**
   * This function is called by the event processor whenever a touch event occurs.
   * @param[in] event The touch event that has occurred.
   */
  void ProcessTouchEvent( const Integration::TouchEvent& event );

private:

  // Undefined
  TouchEventProcessor(const TouchEventProcessor&);

  // Undefined
  TouchEventProcessor& operator=(const TouchEventProcessor& rhs);

private:

  /**
   * Stores an actor pointer and connects/disconnects to any required signals appropriately when set/unset.
   */
  struct ActorObserver : public ProxyObject::Observer
  {
  public:

    // Construction & Destruction

    /**
     * Constructor.
     */
    ActorObserver();

    /**
     * Non virtual destructor
     */
    ~ActorObserver();

    // Methods

    /**
     * Return the stored Actor pointer.
     * @return The Actor pointer.
     */
    Actor* GetActor();

    /**
     * Assignment operator.
     * This disconnects the required signals from the currently set actor and connects to the required
     * signals for the the actor specified (if set).
     */
    void SetActor( Actor* actor );

    /**
     * Resets the set actor and disconnects any connected signals.
     */
    void ResetActor();

  private:

    // Undefined
    ActorObserver( const ActorObserver& );
    ActorObserver& operator=( const ActorObserver& );

  private:

    /**
     * This will never get called as we do not observe objects that have not been added to the scene.
     * @param[in] proxy The proxy object.
     * @see ProxyObject::Observer::SceneObjectAdded()
     */
    virtual void SceneObjectAdded(ProxyObject& proxy) { }

    /**
     * This will be called when the actor is removed from the stage, we should clear and stop
     * observing it.
     * @param[in] proxy The proxy object.
     * @see ProxyObject::Observer::SceneObjectRemoved()
     */
    virtual void SceneObjectRemoved(ProxyObject& proxy);

    /**
     * This will be called when the actor is destroyed. We should clear the actor.
     * No need to stop observing as the object is being destroyed anyway.
     * @see ProxyObject::Observer::ProxyDestroyed()
     */
    virtual void ProxyDestroyed(ProxyObject& proxy);

  private:
    Actor* mActor;              ///< Raw pointer to an Actor.
    bool   mActorDisconnected;  ///< Indicates whether the actor has been disconnected from the scene
  };

  Stage& mStage; ///< Used to deliver touch events
  ActorObserver mLastPrimaryHitActor; ///< Stores the last primary point hit actor
  ActorObserver mLastConsumedActor; ///< Stores the last consumed actor
  Dali::RenderTask mLastRenderTask; ///< The RenderTask used for the last hit actor
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TOUCH_EVENT_PROCESSOR_H__
