#ifndef __DALI_SLOT_DELEGATE_H__
#define __DALI_SLOT_DELEGATE_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/connection-tracker.h>

namespace Dali DALI_IMPORT_API
{

/**
 * SlotDelegates can be used to connect member functions to signals, without inheriting from SlotDelegateInterface.
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

/**
 * A template for SlotDelegates with no parameters or return value.
 */
template <typename Slot>
class SlotDelegate
{
public:

  /**
   * Constructor
   * @param[in] slot The object with a callback.
   */
  SlotDelegate( Slot* slot )
  : mSlot( slot )
  {
  }

  /**
   * Non-virtual destructor.
   */
  ~SlotDelegate()
  {
  }

  /**
   * Disconnect all signals from this object
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
   * Retrieve the slot object.
   * @return The object with a callback.
   */
  Slot* GetSlot()
  {
    return mSlot;
  }

  /**
   * Retrieve the connection tracker component.
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

  Slot* mSlot;

  // Use composition instead of inheritance (virtual methods don't mix well with templates)
  ConnectionTracker mConnectionTracker;
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_SLOT_DELEGATE_H__
