#ifndef __DALI_SLOT_DELEGATE_H__
#define __DALI_SLOT_DELEGATE_H__

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

/**
 * @addtogroup CAPI_DALI_SIGNALS_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/connection-tracker.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief SlotDelegates can be used to connect member functions to signals, without inheriting from SlotDelegateInterface.
 *
 * Note that the object providing the member function is expected to own the SlotDelegate; therefore when the object
 * is destroyed, the SlotDelegate destructor will automatically disconnect.
 *
 * @code
 *
 * class Example // This does not inherit from SlotDelegateInterface!
 * {
 * public:
 *
 *   Example()
 *   : mSlotDelegate( this )
 *   {
 *   }
 *
 *   ~Example()
 *   {
 *     // mSlotDelegate disconnects automatically here
 *   }
 *
 *   void Animate()
 *   {
 *     Animation animation = Animation::New( 1.0f );
 *     animation.FinishedSignal().Connect( mSlotDelegate, &Example::OnAnimationFinished );
 *     animation.Play(); // fire & forget
 *   }
 *
 *   void OnAnimationFinished( Animation& animation )
 *   {
 *     std::cout << "Animation Finished!" << std::endl;
 *   }
 *
 * private:
 *
 *  SlotDelegate<Example> mSlotDelegate;
 *
 * };
 *
 * @endcode
 */
template <typename Slot>
class SlotDelegate
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in] slot The object with a callback.
   */
  SlotDelegate( Slot* slot )
  : mSlot( slot )
  {
  }

  /**
   * @brief Non-virtual destructor.
   *
   */
  ~SlotDelegate()
  {
  }

  /**
   * @brief Disconnect all signals from this object.
   *
   */
  void DisconnectAll()
  {
    mConnectionTracker.DisconnectAll();
  }

  /**
   * @copydoc ConnectionTrackerInterface::GetConnectionCount
   */
  std::size_t GetConnectionCount() const
  {
    return mConnectionTracker.GetConnectionCount();
  }

  /**
   * @brief Retrieve the slot object.
   *
   * @return The object with a callback.
   */
  Slot* GetSlot()
  {
    return mSlot;
  }

  /**
   * @brief Retrieve the connection tracker component.
   *
   * @return The connection tracker component.
   */
  ConnectionTracker* GetConnectionTracker()
  {
    return &mConnectionTracker;
  }

private:

  SlotDelegate( const SlotDelegate& );            ///< undefined copy constructor
  SlotDelegate& operator=( const SlotDelegate& ); ///< undefined assignment operator

private:

  Slot* mSlot; ///< The slot object

  // Use composition instead of inheritance (virtual methods don't mix well with templates)
  ConnectionTracker mConnectionTracker; ///< A connection tracker
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_SLOT_DELEGATE_H__
