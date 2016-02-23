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
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/render/common/performance-monitor.h>

namespace //Unnamed namespace
{
//Memory pool used to allocate new animations. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Animation> gAnimationMemoryPool;

inline void WrapInPlayRange( float& elapsed, const Dali::Vector2& playRangeSeconds)
{
  if( elapsed > playRangeSeconds.y )
  {
    elapsed = playRangeSeconds.x + fmodf((elapsed-playRangeSeconds.x), (playRangeSeconds.y-playRangeSeconds.x));
  }
  else if( elapsed < playRangeSeconds.x )
  {
    elapsed = playRangeSeconds.y - fmodf( (playRangeSeconds.x - elapsed), (playRangeSeconds.y-playRangeSeconds.x) );
  }
}

}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

Animation* Animation::New( float durationSeconds, float speedFactor, const Vector2& playRange, int loopCount, EndAction endAction, EndAction disconnectAction )
{
  return new ( gAnimationMemoryPool.AllocateRawThreadSafe() ) Animation( durationSeconds, speedFactor, playRange, loopCount, endAction, disconnectAction );
}

Animation::Animation( float durationSeconds, float speedFactor, const Vector2& playRange, int loopCount, Dali::Animation::EndAction endAction, Dali::Animation::EndAction disconnectAction )
: mDurationSeconds(durationSeconds),
  mSpeedFactor( speedFactor ),
  mEndAction(endAction),
  mDisconnectAction(disconnectAction),
  mState(Stopped),
  mElapsedSeconds(playRange.x*mDurationSeconds),
  mPlayedCount(0),
  mLoopCount(loopCount),
  mCurrentLoop(0),
  mPlayRange( playRange )
{
}

Animation::~Animation()
{
}

void Animation::operator delete( void* ptr )
{
  gAnimationMemoryPool.FreeThreadSafe( static_cast<Animation*>( ptr ) );
}

void Animation::SetDuration(float durationSeconds)
{
  mDurationSeconds = durationSeconds;
}

void Animation::SetLoopCount(int loopCount)
{
  mLoopCount = loopCount;
  mCurrentLoop = 0;
}

void Animation::SetEndAction(Dali::Animation::EndAction action)
{
  mEndAction = action;
}

void Animation::SetDisconnectAction(Dali::Animation::EndAction action)
{
  if ( mDisconnectAction != action )
  {
    mDisconnectAction = action;

    for ( AnimatorIter iter = mAnimators.Begin(), endIter = mAnimators.End(); iter != endIter; ++iter )
    {
      (*iter)->SetDisconnectAction( action );
    }
  }
}

void Animation::SetPlayRange( const Vector2& range )
{
  mPlayRange = range;

  // Make sure mElapsedSeconds is within the new range

  if( mState == Stopped )
  {
    // Ensure that the animation starts at the right place
    mElapsedSeconds = mPlayRange.x * mDurationSeconds;
  }
  else
  {
    // If already past the end of the range, but before end of duration, then clamp will
    // ensure that the animation stops on the next update.
    // If not yet at the start of the range, clamping will jump to the start
    mElapsedSeconds = Dali::Clamp(mElapsedSeconds, mPlayRange.x*mDurationSeconds , mPlayRange.y*mDurationSeconds );
  }
}

void Animation::Play()
{
  mState = Playing;

  if ( mSpeedFactor < 0.0f && mElapsedSeconds <= mPlayRange.x*mDurationSeconds )
  {
    mElapsedSeconds = mPlayRange.y * mDurationSeconds;
  }

  SetAnimatorsActive( true );

  mCurrentLoop = 0;
}

void Animation::PlayFrom( float progress )
{
  // If the animation is already playing this has no effect
  // Progress is guaranteed to be in range.
  if( mState != Playing )
  {
    mElapsedSeconds = progress * mDurationSeconds;
    mState = Playing;

    SetAnimatorsActive( true );
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
      mElapsedSeconds = mPlayRange.y*mDurationSeconds + Math::MACHINE_EPSILON_1; // Force animation to reach it's end
    }
    else
    {
      mElapsedSeconds = mPlayRange.x*mDurationSeconds - Math::MACHINE_EPSILON_1; //Force animation to reach it's beginning
    }
  }

  UpdateAnimators( bufferIndex, true/*bake the final result*/, true /*animation finished*/ );
}

void Animation::SetAnimatorsActive( bool active )
{
  for ( AnimatorIter iter = mAnimators.Begin(), endIter = mAnimators.End(); iter != endIter; ++iter )
  {
    (*iter)->SetActive( active );
  }
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

      // Animators are automatically set to inactive in Bake
    }
    else
    {
      SetAnimatorsActive( false );
    }

    // The animation has now been played to completion
    ++mPlayedCount;
    mCurrentLoop = 0;
  }

  mElapsedSeconds = mPlayRange.x*mDurationSeconds;
  mState = Stopped;

  return animationFinished;
}

void Animation::OnDestroy(BufferIndex bufferIndex)
{
  if (mState == Playing || mState == Paused)
  {
    if (mEndAction != Dali::Animation::Discard)
    {
      Bake( bufferIndex, mEndAction );

      // Animators are automatically set to inactive in Bake
    }
    else
    {
      SetAnimatorsActive( false );
    }
  }

  mState = Destroyed;
}

void Animation::AddAnimator( AnimatorBase* animator )
{
  animator->ConnectToSceneGraph();
  animator->SetDisconnectAction( mDisconnectAction );

  mAnimators.PushBack( animator );
}

void Animation::Update(BufferIndex bufferIndex, float elapsedSeconds, bool& looped, bool& finished )
{
  looped = false;
  finished = false;

  if (mState == Stopped || mState == Destroyed)
  {
    // Short circuit when animation isn't running
    return;
  }

  // The animation must still be applied when Paused/Stopping
  if (mState == Playing)
  {
    mElapsedSeconds += elapsedSeconds * mSpeedFactor;
  }

  Vector2 playRangeSeconds = mPlayRange * mDurationSeconds;

  if( 0 == mLoopCount )
  {
    // loop forever
    WrapInPlayRange(mElapsedSeconds, playRangeSeconds);

    UpdateAnimators(bufferIndex, false, false);

    // don't increment mPlayedCount as event loop tracks this to indicate animation finished (end of all loops)
  }
  else if( mCurrentLoop < mLoopCount - 1) // '-1' here so last loop iteration uses play once below
  {
    // looping
    looped =  (mState == Playing                                                 &&
               (( mSpeedFactor > 0.0f && mElapsedSeconds > playRangeSeconds.y )  ||
                ( mSpeedFactor < 0.0f && mElapsedSeconds < playRangeSeconds.x )) );

    WrapInPlayRange( mElapsedSeconds, playRangeSeconds );

    UpdateAnimators(bufferIndex, false, false);

    if(looped)
    {
      ++mCurrentLoop;
      // don't increment mPlayedCount until the finished final loop
    }
  }
  else
  {
    // playing once (and last mCurrentLoop)
    finished = (mState == Playing                                                 &&
                (( mSpeedFactor > 0.0f && mElapsedSeconds > playRangeSeconds.y )  ||
                 ( mSpeedFactor < 0.0f && mElapsedSeconds < playRangeSeconds.x )) );

    // update with bake if finished
    UpdateAnimators(bufferIndex, finished && (mEndAction != Dali::Animation::Discard), finished);

    if(finished)
    {
      // The animation has now been played to completion
      ++mPlayedCount;

      // loop iterations come to this else branch for their final iterations
      if( mCurrentLoop < mLoopCount)
      {
        ++mCurrentLoop;
        DALI_ASSERT_DEBUG(mCurrentLoop == mLoopCount);
      }

      mElapsedSeconds = playRangeSeconds.x;
      mState = Stopped;
    }
  }
}

void Animation::UpdateAnimators( BufferIndex bufferIndex, bool bake, bool animationFinished )
{
  const Vector2 playRange( mPlayRange * mDurationSeconds );
  float elapsedSecondsClamped = Clamp( mElapsedSeconds, playRange.x, playRange.y );

  //Loop through all animators
  bool applied(true);
  for ( AnimatorIter iter = mAnimators.Begin(); iter != mAnimators.End(); )
  {
    AnimatorBase *animator = *iter;

    if( animator->Orphan() )
    {
      //Remove animators whose PropertyOwner has been destroyed
      iter = mAnimators.Erase(iter);
    }
    else
    {
      if( animator->IsEnabled() )
      {
        const float initialDelay( animator->GetInitialDelay() );
        if( elapsedSecondsClamped >= initialDelay )
        {
          // Calculate a progress specific to each individual animator
          float progress(1.0f);
          const float animatorDuration = animator->GetDuration();
          if (animatorDuration > 0.0f) // animators can be "immediate"
          {
            progress = Clamp((elapsedSecondsClamped - initialDelay) / animatorDuration, 0.0f , 1.0f );
          }
          animator->Update(bufferIndex, progress, bake);
        }
        applied = true;
      }
      else
      {
        applied = false;
      }

      if ( animationFinished )
      {
        animator->SetActive( false );
      }

      if (applied)
      {
        INCREASE_COUNTER(PerformanceMonitor::ANIMATORS_APPLIED);
      }

      ++iter;
    }
  }

}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
