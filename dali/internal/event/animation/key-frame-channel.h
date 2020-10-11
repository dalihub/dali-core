#ifndef DALI_INTERNAL_KEY_FRAME_CHANNEL_H
#define DALI_INTERNAL_KEY_FRAME_CHANNEL_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/internal/event/animation/progress-value.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{
namespace Internal
{
template<typename V>
struct KeyFrameChannel
{
  using ProgressValues = std::vector<ProgressValue<V>>;

  bool IsActive(float progress) const
  {
    if(!mValues.empty() && (progress >= mValues[0].GetProgress()))
    {
      return true;
    }
    return false;
  }

  V GetValue(float progress, Dali::Animation::Interpolation interpolation) const
  {
    V interpolatedV{};

    if(progress >= mValues.back().GetProgress())
    {
      interpolatedV = mValues.back().GetValue();
    }
    else
    {
      auto end   = std::find_if(mValues.begin(), mValues.end(), [=](const auto& element) { return element.GetProgress() > progress; });
      auto start = end - 1;

      const bool validInterval = (end != mValues.end()) && (start->GetProgress() <= progress);

      if(validInterval)
      {
        float frameProgress = (progress - start->GetProgress()) / (end->GetProgress() - start->GetProgress());
        if(interpolation == Dali::Animation::LINEAR)
        {
          Interpolate(interpolatedV, start->GetValue(), end->GetValue(), frameProgress);
        }
        else
        {
          //Calculate prev and next values
          V prev;
          if(start != mValues.begin())
          {
            prev = (start - 1)->GetValue();
          }
          else
          {
            //Project next value through start point
            prev = start->GetValue() + (start->GetValue() - (start + 1)->GetValue());
          }

          V next;
          if(end != mValues.end() - 1)
          {
            next = (end + 1)->GetValue();
          }
          else
          {
            //Project prev value through end point
            next = end->GetValue() + (end->GetValue() - (end - 1)->GetValue());
          }

          CubicInterpolate(interpolatedV, prev, start->GetValue(), end->GetValue(), next, frameProgress);
        }
      }
    }
    return interpolatedV;
  }

  ProgressValues mValues;
};

} // Internal
} // namespace Dali

#endif // DALI_INTERNAL_KEY_FRAME_CHANNEL_H
