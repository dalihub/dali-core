/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/animation/animation-playlist.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/public-api/common/vector-wrapper.h>

#ifdef TRACE_ENABLED
#include <chrono>
#include <cmath>
#include <thread>
#endif

namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);

#if defined(DEBUG_ENABLED)
Debug::Filter* gAnimFilter = Debug::Filter::New(Debug::NoLogging, false, "DALI_LOG_ANIMATION");
#endif

#ifdef TRACE_ENABLED
uint64_t GetNanoseconds()
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch = std::chrono::steady_clock::now().time_since_epoch();

  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);

  return static_cast<uint64_t>(duration.count());
}
#endif
} // namespace

namespace Dali
{
namespace Internal
{
AnimationPlaylist* AnimationPlaylist::New()
{
  return new AnimationPlaylist();
}

AnimationPlaylist::AnimationPlaylist() = default;

AnimationPlaylist::~AnimationPlaylist() = default;

void AnimationPlaylist::AnimationCreated(Animation& animation)
{
  mAnimations.PushBack(&animation);
}

void AnimationPlaylist::AnimationDestroyed(Animation& animation)
{
  auto iter = mAnimations.Find(&animation);
  DALI_ASSERT_ALWAYS(iter != mAnimations.End() && "Animation not found");

  mAnimations.Erase(iter);
}

void AnimationPlaylist::OnPlay(Animation& animation)
{
  Dali::Animation handle = Dali::Animation(&animation);
  mPlaylist.insert(handle);
}

void AnimationPlaylist::OnClear(Animation& animation, bool ignoreRequired)
{
  Dali::Animation handle = Dali::Animation(&animation);
  auto            iter   = mPlaylist.find(handle);

  // Animation might be removed when NotifyCompleted called.
  if(DALI_LIKELY(iter != mPlaylist.end()))
  {
    mPlaylist.erase(iter);
  }

  DALI_LOG_INFO(gAnimFilter, Debug::Verbose, "OnClear(%d) Animation[%u]\n", ignoreRequired, animation.GetAnimationId());

  if(ignoreRequired)
  {
    mIgnoredAnimations.insert(animation.GetAnimationId());
  }
}

void AnimationPlaylist::EventLoopFinished()
{
  if(mIgnoredAnimations.size() > 0u)
  {
    DALI_LOG_INFO(gAnimFilter, Debug::Verbose, "Ignored animations count[%zu]\n", mIgnoredAnimations.size());
    mIgnoredAnimations.clear();
  }
}

void AnimationPlaylist::NotifyProgressReached(NotifierInterface::NotifyId notifyId)
{
  Dali::Animation handle; // Will own handle until all emits have been done.

  if(DALI_LIKELY(mIgnoredAnimations.find(notifyId) == mIgnoredAnimations.end()))
  {
    auto* animation = GetEventObject(notifyId);
    if(DALI_LIKELY(animation))
    {
      // Check if this animation hold inputed scenegraph animation.
      DALI_ASSERT_DEBUG(animation->GetSceneObject()->GetNotifyId() == notifyId);

      handle = Dali::Animation(animation);
      animation->EmitSignalProgressReached();
    }
  }
}

void AnimationPlaylist::NotifyCompleted(CompleteNotificationInterface::ParameterList notifierIdList)
{
  std::vector<Dali::Animation> finishedAnimations; // Will own handle until all emits have been done.

#ifdef TRACE_ENABLED
  std::vector<std::pair<uint64_t, uint32_t>> animationFinishedTimeChecker;

  uint64_t start = 0u;
  uint64_t end   = 0u;
#endif

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_ANIMATION_FINISHED", [&](std::ostringstream& oss) {
    oss << "[n:" << notifierIdList.Count() << ",i:" << mIgnoredAnimations.size() << "]";
  });

  for(const auto& notifierId : notifierIdList)
  {
    if(DALI_LIKELY(mIgnoredAnimations.find(notifierId) == mIgnoredAnimations.end()))
    {
      auto* animation = GetEventObject(notifierId);
      if(DALI_LIKELY(animation))
      {
        // Check if this animation hold inputed scenegraph animation.
        DALI_ASSERT_DEBUG(animation->GetSceneObject()->GetNotifyId() == notifierId);

        // Update loop count. And check whether animation was finished or not.
        if(animation->HasFinished())
        {
          finishedAnimations.push_back(Dali::Animation(animation));

          // The animation may be present in mPlaylist - remove if necessary
          // Note that the animation "Finish" signal is emitted after Stop() has been called
          OnClear(*animation, false);
        }
        else
        {
          DALI_LOG_INFO(gAnimFilter, Debug::Verbose, "Animation[%u] not finished actually...\n", notifierId);
        }
      }
      else
      {
        DALI_LOG_INFO(gAnimFilter, Debug::Verbose, "Animation[%u] destroyed!!\n", notifierId);
      }
    }
    else
    {
      DALI_LOG_INFO(gAnimFilter, Debug::Verbose, "Animation[%u] Ignored (Clear() called)\n", notifierId);
    }
  }

  // Now it's safe to emit the signals
  for(auto& animation : finishedAnimations)
  {
    // Check whether given animation still available (Since it could be cleared during finished signal emitted).
    if(DALI_LIKELY(mIgnoredAnimations.find(animation.GetAnimationId()) == mIgnoredAnimations.end()))
    {
#ifdef TRACE_ENABLED
      if(gTraceFilter && gTraceFilter->IsTraceEnabled())
      {
        start = GetNanoseconds();
      }
#endif
      GetImplementation(animation).EmitSignalFinish();
#ifdef TRACE_ENABLED
      if(gTraceFilter && gTraceFilter->IsTraceEnabled())
      {
        end = GetNanoseconds();
        animationFinishedTimeChecker.emplace_back(end - start, GetImplementation(animation).GetSceneObject()->GetNotifyId());
      }
#endif
    }
    else
    {
      DALI_LOG_INFO(gAnimFilter, Debug::Verbose, "Animation[%u] Ignored (Clear() called)\n", animation.GetAnimationId());
    }
  }

  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_ANIMATION_FINISHED", [&](std::ostringstream& oss) {
    oss << "[f:" << finishedAnimations.size() << ",i:" << mIgnoredAnimations.size();

    if(finishedAnimations.size() > 0u)
    {
      oss << ",";
      std::sort(animationFinishedTimeChecker.rbegin(), animationFinishedTimeChecker.rend());
      auto topCount = std::min(5u, static_cast<uint32_t>(animationFinishedTimeChecker.size()));

      oss << "top" << topCount;
      for(auto i = 0u; i < topCount; ++i)
      {
        oss << "(" << static_cast<float>(animationFinishedTimeChecker[i].first) / 1000000.0f << "ms,";
        oss << animationFinishedTimeChecker[i].second << ")";
      }
    }
    oss << "]";
  });
}

uint32_t AnimationPlaylist::GetAnimationCount()
{
  return mAnimations.Count();
}

Dali::Animation AnimationPlaylist::GetAnimationAt(uint32_t index)
{
  if(index >= mAnimations.Count())
  {
    DALI_LOG_ERROR("Animation index is out of bounds.\n");
    return Dali::Animation();
  }

  // This will spend a lot of time. But GetAnimationAt API will be called very rarely.
  Animation* ret = nullptr;
  for(auto iter : mAnimations)
  {
    if(index == 0u)
    {
      ret = iter;
      break;
    }
    --index;
  }
  DALI_ASSERT_DEBUG(ret != nullptr);
  return Dali::Animation(ret);
}

} // namespace Internal

} // namespace Dali
