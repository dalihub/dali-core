#ifndef __DALI_BASE_OBJECT_H__
#define __DALI_BASE_OBJECT_H__

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
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/signals/functor-delegate.h>

namespace Dali DALI_IMPORT_API
{

class BaseHandle;

/**
 * A base class for objects.
 */
class BaseObject : public Dali::RefObject
{
public:

  /**
   * Connects a void() functor to a specified signal.
   * @pre The signal must be available in this object.
   * @param [in] connectionTracker A connection tracker which can be used to disconnect.
   * @param [in] signalName Name of the signal to connect to.
   * @param [in] functor The functor to copy.
   * @return True if the signal was available.
   */
  template <class T>
  bool ConnectSignal( ConnectionTrackerInterface* connectionTracker, const std::string& signalName, const T& functor )
  {
    return DoConnectSignal( connectionTracker, signalName, FunctorDelegate::New( functor ) );
  }

  /**
   * @copydoc Dali::BaseHandle::DoAction
   */
  bool DoAction(const std::string& actionName, const std::vector<Property::Value>& attributes);

  /**
   * @copydoc Dali::BaseHandle::GetTypeName
   */
  std::string GetTypeName();

public: // Not intended for application developers

  /**
   * @copydoc Dali::BaseHandle::DoConnectSignal
   */
  bool DoConnectSignal( ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functorDelegate );

protected:

  /**
   * Default constructor.
   */
  BaseObject();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~BaseObject();

  /**
   * Registers the object as created with the Object registry
   */
  void RegisterObject();

  /**
   * Unregisters the object from Object registry.
   */
  void UnregisterObject();

private:

  // Not implemented
  DALI_INTERNAL BaseObject(const BaseObject& rhs);

  // Not implemented
  DALI_INTERNAL BaseObject& operator=(const BaseObject& rhs);
};

// Helpers for public-api forwarding methods

inline BaseObject& GetImplementation(Dali::BaseHandle& object)
{
  DALI_ASSERT_ALWAYS( object && "BaseObject handle is empty" );

  return object.GetBaseObject();
}

inline const BaseObject& GetImplementation(const Dali::BaseHandle& object)
{
  DALI_ASSERT_ALWAYS( object && "BaseObject handle is empty" );

  return object.GetBaseObject();
}

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_BASE_OBJECT_H__
