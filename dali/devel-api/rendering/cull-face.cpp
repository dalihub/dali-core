/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/cull-face.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/image-actor-impl.h>

namespace Dali
{

void SetCullFace( ImageActor actor, CullFaceMode mode )
{
  GetImplementation( actor ).SetCullFace( mode );
}

CullFaceMode GetCullFace( ImageActor actor )
{
  return GetImplementation( actor ).GetCullFace();
}

} //namespace Dali
