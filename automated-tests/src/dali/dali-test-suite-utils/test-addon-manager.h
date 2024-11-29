#ifndef TEST_ADDON_MANAGER_H
#define TEST_ADDON_MANAGER_H

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

#include <dali/integration-api/addon-manager.h>
#include <dali/public-api/common/vector-wrapper.h>

#include <string>

namespace Test
{
class AddOnManager : public Dali::Integration::AddOnManager
{
public:
  /**
   * Initializes the test addon manager so that it can be used by DALi.
   */
  static void Initialize();

  /**
   * @brief Constructor, initialised by the Adaptor
   */
  AddOnManager() = default;

  /**
   * @brief Destructor
   */
  ~AddOnManager() override = default;

  std::vector<std::string> EnumerateAddOns() override;

  bool GetAddOnInfo(const std::string& name, Dali::AddOnInfo& info) override;

  std::vector<Dali::AddOnLibrary> LoadAddOns(const std::vector<std::string>& addonNames) override;

  Dali::AddOnLibrary LoadAddOn(const std::string& addonName, const std::string& libraryName) override;

  void* GetGlobalProc(const Dali::AddOnLibrary& addOnLibrary, const char* procName) override;

  void* GetInstanceProc(const Dali::AddOnLibrary& addOnLibrary, const char* procName) override;

  void RegisterAddOnDispatchTable(const Dali::AddOnDispatchTable* dispatchTable) override;

  void Start() override;

  void Resume() override;

  void Stop() override;

  void Pause() override;

  struct AddOnCacheEntry
  {
    std::string     name{};
    Dali::AddOnInfo info{};

    // library handle
    void* handle{nullptr};

    // main function pointers
    void (*GetAddOnInfo)(Dali::AddOnInfo&) = nullptr; ///< Returns AddOnInfo structure
    void* (*GetInstanceProc)(const char*)  = nullptr; ///< Returns pointer of instance function (member funtion)
    void* (*GetGlobalProc)(const char*)    = nullptr; ///< Returns pointer of global function (non-member function)

    void (*OnStart)()  = nullptr;
    void (*OnResume)() = nullptr;
    void (*OnPause)()  = nullptr;
    void (*OnStop)()   = nullptr;

    bool valid = false;
  };

  std::vector<AddOnCacheEntry> mAddOnCache;
};
} // Namespace Test

#endif // TEST_ADDON_MANAGER_H
