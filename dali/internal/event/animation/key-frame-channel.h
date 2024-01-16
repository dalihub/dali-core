#ifndef DALI_INTERNAL_KEY_FRAME_CHANNEL_H
#define DALI_INTERNAL_KEY_FRAME_CHANNEL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
      // Find lowest element s.t. progress is greater than progress.
      // So start->GetProgress() <= progress < end->GetProgress() is satisfied.
      auto end   = std::lower_bound(mValues.begin(), mValues.end(), progress, [](const auto& element, const float& progress) { return element.GetProgress() <= progress; });
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

  bool OptimizeValuesLinear()
  {
    ProgressValues optimizedValues;
    bool           optimized = false;

    // Optimize works only if value has more or equal than 3 values.
    if(mValues.size() < 3u)
    {
      return optimized;
    }

    auto iter = mValues.begin();
    for(; iter + 1 != mValues.end();)
    {
      // Insert iter, which is the first value, or jter what we fail to ignore previous loops.
      optimizedValues.push_back(*iter);
      const float iterProgress = iter->GetProgress();

      auto jter = iter + 1;

      for(; jter + 1 != mValues.end();)
      {
        // Check whether we can ignore jter now.
        const auto  kter         = jter + 1;
        const float jterProgress = jter->GetProgress();
        const float kterProgress = kter->GetProgress();

        float frameProgress = (jterProgress - iterProgress) / (kterProgress - iterProgress);

        // Interpolate with iter and kter.
        // Check value between interpolatedV and jter->GetValue().
        // If two values are similar, we can skip jter!
        V interpolatedV{};
        Interpolate(interpolatedV, iter->GetValue(), kter->GetValue(), frameProgress);

        // TODO : We might need to find more good way to compare two values
        if(Property::Value(interpolatedV) != Property::Value(jter->GetValue()))
        {
          break;
        }

        optimized = true;

        // Keep checking for the next jter
        ++jter;
      }
      iter = jter;
    }

    // Insert last value.
    optimizedValues.push_back(*iter);

    if(optimized)
    {
      mValues = std::move(optimizedValues);
    }
    return optimized;
  }

  ProgressValues mValues;
};

} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_KEY_FRAME_CHANNEL_H
