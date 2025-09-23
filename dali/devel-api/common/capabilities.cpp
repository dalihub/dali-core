/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/devel-api/common/capabilities.h>
#include <dali/internal/event/common/thread-local-storage.h>

namespace Dali
{
namespace Capabilities
{
bool IsBlendEquationSupported(BlendEquation::Type blendEquation)
{
  return IsBlendEquationSupported(static_cast<DevelBlendEquation::Type>(blendEquation));
}

bool IsBlendEquationSupported(DevelBlendEquation::Type blendEquation)
{
  Dali::Internal::ThreadLocalStorage& tls = Dali::Internal::ThreadLocalStorage::Get();
  return tls.IsBlendEquationSupported(blendEquation);
}

} // namespace Capabilities

} // namespace Dali