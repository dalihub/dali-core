#ifndef __DALI_BASE_OBJECT_H__
#define __DALI_BASE_OBJECT_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/signals/functor-delegate.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

class BaseHandle;

/**
 * @brief A base class for objects.
 * @SINCE_1_0.0
 */
class DALI_CORE_API BaseObject : public Dali::RefObject
{
public:

  /**
   * @brief Connects a void() functor to a specified signal.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] signalName Name of the signal to connect to
   * @param[in] functor The functor to copy
   * @return True if the signal was available
   * @pre The signal must be available in this object.
   */
  template <class T>
  bool ConnectSignal( ConnectionTrackerInterface* connectionTracker, const std::string& signalName, const T& functor )
  {
    return DoConnectSignal( connectionTracker, signalName, FunctorDelegate::New( functor ) );
  }

  /**
   * @copydoc Dali::BaseHandle::DoAction
   */
  bool DoAction(const std::string& actionName, const Property::Map& attributes);

  /**
   * @copydoc Dali::BaseHandle::GetTypeName
   */
  const std::string& GetTypeName() const;

  /**
   * @copydoc Dali::BaseHandle::GetTypeInfo
   */
  bool GetTypeInfo(Dali::TypeInfo& info) const;

public: // Not intended for application developers

  /**
   * @brief Not intended for application developers.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] signalName Name of the signal to connect to
   * @param[in] functorDelegate A newly allocated functor delegate (takes ownership)
   * @return True if the signal was available
   */
  bool DoConnectSignal( ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functorDelegate );

protected:

  /**
   * @brief Default constructor.
   * @SINCE_1_0.0
   */
  BaseObject();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   * @SINCE_1_0.0
   */
  virtual ~BaseObject();

  /**
   * @brief Registers the object as created with the Object registry.
   * @SINCE_1_0.0
   */
  void RegisterObject();

  /**
   * @brief Unregisters the object from Object registry.
   * @SINCE_1_0.0
   */
  void UnregisterObject();

private:

  // Not implemented
  DALI_INTERNAL BaseObject(const BaseObject& rhs);

  // Not implemented
  DALI_INTERNAL BaseObject& operator=(const BaseObject& rhs);
};

// Helpers for public-api forwarding methods

/**
 * @brief Gets the implementation of a handle.
 *
 * @SINCE_1_0.0
 * @param[in] handle The handle
 * @return A reference to the object the handle points at
 */
inline BaseObject& GetImplementation(Dali::BaseHandle& handle)
{
  DALI_ASSERT_ALWAYS( handle && "BaseObject handle is empty" );

  return handle.GetBaseObject();
}

/**
 * @brief Gets the implementation of a handle.
 *
 * @SINCE_1_0.0
 * @param[in] handle The handle
 * @return A reference to the object the handle points at
 */
inline const BaseObject& GetImplementation(const Dali::BaseHandle& handle)
{
  DALI_ASSERT_ALWAYS( handle && "BaseObject handle is empty" );

  return handle.GetBaseObject();
}

/**
 * @}
 */
} // namespace Dali

# endif // __DALI_BASE_OBJECT_H__
