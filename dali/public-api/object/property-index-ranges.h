#ifndef __DALI_PROPERTY_INDEX_RANGES_H__
#define __DALI_PROPERTY_INDEX_RANGES_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

/**
 * @brief Enumeration for the property index ranges.
 *
 * Enumerations are being used here rather than static constants so that switch statements can be
 * used to compare property indices.
 * @SINCE_1_0.0
 */
enum PropertyRanges
{
  DEFAULT_OBJECT_PROPERTY_START_INDEX           = 0,          ///< For all objects deriving from Handle (including Actors). @SINCE_1_0.0

  DEFAULT_ACTOR_PROPERTY_START_INDEX            = DEFAULT_OBJECT_PROPERTY_START_INDEX, ///< Start index for Actor. @SINCE_1_0.0
  DEFAULT_ACTOR_PROPERTY_MAX_COUNT              = 10000,      ///< Actor range: 0 to 9999 @SINCE_1_0.0

  DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX    = DEFAULT_ACTOR_PROPERTY_START_INDEX + DEFAULT_ACTOR_PROPERTY_MAX_COUNT, ///< Property start index for classes deriving directly from Actor. @SINCE_1_0.0

  DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION     = 10000,      ///< Second-level and onwards derived objects should increment their start index by this. @SINCE_1_0.0

  DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX,  ///< Used by PanGestureDetector. @SINCE_1_0.0

  DEFAULT_RENDERER_PROPERTY_START_INDEX         = 9000000,    ///< Start index for Renderer. @SINCE_1_1.44
  DEFAULT_RENDERER_PROPERTY_MAX_INDEX           = DEFAULT_RENDERER_PROPERTY_START_INDEX + 100000, ///< Renderer range: 9000000 to 9100000 @SINCE_1_1.44

  PROPERTY_REGISTRATION_START_INDEX             = 10000000,   ///< The index when registering a property should start from this number. @SINCE_1_0.0
  DEFAULT_PROPERTY_MAX_COUNT                    = PROPERTY_REGISTRATION_START_INDEX,           ///< Default Property Range: 0 to 9999999 @SINCE_1_0.0

  PROPERTY_REGISTRATION_MAX_INDEX               = 19999999,   ///< The maximum index supported when registering a property @SINCE_1_0.0

  ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX  = 20000000,   ///< The index when registering an animatable property should start from this number. (SceneGraph properties per type) @SINCE_1_0.0
  ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX    = 29999999,   ///< The maximum index supported when registering an animatable property @SINCE_1_0.0

  CHILD_PROPERTY_REGISTRATION_START_INDEX       = 45000000,   ///< The index when registering a child property should start from this number. (Event side properties per instance) @SINCE_1_1.35
  CHILD_PROPERTY_REGISTRATION_MAX_INDEX         = 49999999,   ///< The maximum index supported when registering a child property @SINCE_1_1.35

  PROPERTY_CUSTOM_START_INDEX                   = 50000000,   ///< The index at which custom properties start (SceneGraph and Event side properties per instance) @SINCE_1_0.0
  PROPERTY_CUSTOM_MAX_INDEX                     = 59999999,   ///< The maximum index supported for custom properties @SINCE_1_1.45

  CORE_PROPERTY_MAX_INDEX                       = PROPERTY_CUSTOM_MAX_INDEX, ///< The maximum index that Core properties can go up to @SINCE_1_1.45
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_PROPERTY_INDEX_RANGES_H__
