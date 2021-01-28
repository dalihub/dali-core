#ifndef DALI_DEMANGLER_H
#define DALI_DEMANGLER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

} // namespace Internal

} // namespace Dali

#endif // DALI_DEMANGLER_H
