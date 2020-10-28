#ifndef DALI_INTERNAL_OBJECT_CONNECTOR_H
#define DALI_INTERNAL_OBJECT_CONNECTOR_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/intrusive-ptr.h>

namespace Dali
{

namespace Internal
{

/**
 * @brief Wrapper class which helps managing intrusive pointer assignments and Connect / Disconnect.
 *
 * Should be used with objects that implement Dali::Internal::Connectable
 */
template<typename Object>
class ObjectConnector
{
public:
  using ObjectPtr = IntrusivePtr<Object>;

  /**
   * @brief Default constructor.
   */
  ObjectConnector() = default;

  /**
   * @brief Destructor.
   */
  ~ObjectConnector() = default;

  /**
   * @brief Copy constructor
   */
  ObjectConnector( const ObjectConnector& connector )
  : mObject( connector.mObject )
  {
  }

  /**
   * @brief Assignment operator
   */
  ObjectConnector& operator=( const ObjectConnector& connector )
  {
    this->mObject = connector.mObject;
    return *this;
  }

  /**
   * @brief Returns a smart pointer to the object
   * @return a smart pointer to the object
   */
  ObjectPtr Get() const
  {
    return mObject;
  }

  /**
   * @brief Assigns the object, calling Connect and Disconnect methods accordingly, taking onScene into account.
   * @param [in] object smart pointer to a object
   * @param [in] onScene whether the object is used on stage or not
   */
  void Set( Object& object, bool onScene )
  {
    if ( mObject.Get() != &object )
    {
      // Disconnect from old object
      if ( mObject && onScene )
      {
        mObject->Disconnect();
      }

      mObject = &object;

      // Connect to new object
      if ( mObject && onScene )
      {
        mObject->Connect();
      }
    }
  }

  /**
   * @brief Manages connection reference count.
   *
   * Must be called from owner when connected to the scene.
   */
  void OnSceneConnect()
  {
    if ( mObject )
    {
      mObject->Connect();
    }
  }

  /**
   * @brief Manages connection reference count.
   *
   * Must be called from owner when disconnecting from the scene.
   */
  void OnSceneDisconnect()
  {
    if ( mObject )
    {
      mObject->Disconnect();
    }
  }

private: //data
  ObjectPtr mObject;  ///< intrusive pointer to the Object. ObjectConnector owns this.

};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_OBJECT_CONNECTOR_H
