#ifndef DALI_INTEGRATION_TESTING_H
#define DALI_INTEGRATION_TESTING_H

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

#include <dali/public-api/common/dali-namespace.h>
#include <string>

namespace DALI_NAMESPACE
{
namespace Integration
{
/**
 * This functions should be used only within automated tests suit
 */
namespace Test
{
/**
 * @brief Generates internal tag for shader prefix
 * @param[in] prefix Prefix to generate tag for
 * @return Tagged prefix
 */
std::string GenerateTaggedShaderPrefix(std::string prefix);

} // Namespace Test
} // Namespace Integration
} //namespace DALI_NAMESPACE

#endif