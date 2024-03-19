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

// CLASS HEADER
#include <dali/internal/update/animation/scene-graph-animation.h>

// EXTERNAL INCLUDES
#include <cmath> // fmod

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/public-api/math/math-utils.h>

namespace // Unnamed namespace
{
// Memory pool used to allocate new animations. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Animation>& GetAnimationMemoryPool()
{
  static Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Animation> gAnimationMemoryPool;
  return gAnimationMemoryPool;
}

inline void WrapInPlayRange(float& elapsed, const float& playRangeStartSeconds, const float& playRangeEndSeconds)
{
  if(elapsed > playRangeEndSeconds)
  {
    elapsed = playRangeStartSeconds + fmodf((elapsed - playRangeStartSeconds), (playRangeEndSeconds - playRangeStartSeconds));
  }
  else if(elapsed < playRangeStartSeconds)
  {
    elapsed = playRangeEndSeconds - fmodf((playRangeStartSeconds - elapsed), (playRangeEndSeconds - playRangeStartSeconds));
  }
}

/// Compares the end times of the animators and if the end time is less, then it is moved earlier in the list. If end times are the same, then no change.
bool CompareAnimatorEndTimes(const Dali::Internal::SceneGraph::AnimatorBase* lhs, const Dali::Internal::SceneGraph::AnimatorBase* rhs)
{
  return ((lhs->GetIntervalDelay() + lhs->GetDuration()) < (rhs->GetIntervalDelay() + rhs->GetDuration()));
}

} // unnamed namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
Animation* Animation::New(float durationSeconds, float speedFactor, const Vector2& playRange, int32_t loopCount, EndAction endAction, EndAction disconnectAction)
{
  return new(GetAnimationMemoryPool().AllocateRawThreadSafe()) Animation(durationSeconds, speedFactor, playRange, loopCount, endAction, disconnectAction);
}

Animation::Animation(float durationSeconds, float speedFactor, const Vector2& playRange, int32_t loopCount, Dali::Animation::EndAction endAction, Dali::Animation::EndAction disconnectAction)
: mPlayRange(playRange),
  mDurationSeconds(durationSeconds),
  mDelaySeconds(0.0f),
  mElapsedSeconds(playRange.x * mDurationSeconds),
  mSpeedFactor(speedFactor),
  mProgressMarker(0.0f),
  mBlendPoint(0.0f),
  mPlayedCount(0),
  mLoopCount(loopCount),
  mCurrentLoop(0),
  mEndAction(endAction),
  mDisconnectAction(disconnectAction),
  mState(Stopped),
  mProgressReachedSignalRequired(false),
  mAutoReverseEnabled(false),
  mAnimatorSortRequired(false),
  mIsActive{false, false},
  mIsFirstLoop{true},
  mIsStopped{false}
{
}

Animation::~Animation() = default;

void Animation::operator delete(void* ptr)
{
  GetAnimationMemoryPool().FreeThreadSafe(static_cast<Animation*>(ptr));
}

void Animation::SetDuration(float durationSeconds)
{
  mDurationSeconds = durationSeconds;
}

void Animation::SetProgressNotification(float progress)
{
  mProgressMarker = progress;
  if(mProgressMarker > 0.0f)
  {
    mProgressReachedSignalRequired = true;
  }
}

void Animation::SetLoopCount(int32_t loopCount)
{
  mLoopCount   = loopCount;
  mCurrentLoop = 0;
}

void Animation::SetEndAction(Dali::Animation::EndAction action)
{
  mEndAction = action;
}

void Animation::SetDisconnectAction(Dali::Animation::EndAction action)
{
  if(mDisconnectAction != action)
  {
    mDisconnectAction = action;

    for(auto&& item : mAnimators)
    {
      item->SetDisconnectAction(action);
    }
  }
}

void Animation::SetPlayRange(const Vector2& range)
{
  mPlayRange = range;

  // Make sure mElapsedSeconds is within the new range

  if(mState == Stopped)
  {
    // Ensure that the animation starts at the right place
    mElapsedSeconds = mPlayRange.x * mDurationSeconds;
  }
  else
  {
    // If already past the end of the range, but before end of duration, then clamp will
    // ensure that the animation stops on the next update.
    // If not yet at the start of the range, clamping will jump to the start
    mElapsedSeconds = Dali::Clamp(mElapsedSeconds, mPlayRange.x * mDurationSeconds, mPlayRange.y * mDurationSeconds);
  }
}

void Animation::SetBlendPoint(float blendPoint)
{
  mBlendPoint = blendPoint;
}

void Animation::Play()
{
  if(mAnimatorSortRequired)
  {
    // Sort according to end time with earlier end times coming first, if the end time is the same, then the animators are not moved
    std::stable_sort(mAnimators.Begin(), mAnimators.End(), CompareAnimatorEndTimes);
    mAnimatorSortRequired = false;
  }

  // Let we don't change current loop value if the state was paused.
  if(mState != Paused)
  {
    mCurrentLoop  = 0;
    mDelaySeconds = 0.0f;
  }
  mState = Playing;

  if(mSpeedFactor < 0.0f && mElapsedSeconds <= mPlayRange.x * mDurationSeconds)
  {
    mElapsedSeconds = mPlayRange.y * mDurationSeconds;
  }

  SetAnimatorsActive(true);
}

void Animation::PlayFrom(float progress)
{
  // If the animation is already playing this has no effect
  // Progress is guaranteed to be in range.
  if(mState != Playing)
  {
    mElapsedSeconds = progress * mDurationSeconds;
    // Let we don't change current loop value if the state was paused.
    if(mState != Paused)
    {
      mCurrentLoop  = 0;
      mDelaySeconds = 0.0f;
    }
    mState = Playing;

    SetAnimatorsActive(true);
  }
}

void Animation::PlayAfter(float delaySeconds)
{
  if(mState != Playing)
  {
    mDelaySeconds = delaySeconds;
    // Let we don't change current loop value if the state was paused.
    if(mState != Paused)
    {
      mCurrentLoop = 0;
    }
    mState = Playing;

    if(mSpeedFactor < 0.0f && mElapsedSeconds <= mPlayRange.x * mDurationSeconds)
    {
      mElapsedSeconds = mPlayRange.y * mDurationSeconds;
    }

    SetAnimatorsActive(true);
  }
}

void Animation::Pause()
{
  if(mState == Playing)
  {
    mState = Paused;
    DALI_LOG_DEBUG_INFO("Animation[%u] with duration %f ms Paused\n", GetNotifyId(), mDurationSeconds * 1000.0f);
  }
}

void Animation::Bake(BufferIndex bufferIndex, EndAction action)
{
  if(action == Dali::Animation::BAKE_FINAL)
  {
    if(mSpeedFactor > 0.0f)
    {
      mElapsedSeconds = mPlayRange.y * mDurationSeconds + Math::MACHINE_EPSILON_1; // Force animation to reach it's end
    }
    else
    {
      mElapsedSeconds = mPlayRange.x * mDurationSeconds - Math::MACHINE_EPSILON_1; // Force animation to reach it's beginning
    }
  }

  UpdateAnimators(bufferIndex, true /*bake the final result*/, true /*animation finished*/);
}

void Animation::SetAnimatorsActive(bool active)
{
  DALI_LOG_DEBUG_INFO("Animation[%u] with duration %f ms %s\n", GetNotifyId(), mDurationSeconds * 1000.0f, active ? "Play" : "Stop");
  for(auto&& item : mAnimators)
  {
    item->SetActive(active);
  }
}

bool Animation::Stop(BufferIndex bufferIndex)
{
  bool animationFinished(false);

  if(mState == Playing || mState == Paused)
  {
    animationFinished = true; // The actor-thread should be notified of this
    mIsStopped        = true;

    if(mEndAction != Dali::Animation::DISCARD)
    {
      Bake(bufferIndex, mEndAction);

      // Animators are automatically set to inactive in Bake
    }
    else
    {
      SetAnimatorsActive(false);
    }

    // The animation has now been played to completion
    ++mPlayedCount;
    mCurrentLoop = 0;
  }

  mDelaySeconds   = 0.0f;
  mElapsedSeconds = mPlayRange.x * mDurationSeconds;
  mState          = Stopped;
  mIsFirstLoop    = true;

  return animationFinished;
}

void Animation::ClearAnimator(BufferIndex bufferIndex)
{
  // Stop animation immediatly.
  Stop(bufferIndex);

  // Remove all animator.
  mAnimators.Clear();
  mAnimatorSortRequired = false;

  // Reset animation state values.
  mIsStopped   = false; ///< Do not make notify.
  mPlayedCount = 0;
  mCurrentLoop = 0;
}

void Animation::OnDestroy(BufferIndex bufferIndex)
{
  if(mState == Playing || mState == Paused)
  {
    if(mEndAction != Dali::Animation::DISCARD)
    {
      Bake(bufferIndex, mEndAction);

      // Animators are automatically set to inactive in Bake
    }
    else
    {
      SetAnimatorsActive(false);
    }
  }

  mIsStopped = false; ///< Do not make notify.
  mState     = Destroyed;
  DALI_LOG_DEBUG_INFO("Animation[%u] with duration %f ms Destroyed\n", GetNotifyId(), mDurationSeconds * 1000.0f);
}

void Animation::SetLoopingMode(bool loopingMode)
{
  mAutoReverseEnabled = loopingMode;

  for(auto&& item : mAnimators)
  {
    // Send some variables together to figure out the Animation status
    item->SetSpeedFactor(mSpeedFactor);
    item->SetLoopCount(mLoopCount);
    item->SetLoopingMode(loopingMode);
  }
}

void Animation::AddAnimator(OwnerPointer<AnimatorBase>& animator)
{
  animator->ConnectToSceneGraph();
  animator->SetDisconnectAction(mDisconnectAction);

  // Check whether we need to sort mAnimators or not.
  // Sort will be required only if new item is smaller than last value of container.
  if(!mAnimatorSortRequired && !mAnimators.Empty())
  {
    if(CompareAnimatorEndTimes(animator.Get(), *(mAnimators.End() - 1u)))
    {
      mAnimatorSortRequired = true;
    }
  }

  mAnimators.PushBack(animator.Release());
}

void Animation::Update(BufferIndex bufferIndex, float elapsedSeconds, bool& stopped, bool& finished, bool& progressReached)
{
  // Reset mIsStopped flag now.
  stopped    = mIsStopped;
  mIsStopped = false;

  finished = false;

  // Short circuit when animation isn't running
  if(mState == Stopped || mState == Destroyed)
  {
    return;
  }

  // The animation must still be applied when Paused/Stopping
  if(mState == Playing)
  {
    // Sign value of speed factor. It can optimize many arithmetic comparision
    float signSpeedFactor = (mSpeedFactor < 0.0f) ? -1.f : 1.f;

    // If there is delay time before Animation starts, wait the Animation until mDelaySeconds.
    if(mDelaySeconds > 0.0f)
    {
      float reduceSeconds = fabsf(elapsedSeconds * mSpeedFactor);
      if(reduceSeconds > mDelaySeconds)
      {
        // add overflowed time to mElapsedSecond.
        // If speed factor > 0, add it. if speed factor < 0, subtract it.
        float overflowSeconds = reduceSeconds - mDelaySeconds;
        mElapsedSeconds += signSpeedFactor * overflowSeconds;
        mDelaySeconds = 0.0f;
      }
      else
      {
        mDelaySeconds -= reduceSeconds;
      }
    }
    else
    {
      mElapsedSeconds += (elapsedSeconds * mSpeedFactor);
    }

    const float playRangeStartSeconds = mPlayRange.x * mDurationSeconds;
    const float playRangeEndSeconds   = mPlayRange.y * mDurationSeconds;
    // Final reached seconds. It can optimize many arithmetic comparision
    float edgeRangeSeconds = (mSpeedFactor < 0.0f) ? playRangeStartSeconds : playRangeEndSeconds;

    // Optimized Factors.
    // elapsed >  edge   --> check if looped
    // elapsed >= marker --> check if elapsed reached to marker in normal case
    // edge    >= marker --> check if elapsed reached to marker in looped case
    float elapsedFactor = signSpeedFactor * mElapsedSeconds;
    float edgeFactor    = signSpeedFactor * edgeRangeSeconds;
    float markerFactor  = signSpeedFactor * mProgressMarker;

    // check it is looped
    const bool looped = (elapsedFactor > edgeFactor);

    if(looped)
    {
      WrapInPlayRange(mElapsedSeconds, playRangeStartSeconds, playRangeEndSeconds);

      // Recalculate elapsedFactor here
      elapsedFactor = signSpeedFactor * mElapsedSeconds;

      mIsFirstLoop = false;
      if(mLoopCount != 0)
      {
        // Check If this animation is finished
        ++mCurrentLoop;
        if(mCurrentLoop >= mLoopCount)
        {
          DALI_ASSERT_DEBUG(mCurrentLoop == mLoopCount);
          finished = true;

          // The animation has now been played to completion
          ++mPlayedCount;

          // Make elapsed second as edge of range forcely.
          mElapsedSeconds = edgeRangeSeconds + signSpeedFactor * Math::MACHINE_EPSILON_10;
          UpdateAnimators(bufferIndex, finished && (mEndAction != Dali::Animation::DISCARD), finished);

          // After update animation, mElapsedSeconds must be begin of value
          mElapsedSeconds = playRangeStartSeconds + playRangeEndSeconds - edgeRangeSeconds;
          mState          = Stopped;
          mIsFirstLoop    = true;
        }
      }

      // when it is on looped state, 2 case to send progress signal.
      // (require && range_value >= marker) ||         << Signal at previous loop
      // (marker > 0 && !finished && elaped >= marker) << Signal at current loop
      if((mProgressMarker > 0.0f) && !finished && (elapsedFactor >= markerFactor))
      {
        // The application should be notified by NotificationManager, in another thread
        progressReached                = true;
        mProgressReachedSignalRequired = false;
      }
      else
      {
        if(mProgressReachedSignalRequired && (edgeFactor >= markerFactor))
        {
          progressReached = true;
        }
        mProgressReachedSignalRequired = mProgressMarker > 0.0f;
      }
    }
    else
    {
      // when it is not on looped state, only 1 case to send progress signal.
      // (require && elaped >= marker)
      if(mProgressReachedSignalRequired && (elapsedFactor >= markerFactor))
      {
        // The application should be notified by NotificationManager, in another thread
        progressReached                = true;
        mProgressReachedSignalRequired = false;
      }
    }
  }

  // Already updated when finished. So skip.
  if(!finished)
  {
    UpdateAnimators(bufferIndex, false, false);
  }
}

void Animation::UpdateAnimators(BufferIndex bufferIndex, bool bake, bool animationFinished)
{
  mIsActive[bufferIndex] = false;

  const Vector2 playRange(mPlayRange * mDurationSeconds);
  float         elapsedSecondsClamped = Clamp(mElapsedSeconds, playRange.x, playRange.y);

  bool cleanup = false;

  // Loop through all animators
  for(auto& animator : mAnimators)
  {
    if(animator->Orphan())
    {
      cleanup = true;
      continue;
    }

    bool applied(true);
    if(animator->IsEnabled())
    {
      const float intervalDelay(animator->GetIntervalDelay());

      if(elapsedSecondsClamped >= intervalDelay)
      {
        // Calculate a progress specific to each individual animator
        float       progress(1.0f);
        const float animatorDuration = animator->GetDuration();
        if(animatorDuration > 0.0f) // animators can be "immediate"
        {
          progress = Clamp((elapsedSecondsClamped - intervalDelay) / animatorDuration, 0.0f, 1.0f);
        }
        animator->Update(bufferIndex, progress, mIsFirstLoop ? mBlendPoint : 0.0f, bake);

        if(animatorDuration > 0.0f && (elapsedSecondsClamped - intervalDelay) <= animatorDuration)
        {
          mIsActive[bufferIndex] = true;
        }
      }
      else
      {
        animator->SetDelayed(true);
      }
      applied = true;
    }
    else
    {
      applied = false;
    }

    if(animationFinished)
    {
      animator->SetActive(false);
    }

    if(applied)
    {
      INCREASE_COUNTER(PerformanceMonitor::ANIMATORS_APPLIED);
    }
  }

  if(cleanup)
  {
    // Remove animators whose PropertyOwner has been destroyed
    mAnimators.EraseIf([](auto& animator) { return animator->Orphan(); });

    // Need to be re-sort if remained animators size is bigger than one.
    // Note that if animator contains only zero or one items, It is already sorted case.
    mAnimatorSortRequired = (mAnimators.Count() >= 2);
  }
}

uint32_t Animation::GetMemoryPoolCapacity()
{
  return GetAnimationMemoryPool().GetCapacity();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
