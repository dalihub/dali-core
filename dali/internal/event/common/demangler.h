#ifndef DALI_DEMANGLER_H
#define DALI_DEMANGLER_H

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
 *
 */

// EXTERNAL INCLUDES
#include <string>

namespace Dali
{
namespace Internal
{
/**
 * @brief Demangle a nested typeid name to its class name.
 * @param[in] typeIdName The type id name string to demangle.
 * @returns the class name ie "Actor" or an empty string
 */
std::string DemangleClassName(const char* typeIdName);

/**
 * @brief Demangle any typeid name to a full human-readable type name.
 *
 * Unlike DemangleClassName(), which extracts only the last component of a
 * nested class name for use by the TypeRegistry, this function produces the
 * full qualified type name suitable for use as a stable type identifier key.
 *
 * Examples (GCC/Clang mangled → demangled):
 *   "i"              → "int"
 *   "f"              → "float"
 *   "N5Dali5ActorE"  → "Dali::Actor"
 *
 * Examples (MSVC — already human-readable, just normalised):
 *   "int"               → "int"
 *   "class Dali::Actor" → "Dali::Actor"
 *   "struct Foo::Bar"   → "Foo::Bar"
 *
 * @param[in] typeIdName The raw string from typeid(T).name().
 * @returns The full demangled type name, or typeIdName unchanged if
 *          demangling fails (GCC/Clang only; MSVC never fails).
 */
std::string DemangleTypeInfoName(const char* typeIdName);

} // namespace Internal

} // namespace Dali

#endif // DALI_DEMANGLER_H
