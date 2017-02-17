#ifndef __DALI_SLOT_DELEGATE_H__
#define __DALI_SLOT_DELEGATE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/connection-tracker.h>

namespace Dali
{
/**
 * @addtogroup dali_core_signals
 * @{
 */

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
 * @SINCE_1_0.0
 */
template <typename Slot>
class SlotDelegate
{
public:

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] slot The object with a callback
   */
  SlotDelegate( Slot* slot )
  : mSlot( slot )
  {
  }

  /**
   * @brief Non-virtual destructor.
   *
   * @SINCE_1_0.0
   */
  ~SlotDelegate()
  {
  }

  /**
   * @brief Disconnects all signals from this object.
   *
   * @SINCE_1_0.0
   */
  void DisconnectAll()
  {
    mConnectionTracker.DisconnectAll();
  }

  /**
   * @copydoc ConnectionTracker::GetConnectionCount
   */
  std::size_t GetConnectionCount() const
  {
    return mConnectionTracker.GetConnectionCount();
  }

  /**
   * @brief Retrieves the slot object.
   *
   * @SINCE_1_0.0
   * @return The object with a callback
   */
  Slot* GetSlot()
  {
    return mSlot;
  }

  /**
   * @brief Retrieves the connection tracker component.
   *
   * @SINCE_1_0.0
   * @return The connection tracker component
   */
  ConnectionTracker* GetConnectionTracker()
  {
    return &mConnectionTracker;
  }

private:

  SlotDelegate( const SlotDelegate& );            ///< undefined copy constructor @SINCE_1_0.0
  SlotDelegate& operator=( const SlotDelegate& ); ///< undefined assignment operator @SINCE_1_0.0

private:

  Slot* mSlot; ///< The slot object

  // Use composition instead of inheritance (virtual methods don't mix well with templates)
  ConnectionTracker mConnectionTracker; ///< A connection tracker
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_SLOT_DELEGATE_H__
