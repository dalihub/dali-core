#ifndef __DALI_INTEGRATION_TOUCH_DATA_H__
#define __DALI_INTEGRATION_TOUCH_DATA_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace Integration
{

struct TouchData;

typedef std::vector<TouchData> TouchDataContainer;
typedef TouchDataContainer::iterator TouchDataIter;

/**
 * TouchData structure
 * represents the raw touch information from touch-screen for
 * a single touch event e.g.
 *
 * "First finger touching down at pixel 123,456 on the screen (relative
 * to top left corner of phone in portrait mode). at timestamp 125ms (from a reference
 * timestamp e.g. phone boot being 0secs)."
 * TouchData(Down, 125, 0, 123, 456)
 *
 * "Above finger moving to pixel 133,457, at timestamp 150ms"
 * TouchData(Motion, 150, 0, 133, 457)
 *
 * "Additional finger touching down at pixel 50, 75, at timestamp 175ms"
 * TouchData(Down, 175, 1, 50, 75)
 *
 * "First finger removing at pixel 143, 458, at timestamp 200ms"
 * TouchData(Up, 200, 0, 143, 458)
 *
 * "Additional finger removing at pixel 51, 77, at timestamp 225ms"
 * TouchData(Up, 225, 1, 51, 77)
 *
 * Note: Multiple incidents of touch data can be present at the same timestamp.
 */
struct TouchData
{

  /**
   * The Touch Type for this data.
   */
  enum TouchType
  {
    Down,     // Touch started
    Up,       // Touch ended
    Motion    // Touch is continuing
  };

  TouchData()
  : type(Motion),
    timestamp(0u),
    index(0u),
    x(0),
    y(0)
  {

  }

  TouchData(TouchType type,
            unsigned int timestamp,
            unsigned int index,
            int x,
            int y)
  : type(type),
    timestamp(timestamp),
    index(index),
    x(x),
    y(y)
  {
  }

  TouchType type;
  unsigned int timestamp;
  unsigned int index;
  int x;
  int y;

};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_TOUCH_DATA_H__
