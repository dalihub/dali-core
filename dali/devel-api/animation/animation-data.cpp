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

// CLASS HEADER
#include <dali/devel-api/animation/animation-data.h>

namespace Dali
{


AnimationData::AnimationData()
{
}

AnimationData::~AnimationData()
{
  Clear();
}


void AnimationData::Add( AnimationDataElement* animationDataElement )
{
  mAnimationDataList.PushBack( animationDataElement );
}


Dali::Animation AnimationData::CreateAnimation( Dali::Actor targetActor, float duration )
{
  Dali::Animation animation;

  if( mAnimationDataList.Size() > 0 )
  {
    animation = Dali::Animation::New( duration );

    // Setup an Animation from AnimationData.
    AnimationData::AnimationDataList::Iterator end = mAnimationDataList.End();
    for( AnimationData::AnimationDataList::Iterator iter = mAnimationDataList.Begin(); iter != end; ++iter )
    {
      // Override the actor in the animation.
      animation.AnimateTo( Property( targetActor, ( *iter )->property ), ( *iter )->value,
          ( *iter )->alphaFunction, TimePeriod( ( *iter )->timePeriodDelay, ( *iter )->timePeriodDuration ) );
    }
  }

  return animation;
}


void AnimationData::Clear()
{
  AnimationData::AnimationDataList::Iterator end = mAnimationDataList.End();
  for( AnimationData::AnimationDataList::Iterator iter = mAnimationDataList.Begin(); iter != end; ++iter )
  {
    delete ( *iter );
  }
  mAnimationDataList.Clear();
}


} // namespace Dali
