#ifndef __DALI_INTEGRATION_PROFILING_H__
#define __DALI_INTEGRATION_PROFILING_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
DALI_IMPORT_API void EnableProfiling( ProfilingType type );


namespace Profiling
{

DALI_IMPORT_API extern const int ANIMATION_MEMORY_SIZE;    ///< Total size of animation and associated internal objects
DALI_IMPORT_API extern const int CONSTRAINT_MEMORY_SIZE;   ///< Total size of constraint and associated internal objects
DALI_IMPORT_API extern const int ACTOR_MEMORY_SIZE;        ///< Total size of actor and associated internal objects
DALI_IMPORT_API extern const int CAMERA_ACTOR_MEMORY_SIZE; ///< Total size of camera actor and associated internal objects
DALI_IMPORT_API extern const int TEXT_ACTOR_MEMORY_SIZE;   ///< Total size of text actor and associated internal objects
DALI_IMPORT_API extern const int IMAGE_ACTOR_MEMORY_SIZE;  ///< Total size of image actor and associated internal objects
DALI_IMPORT_API extern const int LAYER_MEMORY_SIZE;        ///< Total size of layer and associated internal objects
DALI_IMPORT_API extern const int IMAGE_MEMORY_SIZE;        ///< Total size of image and associated internal objects

DALI_IMPORT_API extern const int RENDERER_MEMORY_SIZE;     ///< Total size of renderer and associated internal objects
DALI_IMPORT_API extern const int GEOMETRY_MEMORY_SIZE;     ///< Total size of geometry and associated internal objects
DALI_IMPORT_API extern const int PROPERTY_BUFFER_MEMORY_SIZE;     ///< Total size of property-0buffer and associated internal objects
DALI_IMPORT_API extern const int MATERIAL_MEMORY_SIZE;     ///< Total size of material and associated internal objects
DALI_IMPORT_API extern const int SAMPLER_MEMORY_SIZE;     ///< Total size of material and associated internal objects
DALI_IMPORT_API extern const int SHADER_MEMORY_SIZE;     ///< Total size of material and associated internal objects
} // namespace Profiling

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_PROFILING_H__
