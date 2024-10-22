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

#include "test-addon-manager.h"

#include <dali-test-suite-utils.h>
#include <dlfcn.h>

#include <cstring>

#ifndef ADDON_LIBS_PATH
#define ADDON_LIBS_PATH ""
#endif

namespace Dali
{
namespace Test
{
std::vector<std::string> AddOnManager::EnumerateAddOns()
{
  std::string listFileName(ADDON_LIBS_PATH);
  listFileName += "/addons.txt";

  // Read list of available test addons
  tet_printf("Enumerating addons, file: %s\n", listFileName.c_str());
  std::vector<std::string> addons{};
  auto*                    fin     = fopen(listFileName.c_str(), "r");
  char*                    lineBuf = new char[256];
  memset(lineBuf, 0, 256);
  size_t n = 256;
  while(getline(&lineBuf, &n, fin) > 0)
  {
    char* c = lineBuf;
    while(*c)
    {
      if(*c == '\n' || *c == '\r')
      {
        *c = 0;
        break;
      }
      ++c;
    }
    tet_printf("Adding %s\n", lineBuf);
    addons.emplace_back(lineBuf);
    memset(lineBuf, 0, 256);
  }
  fclose(fin);
  delete[] lineBuf;
  std::vector<std::string> retval{};
  // Open addons
  for(auto& name : addons)
  {
    std::string path(ADDON_LIBS_PATH);
    path += "/";
    path += name;

    mAddOnCache.emplace_back();
    mAddOnCache.back().handle = dlopen(path.c_str(), RTLD_DEEPBIND | RTLD_LAZY);
    if(!mAddOnCache.back().handle)
    {
      mAddOnCache.back().valid = false;
      tet_printf("Can't open addon lib: %s\n", path.c_str());
      continue;
    }
    // Here addon must self register
    if(!mAddOnCache.back().valid)
    {
      puts("Addon invalid!");
    }
    else
    {
      tet_printf("Valid AddOn: %s\n", mAddOnCache.back().name.c_str());
      retval.emplace_back(mAddOnCache.back().name);
    }
  }

  return retval;

  /**
   * Check for self-registering addons
   */
}

void AddOnManager::RegisterAddOnDispatchTable(const AddOnDispatchTable* dispatchTable)
{
  // Register the dispatch table
  auto& entry = mAddOnCache.back();
  entry.name  = dispatchTable->name;
  tet_printf("Registering AddOn: %s\n", entry.name.c_str());
  entry.GetGlobalProc   = dispatchTable->GetGlobalProc;
  entry.GetInstanceProc = dispatchTable->GetInstanceProc;
  entry.GetAddOnInfo    = dispatchTable->GetAddOnInfo;
  entry.OnStart         = dispatchTable->OnStart;
  entry.OnStop          = dispatchTable->OnStop;
  entry.OnPause         = dispatchTable->OnPause;
  entry.OnResume        = dispatchTable->OnResume;
  entry.valid           = true;
}

bool AddOnManager::GetAddOnInfo(const std::string& name, AddOnInfo& info)
{
  auto retval = false;
  std::find_if(mAddOnCache.begin(), mAddOnCache.end(), [&retval, name, &info](AddOnCacheEntry& entry) {
    if(entry.name == name)
    {
      entry.GetAddOnInfo(info);
      retval = true;
      return true;
    }
    return false;
  });
  return retval;
}

std::vector<AddOnLibrary> AddOnManager::LoadAddOns(const std::vector<std::string>& addonNames)
{
  if(mAddOnCache.empty())
  {
    EnumerateAddOns();
  }

  std::vector<AddOnLibrary> retval{};
  for(auto& name : addonNames)
  {
    size_t index = 0;
    auto   iter  = std::find_if(mAddOnCache.begin(), mAddOnCache.end(), [&retval, name, &index](AddOnCacheEntry& entry) {
      index++;
      if(entry.name == name)
      {
        return true;
      }
      return false;
    });
    if(iter != mAddOnCache.end())
    {
      retval.emplace_back(*reinterpret_cast<void**>(&index));
    }
    else
    {
      retval.emplace_back(nullptr);
    }
  }

  return retval;
}

AddOnLibrary AddOnManager::LoadAddOn(const std::string& addonName, const std::string& libraryName)
{
  AddOnLibrary addOnLibrary = nullptr;
  size_t       index        = 0;
  auto         iter         = std::find_if(mAddOnCache.begin(), mAddOnCache.end(), [&addonName, &index](AddOnCacheEntry& entry) {
    index++;
    return (entry.name == addonName);
  });

  if(iter != mAddOnCache.end())
  {
    addOnLibrary = reinterpret_cast<void*>(index);
  }
  else
  {
    mAddOnCache.emplace_back();
    mAddOnCache.back().handle = dlopen(libraryName.c_str(), RTLD_DEEPBIND | RTLD_LAZY);
    if(!mAddOnCache.back().handle)
    {
      mAddOnCache.back().valid = false;
      tet_printf("Can't open addon lib: %s\n", libraryName.c_str());
    }
    // Here addon must self register
    if(!mAddOnCache.back().valid)
    {
      puts("Addon invalid!");
    }
    else
    {
      tet_printf("Valid AddOn: %s\n", mAddOnCache.back().name.c_str());
      addOnLibrary = reinterpret_cast<void*>(mAddOnCache.size());
    }
  }

  return addOnLibrary;
}

void* AddOnManager::GetGlobalProc(const Dali::AddOnLibrary& addOnLibrary, const char* procName)
{
  auto index = *reinterpret_cast<const size_t*>(&addOnLibrary);
  return mAddOnCache[index - 1].GetGlobalProc(procName);
}

void* AddOnManager::GetInstanceProc(const Dali::AddOnLibrary& addOnLibrary, const char* procName)
{
  auto index = *reinterpret_cast<const size_t*>(&addOnLibrary);
  return mAddOnCache[index - 1].GetInstanceProc(procName);
}

void AddOnManager::Start()
{
  for(auto& entry : mAddOnCache)
  {
    if(entry.OnStart)
    {
      entry.OnStart();
    }
  }
}

void AddOnManager::Resume()
{
  for(auto& entry : mAddOnCache)
  {
    if(entry.OnResume)
    {
      entry.OnResume();
    }
  }
}

void AddOnManager::Stop()
{
  for(auto& entry : mAddOnCache)
  {
    if(entry.OnStop)
    {
      entry.OnStop();
    }
  }
}

void AddOnManager::Pause()
{
  for(auto& entry : mAddOnCache)
  {
    if(entry.OnPause)
    {
      entry.OnPause();
    }
  }
}

} // namespace Test
} // namespace Dali
