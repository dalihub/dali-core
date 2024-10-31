#ifndef DALI_INTEGRATION_ADDON_MANAGER_H
#define DALI_INTEGRATION_ADDON_MANAGER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-handle.h>

// EXTERNAL EXCLUDES
#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Dali
{
// Type of extensions (may be used internally)
enum class AddOnType
{
  GENERIC,
  IMAGE_LOADER
};

/**
 * @brief Helper function building the version number as 32-bit integer.
 * The return value should be used to encode AddOnInfo::version field.
 *
 * @param[in] maj Major version number
 * @param[in] min Minor version number
 * @param[in] rev Revision version number
 * @return returns 32-bit version number
 */
constexpr uint32_t DALI_ADDON_VERSION(uint32_t maj, uint32_t min, uint32_t rev)
{
  return ((maj & 0xff) << 24) | ((min & 0xfff) << 16);
}

/**
 * Structure describes AddOn details
 */
struct AddOnInfo
{
  AddOnType type; /// may be use in order to classify extension
  void*     next; /// holds pointer to additional data-structures

  std::string name; /// Name of the extension
  uint32_t    version;

  /**
   * Structure contains details of build
   */
  struct BuildInfo
  {
    uint32_t libCoreVersion;
    uint32_t libAdaptorVersion;
    uint32_t libToolkitVersion;
  } buildInfo;
};

/**
 * The structure contains essential function pointers which AddOnManager
 * requires in order to use AddOns.
 */
struct AddOnDispatchTable
{
  std::string name;
  void (*GetAddOnInfo)(Dali::AddOnInfo&) = nullptr;
  void* (*GetGlobalProc)(const char*)    = nullptr;
  void* (*GetInstanceProc)(const char*)  = nullptr;

  // Lifecycle callbacks
  void (*OnStart)()  = nullptr;
  void (*OnResume)() = nullptr;
  void (*OnPause)()  = nullptr;
  void (*OnStop)()   = nullptr;
};

/**
 * The AddOnLibrary type represents fully opaque object which hides
 * the actual handle to the library and other related data.
 */
typedef void* AddOnLibrary;

namespace Integration
{
/**
 * AddOnManager class
 *
 * Handles DALi AddOn support. The object of AddOnManager exists as a singleton and
 * is created by the Adaptor. The AddOnManager is used by:
 *
 * 1) Application - query the AddOns and obtain AddOn interfaces
 * 2) DALi - handling lifecycle events
 * 3) AddOn - self-registering the AddOn dispatch table
 *
 * It is up to the implementation how the AddOn libraries are enumerated and opened. Any
 * caching (functions, open libraries) must be handled by the implementation.
 */
class DALI_CORE_API AddOnManager
{
protected:
  /**
   * @brief Constructor, initialised by the Adaptor
   */
  AddOnManager();

public:
  /**
   * @brief Destructor
   */
  virtual ~AddOnManager();

  // Functions called by the application
public:
  /**
   * @brief Retrieves list of the available AddOns
   * @return List of AddOn names
   */
  virtual std::vector<std::string> EnumerateAddOns() = 0;

  /**
   * @brief Returns AddOnInfo structure for specified AddOn name
   * @param[in] name Name of AddOn
   * @param[out]] info Output reference
   * @return True on success, False if extension info cannot be retrieved
   */
  virtual bool GetAddOnInfo(const std::string& name, AddOnInfo& info) = 0;

  /**
   * @brief Loads and initialises specified extensions
   * @param[in] extensionNames Array of extension names
   * @return vector of initialised extension handles
   */
  virtual std::vector<AddOnLibrary> LoadAddOns(const std::vector<std::string>& addonNames) = 0;

  /**
   * @brief Loads the specified addon from the specified library if it's not already cached
   * @param addonName Name of the AddOn to acquire
   * @param libraryName Name of the library to load the AddOn from
   * @return Returns a valid handle or nullptr
   */
  virtual AddOnLibrary LoadAddOn(const std::string& addonName, const std::string& libraryName) = 0;

  /**
   * @brief Loads AddOn with specified name
   * @param[in] addOnName Name of AddOn to be acquired
   * @return Returns a valid handle or nullptr
   */
  inline AddOnLibrary GetAddOn(const std::string& addonName)
  {
    return LoadAddOns({addonName})[0];
  }

  /**
   * @brief Returns AddOn global function pointer
   * @param[in] addOnLibrary valid AddOn library object
   * @param[in] procName Name of the function to retrieve
   * @return Pointer to the function or null if function doesn't exist
   */
  virtual void* GetGlobalProc(const Dali::AddOnLibrary& addOnLibrary, const char* procName) = 0;

  /**
   * @brief Returns addon instance function pointer
   * @param[in] addOnLibrary valid AddOn library object
   * @param[in] procName Name of the function to retrieve
   * @return Pointer to the function or null if function doesn't exist
   */
  virtual void* GetInstanceProc(const Dali::AddOnLibrary& addOnLibrary, const char* procName) = 0;

  /**
   * @brief Returns addon global function of specified type
   * @param[in] addOnLibrary valid AddOn library object
   * @param[in] procName Name of the function to retrieve
   * @return std::function object or null if function doesn't exist
   */
  template<class T>
  DALI_INTERNAL std::function<T> GetGlobalProc(const Dali::AddOnLibrary& addonlibrary, const char* procName)
  {
    auto ptr = GetGlobalProc(addonlibrary, procName);
    if(ptr)
    {
      return std::function<T>(*reinterpret_cast<T**>(&ptr));
    }
    return {};
  };

  /**
   * @brief Returns AddOn instance function of specified type
   * @param[in] addOnLibrary valid AddOn library object
   * @param[in] procName Name of the function to retrieve
   * @return std::function object or null if function doesn't exist
   */
  template<class T>
  DALI_INTERNAL std::function<T> GetInstanceProc(const Dali::AddOnLibrary& addOnLibrary, const char* procName)
  {
    auto ptr = GetInstanceProc(addOnLibrary, procName);
    if(ptr)
    {
      return std::function<T>(*reinterpret_cast<T**>(&ptr));
    }
    return {};
  };

  /**
   * @brief Invokes global function by name
   * @param[in] addOnLibrary valid AddOn library object
   * @param[in] functionName Name of function to be called
   * @param args[in] Arguments
   * @return Result of called function
   */
  template<class R, class... Args>
  DALI_INTERNAL R InvokeGlobalProc(AddOnLibrary addOnLibrary, const char* functionName, Args&&... args)
  {
    return std::move(GetGlobalProc<R(Args...)>(addOnLibrary, functionName)(args...));
  }

  // Lifecycle events, functions are called by the Adaptor
public:
  /**
   * @brief Lifecycle pause function
   */
  virtual void Pause() = 0;

  /**
   * @brief Lifecycle resume function
   */
  virtual void Resume() = 0;

  /**
   * @brief Lifecycle start function
   */
  virtual void Start() = 0;

  /**
   * @brief Lifecycle stop function
   */
  virtual void Stop() = 0;

  // Functions called by the AddOn
public:
  /**
   * @brief Registers the dispatch table with AddOnManager.
   *
   * The function must be called by the AddOn in order to self-register and add
   * the dispatch table. The platform-dependent implementation must override it
   * in order to store the dispatch table. The way the dispatch table is stored
   * depends on the implementation.
   *
   * @param[in] dispatchTable Pointer to the valid dispatch table
   */
  virtual void RegisterAddOnDispatchTable(const AddOnDispatchTable* dispatchTable) = 0;

  /**
   * @brief Retrieves AddOnManager singleton
   * @return pointer to the AddOnManager
   */
  static AddOnManager* Get();

protected:
  static AddOnManager* mSingleton; ///< Singleton storing an instance of AddOnManager
};
} // namespace Integration
} // namespace Dali

#endif // DALI_INTEGRATION_ADDON_MANAGER
