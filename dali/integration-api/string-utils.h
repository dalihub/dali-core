#pragma once

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

// EXTERNAL INCLUDES
#include <functional>
#include <string>
#include <string_view>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-map.h>

namespace Dali
{
namespace Integration
{

/**
 * @brief Convert a Dali::String to a std::string.
 *
 * This creates a copy of the string data. For internal use
 * where the implementation needs to work with std::string.
 *
 * @param[in] string The Dali::String
 * @return A std::string containing the same data
 */
inline std::string ToStdString(const String& string)
{
  return std::string(string.CStr(), string.Size());
}

/**
 * @brief Convert a Dali::String to a std::string by moving.
 *
 * This moves the string data without copying. The source Dali::String
 * will be left in a valid but empty state. For internal use where the
 * implementation needs to work with std::string efficiently.
 *
 * @param[in] string The Dali::String to move from
 * @return A std::string containing the moved data
 */
DALI_CORE_API std::string ToStdString(String&& string);

/**
 * @brief Convert a Dali::StringView to a std::string.
 *
 * This creates a copy of the viewed data. For internal use
 * where the implementation needs to work with std::string.
 *
 * @param[in] view The Dali::StringView
 * @return A std::string containing the same data
 */
inline std::string ToStdString(const StringView& view)
{
  const char* data = view.Data();
  return data ? std::string(data, view.Size()) : std::string();
}

/**
 * @brief Convert a Dali::Property::Value to a std::string.
 *
 * Extracts the string from a Property::Value without copying the
 * Property::Value wrapper itself. The string data is still copied
 * internally (no const-ref getter exists on Property::Value).
 *
 * @param[in] value The Property::Value (must be of type STRING)
 * @return A std::string containing the same data
 */
DALI_CORE_API std::string ToStdString(const Property::Value& value);

/**
 * @brief Convert a std::string to a Dali::Property::Value of type STRING.
 *
 * This creates a Dali::String from the std::string and wraps it
 * in a Property::Value. For internal use where the implementation
 * needs to produce Property::Value from std::string.
 *
 * @param[in] str The std::string to convert
 * @return A Property::Value containing the string data
 */
inline Property::Value ToPropertyValue(std::string str)
{
  Dali::String daliString(StringView(str.data(), str.size()));
  return Property::Value(std::move(daliString));
}

/**
 * @brief Extract a std::string from a Property::Value conditionally.
 *
 * Restores the one-liner `if(value.Get(str))` pattern that existed
 * before the Dali::String migration. Returns true if the value was
 * a string and was extracted successfully.
 *
 * @param[in] value The Property::Value to extract from
 * @param[out] out The std::string to write to on success
 * @return true if the value was a string and was extracted
 */
DALI_CORE_API bool GetStdString(const Property::Value& value, std::string& out);

/**
 * @brief Add a std::string to a Property::Array.
 *
 * Wraps the recurring `array.Add(ToDaliStringView(str))` pattern.
 *
 * @param[in,out] array The array to append to
 * @param[in] str The std::string to add
 * @return A reference to the array
 */
inline Property::Array& AddToArray(Property::Array& array, const std::string& str)
{
  array.Add(Property::Value(StringView(str.data(), str.size())));
  return array;
}

/**
 * @brief Add string key, string value pair to a Property::Map
 *
 * This converts the key and the value to a String type, and
 * adds them to the map.
 * @param[in,out] map The map to modify.
 * @param[in] key The string key to use
 * @param[in] value The string to use as a value
 * @return A reference to the map
 */
inline Property::Map& InsertToMap(Property::Map& map, const std::string& key, const std::string& value)
{
  Dali::String keyString(StringView(key.data(), key.size()));
  Dali::String valueString(StringView(value.data(), value.size()));
  map.Insert(std::move(keyString), Property::Value(std::move(valueString)));
  return map;
}

/**
 * @brief Add string key, string value pair to a Property::Map
 *
 * This converts the key and the value to a String type, and
 * adds them to the map.
 * @param[in,out] map The map to modify.
 * @param[in] key The string key to use
 * @param[in] value The string to use as a value
 * @return A reference to the map
 */
inline Property::Map& InsertToMap(Property::Map& map, const std::string& key, Property::Value value)
{
  Dali::String keyString(StringView(key.data(), key.size()));
  map.Insert(std::move(keyString), value);
  return map;
}

/**
 * @brief Add string_view key, string value pair to a Property::Map
 *
 * Overload for std::string_view keys. Useful when keys are
 * compile-time constants or std::string_view variables.
 *
 * @param[in,out] map The map to modify.
 * @param[in] key The string_view key to use
 * @param[in] value The string to use as a value
 * @return A reference to the map
 */
inline Property::Map& InsertToMap(Property::Map& map, std::string_view key, const std::string& value)
{
  Dali::String keyString(StringView(key.data(), key.size()));
  Dali::String valueString(StringView(value.data(), value.size()));
  map.Insert(std::move(keyString), Property::Value(std::move(valueString)));
  return map;
}

/**
 * @brief Add string_view key, Property::Value pair to a Property::Map
 *
 * Overload for std::string_view keys. Useful when keys are
 * compile-time constants or std::string_view variables.
 *
 * @param[in,out] map The map to modify.
 * @param[in] key The string_view key to use
 * @param[in] value The Property::Value to insert
 * @return A reference to the map
 */
inline Property::Map& InsertToMap(Property::Map& map, std::string_view key, Property::Value value)
{
  Dali::String keyString(StringView(key.data(), key.size()));
  map.Insert(std::move(keyString), std::move(value));
  return map;
}

/**
 * @brief Convert a Dali::StringView to a std::string_view.
 *
 * This is a lightweight operation that creates a view over the
 * same data without copying.
 *
 * @param[in] view The Dali::StringView
 * @return A std::string_view over the same data
 */
inline std::string_view ToStdStringView(const StringView& view)
{
  const char* data = view.Data();
  return data ? std::string_view(data, view.Size()) : std::string_view();
}

/**
 * @brief Convert a Dali::String to a std::string_view.
 *
 * This is a lightweight operation that creates a view over the
 * String's data without copying.
 *
 * @param[in] string The Dali::String
 * @return A std::string_view over the string's data
 */
inline std::string_view ToStdStringView(const String& string)
{
  return std::string_view(string.CStr(), string.Size());
}

/**
 * @brief Create a Dali::StringView from a std::string.
 *
 * This is a lightweight operation that creates a view over the
 * std::string's data without copying.
 *
 * @param[in] string The std::string
 * @return A Dali::StringView over the string's data
 */
inline StringView ToDaliStringView(const std::string& string)
{
  return StringView(string.data(), string.size());
}

/**
 * @brief Create a Dali::StringView from a std::string_view.
 *
 * This is a lightweight operation that creates a view over the
 * same data without copying.
 *
 * @param[in] view The std::string_view
 * @return A Dali::StringView over the same data
 */
inline StringView ToDaliStringView(std::string_view view)
{
  return StringView(view.data(), view.size());
}

/**
 * @brief Convert a std::string to a Dali::String.
 *
 * This creates a copy of the string data.
 *
 * @param[in] string The std::string
 * @return A Dali::String containing the same data
 */
inline String ToDaliString(const std::string& string)
{
  return String(StringView(string.data(), string.size()));
}

/**
 * @brief Convert a std::string_view to a Dali::String.
 *
 * This creates a copy of the viewed data.
 *
 * @param[in] view The std::string_view
 * @return A Dali::String containing the same data
 */
inline String ToDaliString(std::string_view view)
{
  return String(StringView(view.data(), view.size()));
}

/**
 * @brief Convert a std::string to a Dali::String by moving.
 *
 * This moves the string data without copying. The source std::string
 * will be left in a valid but empty state.
 *
 * @param[in] string The std::string to move from
 * @return A Dali::String containing the moved data
 */
DALI_CORE_API String ToDaliString(std::string&& string);

/**
 * @brief Copy a Dali::String (no-op conversion for convenience).
 *
 * This is a convenience overload that allows code to work
 * with Dali::String without checking if conversion is needed.
 *
 * @param[in] str The Dali::String
 * @return A copy of the Dali::String
 */
inline String ToDaliString(const String& str)
{
  return String(str);
}

} // namespace Integration
} // namespace Dali

// Add std::hash specialization for Dali::StringView
// This enables use of Dali::StringView as key in std::unordered_map
// Internal only - not part of public API
template<>
struct std::hash<Dali::StringView>
{
  hash()  = default;
  ~hash() = default;

  std::size_t operator()(const Dali::StringView& sv) const noexcept
  {
    const char*      data = sv.Data();
    std::string_view stdSv(data ? data : "", sv.Size());
    return std::hash<std::string_view>()(stdSv);
  }
};

// Add std::hash specialization for Dali::String
// This enables use of Dali::String as key in std::unordered_map
// Internal only - not part of public API
template<>
struct std::hash<Dali::String>
{
  hash()  = default;
  ~hash() = default;

  std::size_t operator()(const Dali::String& str) const noexcept
  {
    // Convert to StringView and reuse its hash implementation
    Dali::StringView sv(str);
    return std::hash<Dali::StringView>()(sv);
  }
};
