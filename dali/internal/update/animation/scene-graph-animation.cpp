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

// CLASS HEADER
#include <dali/internal/update/animation/scene-graph-animation.h>

// EXTERNAL INCLUDES
#include <cmath> // fmod

// INTERNAL INCLUDES
#include <dali/internal/render/common/performance-monitor.h>

using namespace std;

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

float DefaultAlphaFunc(float progress)
{
  return progress; // linear
}

Animation::Animation(float durationSeconds, float speedFactor, bool isLooping, Dali::Animation::EndAction endAction, Dali::Animation::EndAction destroyAction)
: mDurationSeconds(durationSeconds),
  mSpeedFactor( speedFactor ),
  mLooping(isLooping),
  mEndAction(endAction),
  mDestroyAction(destroyAction),
  mState(Stopped),
  mElapsedSeconds(0.0f),
  mPlayCount(0)
{
}

Animation::~Animation()
{
}

void Animation::SetDuration(float durationSeconds)
{
  DALI_ASSERT_DEBUG(durationSeconds > 0.0f);

  mDurationSeconds = durationSeconds;
}

void Animation::SetLooping(bool looping)
{
  mLooping = looping;
}

void Animation::SetEndAction(Dali::Animation::EndAction action)
{
  mEndAction = action;
}

void Animation::SetDestroyAction(Dali::Animation::EndAction action)
{
  mDestroyAction = action;
}

void Animation::Play()
{
  mState = Playing;

  if ( mSpeedFactor < 0.0f && mElapsedSeconds <= 0.0f )
  {
    mElapsedSeconds = mDurationSeconds;
  }
}

void Animation::PlayFrom( float progress )
{
  //If the animation is already playing this has no effect
  if( mState != Playing )
  {
    mElapsedSeconds = progress * mDurationSeconds;
    mState = Playing;
  }
}

void Animation::Pause()
{
  if (mState == Playing)
  {
    mState = Paused;
  }
}

void Animation::Bake(BufferIndex bufferIndex, EndAction action)
{
  if( action == Dali::Animation::BakeFinal )
  {
    if( mSpeedFactor > 0.0f )
    {
      mElapsedSeconds = mDurationSeconds + Math::MACHINE_EPSILON_1; // Force animation to reach it's end
    }
    else
    {
      mElapsedSeconds = 0.0f - Math::MACHINE_EPSILON_1; //Force animation to reach it's beginning
    }
  }

  UpdateAnimators(bufferIndex, true/*bake the final result*/);
}

bool Animation::Stop(BufferIndex bufferIndex)
{
  bool animationFinished(false);

  if (mState == Playing || mState == Paused)
  {
    animationFinished = true; // The actor-thread should be notified of this

    if( mEndAction != Dali::Animation::Discard )
    {
      Bake( bufferIndex, mEndAction );
    }

    // The animation has now been played to completion
    ++mPlayCount;
  }

  mElapsedSeconds = 0.0f;
  mState = Stopped;

  return animationFinished;
}

void Animation::OnDestroy(BufferIndex bufferIndex)
{
  if (mState == Playing || mState == Paused)
  {
    if (mDestroyAction != Dali::Animation::Discard)
    {
      Bake( bufferIndex, mDestroyAction );
    }
  }

  mState = Destroyed;
}

void Animation::AddAnimator( AnimatorBase* animator, PropertyOwner* propertyOwner )
{
  animator->Attach( propertyOwner );

  mAnimators.PushBack( animator );
}

bool Animation::Update(BufferIndex bufferIndex, float elapsedSeconds)
{
  if (mState == Stopped || mState == Destroyed)
  {
    // Short circuit when animation isn't running
    return false;
  }

  // The animation must still be applied when Paused/Stopping
  if (mState == Playing)
  {
    mElapsedSeconds += elapsedSeconds * mSpeedFactor;
  }

  if (mLooping)
  {
    if (mElapsedSeconds > mDurationSeconds )
    {
      mElapsedSeconds = fmod(mElapsedSeconds, mDurationSeconds);
    }
    else if( mElapsedSeconds < 0.0f )
    {
      mElapsedSeconds = mDurationSeconds - fmod(mElapsedSeconds, mDurationSeconds);
    }
  }


  const bool animationFinished(mState == Playing                                              &&
                              (( mSpeedFactor > 0.0f && mElapsedSeconds > mDurationSeconds )  ||
                               ( mSpeedFactor < 0.0f && mElapsedSeconds < 0.0f ))
                              );

  UpdateAnimators(bufferIndex, animationFinished && (mEndAction != Dali::Animation::Discard));

  if (animationFinished)
  {
    // The animation has now been played to completion
    ++mPlayCount;

    mElapsedSeconds = 0.0f;
    mState = Stopped;
  }

  return animationFinished;
}

void Animation::UpdateAnimators(BufferIndex bufferIndex, bool bake)
{
  for ( AnimatorIter iter = mAnimators.Begin(); iter != mAnimators.End(); )
  {
    // If an animator is not successfully applied, then it has been orphaned
    bool applied(true);

    AnimatorBase *animator = *iter;
    const float initialDelay(animator->GetInitialDelay());

    if (mElapsedSeconds >= initialDelay || mSpeedFactor < 0.0f )
    {
      // Calculate a progress specific to each individual animator
      float progress(1.0f);
      const float animatorDuration = animator->GetDuration();
      if (animatorDuration > 0.0f) // animators can be "immediate"
      {
        progress = Clamp((mElapsedSeconds - initialDelay) / animatorDuration, 0.0f , 1.0f );
      }

      applied = animator->Update(bufferIndex, progress, bake);
    }

    // Animators are automatically removed, when orphaned from animatable scene objects.
    if (!applied)
    {
      iter = mAnimators.Erase(iter);
    }
    else
    {
      ++iter;

      INCREASE_COUNTER(PerformanceMonitor::ANIMATORS_APPLIED);
    }
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
