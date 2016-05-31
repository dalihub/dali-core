#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H
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

#include <dali/public-api/dali-core.h>
#include <dali/devel-api/object/property-buffer.h>
#include <dali/devel-api/rendering/geometry.h>
#include <dali/devel-api/rendering/texture-set.h>
#include <dali/devel-api/rendering/sampler.h>
#include <dali/devel-api/rendering/renderer.h>

namespace Dali
{

Shader CreateShader();
TextureSet CreateTextureSet();
TextureSet CreateTextureSet( Image image );
PropertyBuffer CreatePropertyBuffer();
}

#endif // MESH_BUILDER_H
