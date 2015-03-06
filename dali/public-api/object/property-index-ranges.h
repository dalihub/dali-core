#ifndef __DALI_PROPERTY_INDEX_RANGES_H__
#define __DALI_PROPERTY_INDEX_RANGES_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

/**
 * @brief These are the property index ranges.
 *
 * Enumerations are being used here rather than static constants so that switch statements can be
 * used to compare property indices.
 */
enum PropertyRanges
{
  DEFAULT_ACTOR_PROPERTY_START_INDEX            = 0,          ///< Start index for Actor (and other base classes).
  DEFAULT_ACTOR_PROPERTY_MAX_COUNT              = 10000,      ///< Actor range: 0 to 9999

  DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX   = DEFAULT_ACTOR_PROPERTY_START_INDEX,         ///< For non-Actor objects.

  DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX    = DEFAULT_ACTOR_PROPERTY_START_INDEX + DEFAULT_ACTOR_PROPERTY_MAX_COUNT, ///< Property start index for classes deriving directly from Actor.

  DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION     = 10000,      ///< Second-level and onwards derived objects should increment their start index by this.

  DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX, ///< Used by PanGestureDetector.

  PROPERTY_REGISTRATION_START_INDEX             = 10000000,   ///< The index when registering a property should start from this number.
  DEFAULT_PROPERTY_MAX_COUNT                    = PROPERTY_REGISTRATION_START_INDEX,          ///< Default Property Range: 0 to 9999999

  PROPERTY_REGISTRATION_MAX_INDEX               = 19999999,   ///< The maximum index supported when registering a property
  PROPERTY_CUSTOM_START_INDEX                   = 50000000,   ///< The index at which custom properties start
};

} // namespace Dali

#endif // __DALI_PROPERTY_INDEX_RANGES_H__
