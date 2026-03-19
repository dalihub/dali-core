/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

// CLASS INCLUDE
#include <dali/integration-api/dali-string-impl.h>

// EXTERNAL INCLUDES
#include <new>
#include <string>

namespace
{

/**
 * @brief Get a reference to the std::string in the given storage.
 */
inline std::string& GetStdStringFromStorage(void* storage)
{
  return *static_cast<std::string*>(storage);
}

/**
 * @brief Get a const reference to the std::string in the given storage.
 */
inline const std::string& GetStdStringFromStorage(const void* storage)
{
  return *static_cast<const std::string*>(storage);
}

} // unnamed namespace

namespace Dali
{
namespace Internal
{

StringImpl::StringImpl(String&& daliString) noexcept
{
  // Get the raw storage pointer from Dali::String
  void* srcStorage = daliString.GetRawStorage();

  // Use placement new to move-construct std::string from Dali::String's storage
  new(mStorage) std::string(std::move(GetStdStringFromStorage(srcStorage)));

  // Explicitly destroy the std::string in the source Dali::String
  GetStdStringFromStorage(srcStorage).~basic_string();

  // Reconstruct an empty std::string in the source to ensure its destructor
  // can be called safely later
  new(srcStorage) std::string();
}

StringImpl::~StringImpl()
{
  GetStdStringFromStorage(mStorage).~basic_string();
}

std::string StringImpl::Extract() noexcept
{
  std::string result(std::move(GetStdStringFromStorage(mStorage)));
  GetStdStringFromStorage(mStorage).~basic_string();
  new(mStorage) std::string(); // Leave in valid state for destructor
  return result;
}

std::string& StringImpl::GetString() noexcept
{
  return GetStdStringFromStorage(mStorage);
}

const std::string& StringImpl::GetString() const noexcept
{
  return GetStdStringFromStorage(mStorage);
}

String StringImpl::ToDaliString(std::string&& stdString) noexcept
{
  String result;

  // Destroy the empty std::string that the default constructor created
  void* destStorage = result.GetRawStorage();
  GetStdStringFromStorage(destStorage).~basic_string();

  // Move-construct the std::string into the Dali::String's storage
  new(destStorage) std::string(std::move(stdString));

  return result;
}

} // namespace Internal
} // namespace Dali
