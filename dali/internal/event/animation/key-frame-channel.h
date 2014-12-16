#ifndef __DALI_INTERNAL_KEY_FRAME_CHANNEL_H__
#define __DALI_INTERNAL_KEY_FRAME_CHANNEL_H__

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
#include <dali/internal/event/animation/progress-value.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{
namespace Internal
{

class KeyFrameChannelBase
{
public:
  enum KeyFrameChannelId
  {
    Translate, Rotate, Scale,
  };

  KeyFrameChannelBase(KeyFrameChannelId channel_id)
  : mChannelId(channel_id)
  {
  }

  virtual ~KeyFrameChannelBase()
  {
  }

  KeyFrameChannelId GetId() const
  {
    return mChannelId;
  }

  virtual bool IsActive(float progress) = 0;

protected:
  KeyFrameChannelId       mChannelId;
};


template <typename V>
class KeyFrameChannel : public KeyFrameChannelBase
{
public:
  typedef std::vector<ProgressValue<V> > ProgressValues;

  KeyFrameChannel(KeyFrameChannelId channel_id, ProgressValues& values )
  : KeyFrameChannelBase(channel_id),
    mValues(values)
  {
  }


  virtual ~KeyFrameChannel()
  {
  }

  bool IsActive (float progress);

  V GetValue(float progress, Dali::Animation::Interpolation interpolation) const;

  bool FindInterval(typename ProgressValues::iterator& start,
                    typename ProgressValues::iterator& end,
                    float progress) const;

  ProgressValues& mValues;
};

template <class V>
bool KeyFrameChannel<V>::IsActive (float progress)
{
  bool active = false;
  if(!mValues.empty())
  {
    ProgressValue<V>& first = mValues.front();

    if( progress >= first.GetProgress() )
    {
      active = true;
    }
  }
  return active;
}

/**
 * Use a linear search to find the interval containing progress
 * TODO: Use binary search instead
 */
template <class V>
bool KeyFrameChannel<V>::FindInterval(
  typename ProgressValues::iterator& start,
  typename ProgressValues::iterator& end,
  float progress) const
{
  bool found = false;
  typename std::vector<ProgressValue<V> >::iterator iter = mValues.begin();
  typename std::vector<ProgressValue<V> >::iterator prevIter = iter;

  while(iter != mValues.end() && iter->GetProgress() <= progress)
  {
    prevIter = iter;
    ++iter;
  }

  if(iter == mValues.end())
  {
    --prevIter;
    --iter;
  }

  if(prevIter->GetProgress() <= progress
     &&
     iter->GetProgress() > progress)
  {
    found = true;
    start = prevIter;
    end   = iter;
  }

  return found;
}

template <class V>
V KeyFrameChannel<V>::GetValue (float progress, Dali::Animation::Interpolation interpolation) const
{
  ProgressValue<V>&  firstPV =  mValues.front();

  typename std::vector<ProgressValue<V> >::iterator start;
  typename std::vector<ProgressValue<V> >::iterator end;

  V interpolatedV = firstPV.GetValue();
  if(progress >= mValues.back().GetProgress() )
  {
    interpolatedV = mValues.back().GetValue(); // This should probably be last value...
  }
  else if(FindInterval(start, end, progress))
  {
    float frameProgress = (progress - start->GetProgress()) / (end->GetProgress() - start->GetProgress());

    if( interpolation == Dali::Animation::Linear )
    {
      Interpolate(interpolatedV, start->GetValue(), end->GetValue(), frameProgress);
    }
    else
    {
      //Calculate prev and next values
      V prev;
      if( start != mValues.begin() )
      {
        prev = (start-1)->GetValue();
      }
      else
      {
        //Project next value through start point
        prev = start->GetValue() + (start->GetValue()-(start+1)->GetValue());
      }

      V next;
      if( end != mValues.end()-1)
      {
        next = (end+1)->GetValue();
      }
      else
      {
        //Project prev value through end point
        next = end->GetValue() + (end->GetValue()-(end-1)->GetValue());
      }

      CubicInterpolate(interpolatedV, prev, start->GetValue(), end->GetValue(), next, frameProgress);
    }
  }

  return interpolatedV;
}

typedef KeyFrameChannel<float>      KeyFrameChannelNumber;
typedef KeyFrameChannel<Vector2>    KeyFrameChannelVector2;
typedef KeyFrameChannel<Vector3>    KeyFrameChannelVector3;
typedef KeyFrameChannel<Vector4>    KeyFrameChannelVector4;
typedef KeyFrameChannel<Quaternion> KeyFrameChannelQuaternion;
typedef KeyFrameChannel<AngleAxis>  KeyFrameChannelAngleAxis;


} // Internal
} // namespace Dali

#endif // __DALI_INTERNAL_KEY_FRAME_CHANNEL_H__
