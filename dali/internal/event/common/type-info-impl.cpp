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

// CLASS HEADER
#include <dali/internal/event/common/type-info-impl.h>

// EXTERNAL INCLUDES
#include <algorithm> // std::find_if
#include <string>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

using std::find_if;

namespace
{

/*
 * Functor to find by name for vector of pairs
 */
template <typename T>
struct PairNameListFinder
{
  PairNameListFinder(const std::string &find)
  : mFind(find)
  {
  }

  bool operator()(T &p)
  {
    return p.first == mFind;
  }

private:

  const std::string& mFind;
};

} // namespace anon

namespace Dali
{

namespace Internal
{

TypeInfo::TypeInfo(const std::string &name, const std::string &baseTypeName, Dali::TypeInfo::CreateFunction creator)
  : mTypeName(name), mBaseTypeName(baseTypeName), mCreate(creator)
{
  DALI_ASSERT_ALWAYS(!name.empty() && "Type info construction must have a name");
  DALI_ASSERT_ALWAYS(!baseTypeName.empty() && "Type info construction must have a base type name");
}

TypeInfo::~TypeInfo()
{
}

BaseHandle TypeInfo::CreateInstance()
{
  BaseHandle ret;

  if(mCreate)
  {
    ret = mCreate();
  }
  return ret;
}

bool TypeInfo::DoActionTo(BaseObject *object, const std::string &actionName, const std::vector<Property::Value> &properties)
{
  bool done = false;

  ActionContainer::iterator iter = find_if(mActions.begin(), mActions.end(), PairNameListFinder<ActionPair>(actionName));

  if( iter != mActions.end() )
  {
    done = (iter->second)(object, actionName, properties);
  }
  else
  {
    DALI_LOG_WARNING("Type '%s' cannot do action '%s'\n", mTypeName.c_str(), actionName.c_str());
  }

  if(!done)
  {
    Dali::TypeInfo base = Dali::TypeRegistry::Get().GetTypeInfo( mBaseTypeName );
    while( base )
    {
      done = GetImplementation(base).DoActionTo(object, actionName, properties);
      if( done )
      {
        break;
      }
      base =  Dali::TypeRegistry::Get().GetTypeInfo( base.GetBaseName() );
    }
  }

  return done;
}

bool TypeInfo::ConnectSignal( BaseObject* object, ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( false );

  ConnectorContainerV2::iterator iter = find_if( mSignalConnectors.begin(), mSignalConnectors.end(),
                                                 PairNameListFinder<ConnectionPairV2>(signalName) );

  if( iter != mSignalConnectors.end() )
  {
    connected = (iter->second)( object, connectionTracker, signalName, functor );
  }

  return connected;
}

const std::string& TypeInfo::GetName()
{
  return mTypeName;
}

const std::string& TypeInfo::GetBaseName()
{
  return mBaseTypeName;
}

Dali::TypeInfo::CreateFunction TypeInfo::GetCreator()
{
  return mCreate;
}

Dali::TypeInfo::NameContainer TypeInfo::GetActions()
{
  Dali::TypeInfo::NameContainer ret;

  for(ActionContainer::iterator iter = mActions.begin(); iter != mActions.end(); ++iter)
  {
    ret.push_back(iter->first);
  }

  Dali::TypeInfo base = Dali::TypeRegistry::Get().GetTypeInfo( mBaseTypeName );
  while( base )
  {
    for(ActionContainer::iterator iter = GetImplementation(base).mActions.begin();
        iter != GetImplementation(base).mActions.end(); ++iter)
    {
      ret.push_back(iter->first);
    }

    base = Dali::TypeRegistry::Get().GetTypeInfo( base.GetBaseName() );
  }

  return ret;
}

Dali::TypeInfo::NameContainer TypeInfo::GetSignals()
{
  Dali::TypeInfo::NameContainer ret;

  for(ConnectorContainerV2::iterator iter = mSignalConnectors.begin(); iter != mSignalConnectors.end(); ++iter)
  {
    ret.push_back(iter->first);
  }

  Dali::TypeInfo base = Dali::TypeRegistry::Get().GetTypeInfo( mBaseTypeName );
  while( base )
  {
    for(ConnectorContainerV2::iterator iter = GetImplementation(base).mSignalConnectors.begin();
        iter != GetImplementation(base).mSignalConnectors.end(); ++iter)
    {
      ret.push_back(iter->first);
    }

    base = Dali::TypeRegistry::Get().GetTypeInfo( base.GetBaseName() );
  }

  return ret;
}

void TypeInfo::AddActionFunction( const std::string &actionName, Dali::TypeInfo::ActionFunction function )
{
  if( NULL == function)
  {
    DALI_LOG_WARNING("Action function is empty\n");
  }
  else
  {
    ActionContainer::iterator iter = std::find_if(mActions.begin(), mActions.end(),
                                                  PairNameListFinder<ActionPair>(actionName));

    if( iter == mActions.end() )
    {
      mActions.push_back( ActionPair( actionName, function ) );
    }
    else
    {
      DALI_LOG_WARNING("Action already exists in TypeRegistry Type", actionName.c_str());
    }
  }
}

void TypeInfo::AddConnectorFunction( const std::string& signalName, Dali::TypeInfo::SignalConnectorFunctionV2 function )
{
  if( NULL == function)
  {
    DALI_LOG_WARNING("Connector function is empty\n");
  }
  else
  {
    ConnectorContainerV2::iterator iter = find_if( mSignalConnectors.begin(), mSignalConnectors.end(),
                                                   PairNameListFinder<ConnectionPairV2>(signalName) );

    if( iter == mSignalConnectors.end() )
    {
      mSignalConnectors.push_back( ConnectionPairV2( signalName, function ) );
    }
    else
    {
      DALI_LOG_WARNING("Signal name already exists in TypeRegistry Type for signal connector function", signalName.c_str());
    }
  }
}

} // namespace Internal

} // namespace Dali
