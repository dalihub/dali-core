/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// HEADER
#include "renderer-devel.h"

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/renderer.h> // Dali::Internal::FrameBuffer

namespace Dali
{
namespace DevelRenderer
{

void SetFilteringMask( Renderer renderer, uint32_t mask )
{
  Internal::Renderer* rendererPtr = GetImplementation( renderer );
  rendererPtr->SetFilteringMask( mask );
}

uint32_t GetFilteringMask( const Renderer renderer )
{
  Internal::Renderer* rendererPtr = GetImplementation( renderer );
  return rendererPtr->GetFilteringMask();
}

}
}
