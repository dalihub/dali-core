#ifndef __DALI_INTEGRATION_PROFILING_H__
#define __DALI_INTEGRATION_PROFILING_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <cstddef>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Integration
{

enum ProfilingType
{
  PROFILING_TYPE_PAN_GESTURE,

  PROFILING_TYPE_END
};

/**
 * Called by adaptor to provide profiling information.
 *
 * @pre Should be called after Core creation.
 * @param type The type of profiling information to output.
 */
DALI_CORE_API void EnableProfiling( ProfilingType type );


namespace Profiling
{

DALI_CORE_API extern const std::size_t ANIMATION_MEMORY_SIZE;    ///< Total size of animation and associated internal objects
DALI_CORE_API extern const std::size_t CONSTRAINT_MEMORY_SIZE;   ///< Total size of constraint and associated internal objects
DALI_CORE_API extern const std::size_t ACTOR_MEMORY_SIZE;        ///< Total size of actor and associated internal objects
DALI_CORE_API extern const std::size_t CAMERA_ACTOR_MEMORY_SIZE; ///< Total size of camera actor and associated internal objects
DALI_CORE_API extern const std::size_t IMAGE_ACTOR_MEMORY_SIZE;  ///< Total size of image actor and associated internal objects
DALI_CORE_API extern const std::size_t LAYER_MEMORY_SIZE;        ///< Total size of layer and associated internal objects
DALI_CORE_API extern const std::size_t IMAGE_MEMORY_SIZE;        ///< Total size of image and associated internal objects

DALI_CORE_API extern const std::size_t RENDERER_MEMORY_SIZE;     ///< Total size of renderer and associated internal objects
DALI_CORE_API extern const std::size_t GEOMETRY_MEMORY_SIZE;     ///< Total size of geometry and associated internal objects
DALI_CORE_API extern const std::size_t PROPERTY_BUFFER_MEMORY_SIZE;     ///< Total size of property-0buffer and associated internal objects
DALI_CORE_API extern const std::size_t TEXTURE_SET_MEMORY_SIZE;     ///< Total size of TextureSet and associated internal objects
DALI_CORE_API extern const std::size_t SAMPLER_MEMORY_SIZE;     ///< Total size of Sampler and associated internal objects
DALI_CORE_API extern const std::size_t SHADER_MEMORY_SIZE;     ///< Total size of shader and associated internal objects
} // namespace Profiling

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_PROFILING_H__
