#ifndef DALI_ADDONS_ADDON_BASE_H
#define DALI_ADDONS_ADDON_BASE_H

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

#include <dali/integration-api/addon-manager.h>
#include <dali/devel-api/addons/addon-dispatch-table.h>
#include <vector>

namespace Dali
{
namespace AddOns
{

/**
 * @class AddOnBase
 *
 * @brief AddOnBase is a base class for AddOns and should be used rather
 * than writing exported functions of the AddOn directly.
 *
 */
class AddOnBase
{
protected:

  /**
   * @brief Constructor
   */
  AddOnBase()
  {
    mSingleton = this;
  }

public:

  /**
   * @brief Destructor
   */
  virtual ~AddOnBase() = default;

  /**
   * @brief Retrieves AddOn info
   * @param[out] addonInfo AddOnInfo structure to fill by the function
   */
  virtual void GetAddOnInfo( Dali::AddOnInfo& addonInfo ) = 0;

  /**
   * @brief Returns a dispatch table for global functions.
   * @return Valid dispatch table object or nullptr
   */
  virtual DispatchTable* GetGlobalDispatchTable() = 0;

  /**
   * @brief Returns a dispatch table for instance functions
   * @return Valid dispatch table object or nullptr
   */
  virtual DispatchTable* GetInstanceDispatchTable() = 0;

  /**
   * @brief OnStart event.
   * It's optional and should be implemented by the AddOn when it's required to handle the event.
   */
  virtual void OnStart() {}

  /**
   * @brief OnResume event.
   * It's optional and should be implemented by the AddOn when it's required to handle the event.
   */
  virtual void OnResume() {}

  /**
   * @brief OnPause event.
   * It's optional and should be implemented by the AddOn when it's required to handle the event.
   */
  virtual void OnPause() {}

  /**
   * @brief OnStop event.
   * It's optional and should be implemented by the AddOn when it's required to handle the event.
   */
  virtual void OnStop() {}

  /**
   * @brief Getter of static singleton
   * @return Returns valid singleton pointer
   */
  static AddOnBase* Get()
  {
    return mSingleton;
  }

private:

  static AddOnBase* mSingleton;
};

/**
 * @brief The function is automatically added to the AddOn code
 * with use of REGISTER_ADDON_CLASS() macro
 * @return The valid pointer to the AddOnBase interface
 */
extern Dali::AddOns::AddOnBase* CreateAddOn();

/**
 * Fills AddOnInfo structure.
 * @param[out] info Reference to AddOnInfo structure
 */
template<class T>
void GetAddOnInfo( Dali::AddOnInfo& info )
{
  auto* addon = Dali::AddOns::AddOnBase::Get();
  addon->GetAddOnInfo( info );
}

/**
 * Returns pointer to AddOn's global function
 * @param[in] funcname Name of function
 * @return Valid pointer or nullptr if function not found.
 */
template<class T>
void* GetGlobalProc( const char* funcname )
{
  if( !funcname )
  {
    return nullptr;
  }
  auto* addon = Dali::AddOns::AddOnBase::Get();

  // AddOn must be initialised up to this point!
  if( !addon )
  {
    return nullptr;
  }

  static Dali::AddOns::DispatchTable* globalDispatchTable = addon->GetGlobalDispatchTable();

  if( globalDispatchTable )
  {
    return globalDispatchTable->Find( funcname );
  }
  return nullptr;
}

/**
 * Returns pointer to AddOn's instance function
 * @param[in] funcname Name of function
 * @return Valid pointer or nullptr if function not found.
 */
template<class T>
void* GetInstanceProc( const char* funcname )
{
  if( !funcname )
  {
    return nullptr;
  }

  auto* addon = Dali::AddOns::AddOnBase::Get();

  // AddOn must be initialised up to this point!
  if( !addon )
  {
    return nullptr;
  }

  static Dali::AddOns::DispatchTable* instanceDispatchTable = addon->GetInstanceDispatchTable();

  if( instanceDispatchTable )
  {
    return instanceDispatchTable->Find( funcname );
  }
  return nullptr;
}

/**
 * Lifecycle functions to export.
 * They will be instantiated only if macro REGISTER_ADDON_CLASS is used
 */
template<class T>
void OnStart()
{
  auto* addon = Dali::AddOns::AddOnBase::Get();
  addon->OnStart();
}

template<class T>
void OnPause()
{
  auto* addon = Dali::AddOns::AddOnBase::Get();
  addon->OnPause();
}

template<class T>
void OnResume()
{
  auto* addon = Dali::AddOns::AddOnBase::Get();
  addon->OnResume();
}

template<class T>
void OnStop()
{
  auto* addon = Dali::AddOns::AddOnBase::Get();
  addon->OnStop();
}

/**
 * @brief AddOn library internal constructor.
 */
inline void AddOnConstructorInternal()
{
  auto* addon = Dali::AddOns::CreateAddOn();

  Dali::AddOnInfo info{};
  addon->GetAddOnInfo( info );

  // Generate dispatch tables
  addon->GetGlobalDispatchTable();
  addon->GetInstanceDispatchTable();

  // Bind basic functions
  Dali::AddOnDispatchTable table;
  table.name = info.name;
  table.GetAddOnInfo = GetAddOnInfo<void>;
  table.GetGlobalProc = GetGlobalProc<void>;
  table.GetInstanceProc = GetInstanceProc<void>;
  table.OnStart = OnStart<void>;
  table.OnStop = OnStop<void>;
  table.OnResume = OnResume<void>;
  table.OnPause = OnPause<void>;

  // Register dispatch table
  Dali::Integration::AddOnManager::Get()->RegisterAddOnDispatchTable( &table );
}

} // namespace AddOns
} // namespace Dali

/**
 * Macro must be used in order to auto-register AddOn with the AddOnManager.
 * Note: The macro requires GCC/Clang compiler and currently only Linux-based environment
 * is supported.
 */
#define REGISTER_ADDON_CLASS( ADDON_CLASS_WITH_FULL_NAMESPACE ) \
namespace Dali { namespace AddOns { \
__attribute__((constructor)) void AddOnConstructor() { \
  AddOnConstructorInternal();\
}\
AddOnBase* AddOnBase::mSingleton = nullptr; \
AddOnBase* CreateAddOn() \
{\
  return new ADDON_CLASS_WITH_FULL_NAMESPACE();\
}\
}}

#endif // DALI_ADDON_BASE_H
