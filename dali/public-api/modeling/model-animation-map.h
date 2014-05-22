#ifndef __DALI_MODEL_ANIMATION_MAP_H__
#define __DALI_MODEL_ANIMATION_MAP_H__

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


// INTERNAL INCLUDES
#include <dali/public-api/modeling/entity-animator-map.h>

namespace Dali DALI_IMPORT_API
{

struct ModelAnimationMap;
typedef std::vector<ModelAnimationMap> ModelAnimationMapContainer; ///< Container of model animations

/**
 * @brief ModelAnimationMap maps an animation name to a set of entity animators and animation
 * properties such as duration and repeats.
 */
struct ModelAnimationMap
{
  /**
   * @brief Destructor.
   **/
  ModelAnimationMap()
    : duration(0.0f),
      repeats(0)
  {
  }

  std::string                   name;       ///< Name of the animation
  EntityAnimatorMapContainer    animators;  ///< Set of entity - animation mappings
  float                         duration;   ///< Duration of the animation
  int                           repeats;    ///< Number of times the animation should be repeated
};

} // namespace Dali

#endif // __DALI_MODEL_ANIMATION_MAP_H__
