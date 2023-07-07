#ifndef DALI_ADDON_DISPATCH_TABLE_H
#define DALI_ADDON_DISPATCH_TABLE_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/common/vector-wrapper.h>
#include <algorithm>
#include <string>
#include <cstdint>

namespace Dali
{
namespace AddOns
{
/**
 * DispatchTable contains essential function pointers
 * needed to register the AddOn with AddOnManager.
 */
struct DispatchTable
{
  typedef void* FunctionPointer;
  /**
   * Struct Entry
   * The structure contains details of a single function binding
   */
  struct Entry
  {
    /**
     * @brief Assignment operator. Converts function pointer into void*.
     * @param[in] funcPtr Function pointer
     * @return Reference to self
     */
    template<class T>
    Entry& operator=(T funcPtr)
    {
      functionPtr = reinterpret_cast<void*>(funcPtr);
      if(index < 0)
      {
        index = int32_t(table->entries.size());
        table->entries.emplace_back(*this);
      }
      else
      {
        table->entries[index].functionPtr = reinterpret_cast<void*>(funcPtr);
      }
      return *this;
    }

    std::string    functionName{};       ///< Name of function
    void*          functionPtr{nullptr}; ///< Function pointer
    DispatchTable* table{nullptr};       ///< DispatchTable associated with entry
    int32_t        index = -1;           ///< Index within the dispatch table
  };

  /**
   * @brief Accessor of an indexed entry from the dispatch table
   * @param[in] functionName name of function
   * @return Returns Entry object
   */
  Entry operator[](const char* functionName)
  {
    auto iter = std::find_if(entries.begin(), entries.end(), [functionName](Entry& entry) {
      if(entry.functionName == functionName)
      {
        return true;
      }
      return false;
    });

    if(iter == entries.end())
    {
      Entry retval;
      retval.table        = this;
      retval.functionName = functionName;
      return retval;
    }
    return Entry(*iter);
  }

  /**
   * @brief Tests whether cache is empty
   * @return True if empty, False otherwise
   */
  bool Empty() const
  {
    return entries.empty();
  }

  /**
   * @brief Returns size of cache
   * @return Size of cache
   */
  uint32_t Size() const
  {
    return entries.size();
  }

  /**
   * @brief Function pointer lookup
   * @param[in] funcName Name of function
   * @return valid pointer or nullptr if function not found
   */
  FunctionPointer Find(const char* funcName)
  {
    if(entries.empty())
    {
      return nullptr;
    }

    auto iter = std::find_if(entries.begin(), entries.end(), [funcName, entries = &this->entries](Entry& entry) {
      return (entry.functionName == funcName);
    });
    if(iter != entries.end())
    {
      return iter->functionPtr;
    }
    return nullptr;
  }

  std::vector<Entry> entries;
};

} // namespace AddOns

} // namespace Dali

#endif // DALI_ADDON_DISPATCH_TABLE_H
