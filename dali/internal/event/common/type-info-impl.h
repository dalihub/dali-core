#ifndef __DALI_INTERNAL_TYPE_INFO_H__
#define __DALI_INTERNAL_TYPE_INFO_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/type-info.h>

namespace Dali
{

namespace Internal
{

/**
 * A TypeInfo class to support registered type creation, and introspection of available
 * actions and signal connection.
 * It also supports doing actions and connecting signal handlers. Note this is properly
 * conducted through the BaseHandle interface which uses the TypeRegistry to walk
 * all base classes.
 */
class TypeInfo : public BaseObject
{
public:
  /**
   * Create TypeInfo
   * @param [name] the registered name
   * @param [baseName] the base type registered name
   * @param [creator] the creator function for this type
   */
  TypeInfo(const std::string &name, const std::string &baseName, Dali::TypeInfo::CreateFunction creator);

  /**
   *
   */
  ~TypeInfo();

  /**
   * @copydoc Dali::TypeInfo::GetName
   */
  const std::string& GetName();

  /**
   * @copydoc Dali::TypeInfo::GetBaseName
   */
  const std::string& GetBaseName();

  /**
   * @copydoc TypeInfo::CreateFunction
   */
  BaseHandle CreateInstance();

  /**
   * @copydoc Dali::TypeInfo::GetCreator
   */
  Dali::TypeInfo::CreateFunction GetCreator();

  /**
   * @copydoc Dali::TypeInfo::GetActions
   */
  Dali::TypeInfo::NameContainer GetActions();

  /**
   * @copydoc Dali::TypeInfo::GetSignals
   */
  Dali::TypeInfo::NameContainer GetSignals();

  /*
   * Add an action function
   */
  void AddActionFunction( const std::string &actionName, Dali::TypeInfo::ActionFunction function );

  /*
   * Add a function for connecting a signal.
   * @param[in] signalName The name of the signal.
   * @param[in] function The function used for connecting to the signal.
   */
  void AddConnectorFunction( const std::string& signalName, Dali::TypeInfo::SignalConnectorFunctionV2 function );

  /**
   * Do an action on base object
   * @param [in] object The base object to act upon
   * @param [in] actionName The name of the desired action
   * @param [in] properties The arguments of the action
   * @return bool If the action could be executed
   */
  bool DoActionTo(BaseObject *object, const std::string &actionName, const std::vector<Property::Value> &properties);

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  bool ConnectSignal( BaseObject* object, ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functor );

private:

  typedef std::pair<std::string, Dali::TypeInfo::SignalConnectorFunctionV2 > ConnectionPairV2;
  typedef std::pair<std::string, Dali::TypeInfo::ActionFunction > ActionPair;

  typedef std::vector< ActionPair > ActionContainer;
  typedef std::vector< ConnectionPairV2 > ConnectorContainerV2;

  std::string mTypeName;
  std::string mBaseTypeName;
  Dali::TypeInfo::CreateFunction mCreate;
  ActionContainer mActions;
  ConnectorContainerV2 mSignalConnectors;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::TypeInfo& GetImplementation(Dali::TypeInfo& typeInfo)
{
  DALI_ASSERT_ALWAYS(typeInfo);

  BaseObject& handle = typeInfo.GetBaseObject();

  return static_cast<Internal::TypeInfo&>(handle);
}

inline const Internal::TypeInfo& GetImplementation(const Dali::TypeInfo& typeInfo)
{
  DALI_ASSERT_ALWAYS(typeInfo);

  const BaseObject& handle = typeInfo.GetBaseObject();

  return static_cast<const Internal::TypeInfo&>(handle);
}

} // namespace Dali

#endif // header
