/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/pan-gesture/pan-gesture-processor.h>

#if defined(DEBUG_ENABLED)
#include <sstream>
#endif

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/events/multi-point-event-util.h>
#include <dali/internal/event/events/pan-gesture/pan-gesture-event.h>
#include <dali/internal/event/events/pan-gesture/pan-gesture-impl.h>
#include <dali/internal/event/events/pan-gesture/pan-gesture-recognizer.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
namespace Internal
{
namespace // unnamed namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_PAN_PROCESSOR");

const char* GESTURE_STATES[6] =
  {
    "CLEAR",
    "STARTED",
    "CONTINUING",
    "FINISHED",
    "CANCELLED",
    "POSSIBLE"};

#endif // defined(DEBUG_ENABLED)

const unsigned long MAXIMUM_TIME_WITH_VALID_LAST_VELOCITY(50u);

/**
 * Functor which checks whether the specified actor is attached to the gesture detector.
 * If the actor is attached, it also checks whether the number of touches of the current pan event
 * are within the range of that expected by the detector.
 * It returns true if it is no longer attached or the touches are out of range.
 * This can be used in remove_if functions.
 */
struct IsNotAttachedAndOutsideTouchesRangeFunctor
{
  /**
   * Constructor
   * @param[in]  actor                 The actor to check whether it is attached.
   * @param[in]  touches               The number of touches in the current pan event.
   * @param[in]  outsideRangeEmitters  Reference to container where emitters outside of the touches range should be added.
   */
  IsNotAttachedAndOutsideTouchesRangeFunctor(Actor* actor, unsigned int touches, GestureDetectorContainer& outsideRangeEmitters)
  : actorToCheck(actor),
    numberOfTouches(touches),
    outsideTouchesRangeEmitters(outsideRangeEmitters)
  {
  }

  /**
   * Returns true if not attached, false if it is still attached.
   * Additionally, checks if the number of touches has changed and stops sending the pan to a particular
   * detector if it exceeds the range of that detector.
   * @param[in]  detector  The detector to check.
   * @return true, if not attached, false otherwise.
   */
  bool operator()(GestureDetector* detector) const
  {
    bool remove(!detector->IsAttached(*actorToCheck));

    if(!remove)
    {
      PanGestureDetector* panDetector(static_cast<PanGestureDetector*>(detector));

      // Ensure number of touch points is within the range of our emitter. If it isn't then remove
      // this emitter and add it to the outsideTouchesRangeEmitters container
      if((numberOfTouches < panDetector->GetMinimumTouchesRequired()) ||
         (numberOfTouches > panDetector->GetMaximumTouchesRequired()))
      {
        remove = true;
        outsideTouchesRangeEmitters.push_back(detector);
      }
    }

    return remove;
  }

  Actor*                    actorToCheck;                ///< The actor to check whether it is attached or not.
  unsigned int              numberOfTouches;             ///< The number of touches in the pan event.
  GestureDetectorContainer& outsideTouchesRangeEmitters; ///< Emitters that are outside of the range of current pan.
};

} // unnamed namespace

PanGestureProcessor::PanGestureProcessor(SceneGraph::UpdateManager& updateManager)
: GestureProcessor(GestureType::PAN),
  mPanGestureDetectors(),
  mCurrentPanEmitters(),
  mCurrentRenderTask(),
  mPossiblePanPosition(),
  mMinTouchesRequired(1),
  mMaxTouchesRequired(1),
  mMaxMotionEventAge(std::numeric_limits<uint32_t>::max()),
  mCurrentPanEvent(nullptr),
  mSceneObject(SceneGraph::PanGesture::New()) // Create scene object to store pan information.
{
  // Pass ownership to scene-graph; scene object lives for the lifecycle of UpdateManager
  updateManager.SetPanGestureProcessor(mSceneObject);
}

PanGestureProcessor::~PanGestureProcessor()
{
  mSceneObject = nullptr; // mSceneObject is owned and destroyed by update manager (there is only one of these for now)
}

void PanGestureProcessor::Process(Scene& scene, const PanGestureEvent& panEvent)
{
#if defined(DEBUG_ENABLED)
  DALI_LOG_TRACE_METHOD(gLogFilter);

  DALI_LOG_INFO(gLogFilter, Debug::General, "    Pan Event\n");
  DALI_LOG_INFO(gLogFilter, Debug::General, "      State: %s  Touches: %d  Time: %d  TimeDelta: %d\n", GESTURE_STATES[static_cast<uint8_t>(panEvent.state)], panEvent.numberOfTouches, panEvent.time, panEvent.timeDelta);
  DALI_LOG_INFO(gLogFilter, Debug::General, "      Positions: Current: (%.0f, %.0f), Previous: (%.0f, %.0f)\n", panEvent.currentPosition.x, panEvent.currentPosition.y, panEvent.previousPosition.x, panEvent.previousPosition.y);
#endif

  switch(panEvent.state)
  {
    case GestureState::POSSIBLE:
    {
      mCurrentPanEmitters.clear();
      ResetActor();

      HitTestAlgorithm::Results hitTestResults;
      if(HitTest(scene, panEvent.currentPosition, hitTestResults))
      {
        SetActor(&GetImplementation(hitTestResults.actor));
        mPossiblePanPosition = panEvent.currentPosition;
      }
      break;
    }

    case GestureState::STARTED:
    {
      // Requires a core update
      mNeedsUpdate = true;

      // The pan gesture should only be sent to the gesture detector which first received it so that
      // it can be told when the gesture ends as well.

      HitTestAlgorithm::Results hitTestResults;
      HitTest(scene, panEvent.previousPosition, hitTestResults); // Hit Test previous position

      if(hitTestResults.actor)
      {
        // If the current hit actor is different from the one we touched down on then set accordingly & update initial pan position
        if(&GetImplementation(hitTestResults.actor) != GetCurrentGesturedActor())
        {
          mPossiblePanPosition = panEvent.previousPosition;
          SetActor(&GetImplementation(hitTestResults.actor));
        }

        // Record the current render-task for Screen->Actor coordinate conversions
        mCurrentRenderTask = hitTestResults.renderTask;

        // Set mCurrentPanEvent to use inside overridden methods called in ProcessAndEmit()
        mCurrentPanEvent = &panEvent;
        ProcessAndEmit(hitTestResults);
        mCurrentPanEvent = nullptr;
      }
      else
      {
        ResetActor();
        mCurrentPanEmitters.clear();
      }
      break;
    }

    case GestureState::CONTINUING:
    {
      // Requires a core update
      mNeedsUpdate = true;

      DALI_FALLTHROUGH;
    }

    case GestureState::FINISHED:
    case GestureState::CANCELLED:
    {
      // Only send subsequent pan gesture signals if we processed the pan gesture when it started.
      // Check if actor is still touchable.

      Actor* currentGesturedActor = GetCurrentGesturedActor();
      if(currentGesturedActor)
      {
        if(currentGesturedActor->IsHittable() && !mCurrentPanEmitters.empty() && mCurrentRenderTask)
        {
          GestureDetectorContainer outsideTouchesRangeEmitters;

          // Removes emitters that no longer have the actor attached
          // Also remove emitters whose touches are outside the range of the current pan event and add them to outsideTouchesRangeEmitters
          GestureDetectorContainer::iterator endIter = std::remove_if(mCurrentPanEmitters.begin(), mCurrentPanEmitters.end(), IsNotAttachedAndOutsideTouchesRangeFunctor(currentGesturedActor, panEvent.numberOfTouches, outsideTouchesRangeEmitters));
          mCurrentPanEmitters.erase(endIter, mCurrentPanEmitters.end());

          Vector2 actorCoords;

          if(!outsideTouchesRangeEmitters.empty() || !mCurrentPanEmitters.empty())
          {
            currentGesturedActor->ScreenToLocal(*mCurrentRenderTask.Get(), actorCoords.x, actorCoords.y, panEvent.currentPosition.x, panEvent.currentPosition.y);

            // EmitPanSignal checks whether we have a valid actor and whether the container we are passing in has emitters before it emits the pan.
            EmitPanSignal(currentGesturedActor, outsideTouchesRangeEmitters, panEvent, actorCoords, GestureState::FINISHED, mCurrentRenderTask);
            EmitPanSignal(currentGesturedActor, mCurrentPanEmitters, panEvent, actorCoords, panEvent.state, mCurrentRenderTask);
          }

          if(mCurrentPanEmitters.empty())
          {
            // If we have no emitters attached then clear pan actor as well.
            ResetActor();
          }

          // Clear current gesture detectors if pan gesture has ended or been cancelled.
          if((panEvent.state == GestureState::FINISHED) || (panEvent.state == GestureState::CANCELLED))
          {
            mCurrentPanEmitters.clear();
            ResetActor();
          }
        }
        else
        {
          mCurrentPanEmitters.clear();
          ResetActor();
        }
      }
      break;
    }

    case GestureState::CLEAR:
    {
      DALI_ABORT("Incorrect state received from Integration layer: CLEAR\n");
      break;
    }
  }
}

void PanGestureProcessor::AddGestureDetector(PanGestureDetector* gestureDetector, Scene& scene, int32_t minDistance, int32_t minPanEvents)
{
  bool firstRegistration(mPanGestureDetectors.empty());

  mPanGestureDetectors.push_back(gestureDetector);

  if(firstRegistration)
  {
    mMinTouchesRequired = gestureDetector->GetMinimumTouchesRequired();
    mMaxTouchesRequired = gestureDetector->GetMaximumTouchesRequired();
    mMaxMotionEventAge  = gestureDetector->GetMaximumMotionEventAge();

    PanGestureRequest request;
    request.minTouches        = mMinTouchesRequired;
    request.maxTouches        = mMaxTouchesRequired;
    request.maxMotionEventAge = mMaxMotionEventAge;

    Size size          = scene.GetSize();
    mGestureRecognizer = new PanGestureRecognizer(*this, Vector2(size.width, size.height), static_cast<const PanGestureRequest&>(request), minDistance, minPanEvents);
  }
  else
  {
    UpdateDetection();
  }
}

void PanGestureProcessor::RemoveGestureDetector(PanGestureDetector* gestureDetector)
{
  if(!mCurrentPanEmitters.empty())
  {
    // Check if the removed detector was one that is currently being panned and remove it from emitters.
    GestureDetectorContainer::iterator endIter = std::remove(mCurrentPanEmitters.begin(), mCurrentPanEmitters.end(), gestureDetector);
    mCurrentPanEmitters.erase(endIter, mCurrentPanEmitters.end());

    // If we no longer have any emitters, then we should clear mCurrentGesturedActor as well
    if(mCurrentPanEmitters.empty())
    {
      ResetActor();
    }
  }

  // Find the detector...
  PanGestureDetectorContainer::iterator endIter = std::remove(mPanGestureDetectors.begin(), mPanGestureDetectors.end(), gestureDetector);
  DALI_ASSERT_DEBUG(endIter != mPanGestureDetectors.end());

  // ...and remove it
  mPanGestureDetectors.erase(endIter, mPanGestureDetectors.end());

  if(mPanGestureDetectors.empty())
  {
    mGestureRecognizer = nullptr;
  }
  else
  {
    UpdateDetection();
  }
}

void PanGestureProcessor::GestureDetectorUpdated(PanGestureDetector* gestureDetector)
{
  DALI_ASSERT_DEBUG(find(mPanGestureDetectors.begin(), mPanGestureDetectors.end(), gestureDetector) != mPanGestureDetectors.end());

  UpdateDetection();
}

bool PanGestureProcessor::SetPanGestureProperties(const Dali::PanGesture& pan)
{
  // If we are currently processing a pan gesture then just ignore
  if(mCurrentPanEmitters.empty() && mSceneObject)
  {
    const PanGesture& panImpl(GetImplementation(pan));

    // We update the scene object directly rather than sending a message.
    // Sending a message could cause unnecessary delays, the scene object ensure thread safe behaviour.
    mSceneObject->AddGesture(panImpl);

    if(GestureState::STARTED == panImpl.GetState() || GestureState::CONTINUING == panImpl.GetState())
    {
      mNeedsUpdate = true;
    }
  }

  return mNeedsUpdate;
}

void PanGestureProcessor::EnableProfiling()
{
  mSceneObject->EnableProfiling();
}

void PanGestureProcessor::SetPredictionMode(int mode)
{
  if((mode < 0) || (mode >= SceneGraph::PanGesture::NUM_PREDICTION_MODES))
  {
    mode = SceneGraph::PanGesture::DEFAULT_PREDICTION_MODE;
  }
  SceneGraph::PanGesture::PredictionMode modeEnum = static_cast<SceneGraph::PanGesture::PredictionMode>(mode);
  mSceneObject->SetPredictionMode(modeEnum);
}

void PanGestureProcessor::SetPredictionAmount(unsigned int amount)
{
  mSceneObject->SetPredictionAmount(amount);
}

void PanGestureProcessor::SetMaximumPredictionAmount(unsigned int amount)
{
  mSceneObject->SetMaximumPredictionAmount(amount);
}

void PanGestureProcessor::SetMinimumPredictionAmount(unsigned int amount)
{
  mSceneObject->SetMinimumPredictionAmount(amount);
}

void PanGestureProcessor::SetPredictionAmountAdjustment(unsigned int amount)
{
  mSceneObject->SetPredictionAmountAdjustment(amount);
}

void PanGestureProcessor::SetSmoothingMode(int mode)
{
  if((mode < 0) || (mode >= SceneGraph::PanGesture::NUM_SMOOTHING_MODES))
  {
    mode = SceneGraph::PanGesture::DEFAULT_SMOOTHING_MODE;
  }
  SceneGraph::PanGesture::SmoothingMode modeEnum = static_cast<SceneGraph::PanGesture::SmoothingMode>(mode);
  mSceneObject->SetSmoothingMode(modeEnum);
}

void PanGestureProcessor::SetSmoothingAmount(float amount)
{
  mSceneObject->SetSmoothingAmount(amount);
}

void PanGestureProcessor::SetUseActualTimes(bool value)
{
  mSceneObject->SetUseActualTimes(value);
}

void PanGestureProcessor::SetInterpolationTimeRange(int value)
{
  mSceneObject->SetInterpolationTimeRange(value);
}

void PanGestureProcessor::SetScalarOnlyPredictionEnabled(bool value)
{
  mSceneObject->SetScalarOnlyPredictionEnabled(value);
}

void PanGestureProcessor::SetTwoPointPredictionEnabled(bool value)
{
  mSceneObject->SetTwoPointPredictionEnabled(value);
}

void PanGestureProcessor::SetTwoPointInterpolatePastTime(int value)
{
  mSceneObject->SetTwoPointInterpolatePastTime(value);
}

void PanGestureProcessor::SetTwoPointVelocityBias(float value)
{
  mSceneObject->SetTwoPointVelocityBias(value);
}

void PanGestureProcessor::SetTwoPointAccelerationBias(float value)
{
  mSceneObject->SetTwoPointAccelerationBias(value);
}

void PanGestureProcessor::SetMultitapSmoothingRange(int value)
{
  mSceneObject->SetMultitapSmoothingRange(value);
}

const SceneGraph::PanGesture& PanGestureProcessor::GetSceneObject() const
{
  return *mSceneObject;
}

void PanGestureProcessor::UpdateDetection()
{
  DALI_ASSERT_DEBUG(!mPanGestureDetectors.empty());

  uint32_t minimumRequired       = std::numeric_limits<uint32_t>::max();
  uint32_t maximumRequired       = 0;
  uint32_t maximumMotionEventAge = std::numeric_limits<uint32_t>::max();

  for(PanGestureDetectorContainer::iterator iter = mPanGestureDetectors.begin(), endIter = mPanGestureDetectors.end(); iter != endIter; ++iter)
  {
    PanGestureDetector* detector(*iter);

    if(detector)
    {
      uint32_t minimum = detector->GetMinimumTouchesRequired();
      if(minimum < minimumRequired)
      {
        minimumRequired = minimum;
      }

      uint32_t maximum = detector->GetMaximumTouchesRequired();
      if(maximum > maximumRequired)
      {
        maximumRequired = maximum;
      }

      uint32_t maximumAge = detector->GetMaximumMotionEventAge();
      if(maximumAge < maximumMotionEventAge)
      {
        maximumMotionEventAge = maximumAge;
      }
    }
  }

  if((minimumRequired != mMinTouchesRequired) || (maximumRequired != mMaxTouchesRequired) || (maximumMotionEventAge != mMaxMotionEventAge))
  {
    mMinTouchesRequired = minimumRequired;
    mMaxTouchesRequired = maximumRequired;
    mMaxMotionEventAge  = maximumMotionEventAge;

    PanGestureRequest request;
    request.minTouches        = mMinTouchesRequired;
    request.maxTouches        = mMaxTouchesRequired;
    request.maxMotionEventAge = mMaxMotionEventAge;
    mGestureRecognizer->Update(request);
  }
}

void PanGestureProcessor::EmitPanSignal(Actor*                          actor,
                                        const GestureDetectorContainer& gestureDetectors,
                                        const PanGestureEvent&          panEvent,
                                        Vector2                         localCurrent,
                                        GestureState                    state,
                                        RenderTaskPtr                   renderTask)
{
  if(actor && !gestureDetectors.empty())
  {
    Internal::PanGesturePtr pan(new Internal::PanGesture(state));

    pan->SetTime(panEvent.time);

    pan->SetNumberOfTouches(panEvent.numberOfTouches);
    pan->SetScreenPosition(panEvent.currentPosition);
    pan->SetPosition(localCurrent);

    RenderTask& renderTaskImpl(*renderTask.Get());

    Vector2 localPrevious;
    actor->ScreenToLocal(renderTaskImpl, localPrevious.x, localPrevious.y, panEvent.previousPosition.x, panEvent.previousPosition.y);

    pan->SetDisplacement(localCurrent - localPrevious);
    Vector2 previousPos(panEvent.previousPosition);
    if(state == GestureState::STARTED)
    {
      previousPos = mPossiblePanPosition;
    }

    pan->SetScreenDisplacement(panEvent.currentPosition - previousPos);

    // Avoid dividing by 0
    if(panEvent.timeDelta > 0)
    {
      Vector2 velocity;
      velocity.x = pan->GetDisplacement().x / static_cast<float>(panEvent.timeDelta);
      velocity.y = pan->GetDisplacement().y / static_cast<float>(panEvent.timeDelta);
      pan->SetVelocity(velocity);

      Vector2 screenVelocity;
      screenVelocity.x = pan->GetScreenDisplacement().x / static_cast<float>(panEvent.timeDelta);
      screenVelocity.y = pan->GetScreenDisplacement().y / static_cast<float>(panEvent.timeDelta);
      pan->SetScreenVelocity(screenVelocity);
    }

    // When the gesture ends, we may incorrectly get a ZERO velocity (as we have lifted our finger without any movement)
    // so we should use the last recorded velocity instead in this scenario.
    if((state == GestureState::FINISHED) && (pan->GetScreenVelocity() == Vector2::ZERO) &&
       (panEvent.timeDelta < MAXIMUM_TIME_WITH_VALID_LAST_VELOCITY))
    {
      pan->SetVelocity(mLastVelocity);
      pan->SetScreenVelocity(mLastScreenVelocity);
    }
    else
    {
      // Store the current velocity for future iterations.
      mLastVelocity       = pan->GetVelocity();
      mLastScreenVelocity = pan->GetScreenVelocity();
    }

    if(mSceneObject)
    {
      // We update the scene object directly rather than sending a message.
      // Sending a message could cause unnecessary delays, the scene object ensure thread safe behaviour.
      mSceneObject->AddGesture(*pan.Get());
    }

    Dali::Actor actorHandle(actor);

    const GestureDetectorContainer::const_iterator endIter = gestureDetectors.end();
    for(GestureDetectorContainer::const_iterator iter = gestureDetectors.begin(); iter != endIter; ++iter)
    {
      static_cast<PanGestureDetector*>(*iter)->EmitPanGestureSignal(actorHandle, Dali::PanGesture(pan.Get()));
    }
  }
}

void PanGestureProcessor::OnGesturedActorStageDisconnection()
{
  mCurrentPanEmitters.clear();
}

bool PanGestureProcessor::CheckGestureDetector(GestureDetector* detector, Actor* actor)
{
  DALI_ASSERT_DEBUG(mCurrentPanEvent);

  bool                retVal(false);
  PanGestureDetector* panDetector(static_cast<PanGestureDetector*>(detector));

  if((mCurrentPanEvent->numberOfTouches >= panDetector->GetMinimumTouchesRequired()) &&
     (mCurrentPanEvent->numberOfTouches <= panDetector->GetMaximumTouchesRequired()))
  {
    // Check if the detector requires directional panning.
    if(panDetector->RequiresDirectionalPan() && mCurrentRenderTask)
    {
      // It does, calculate the angle of the pan in local actor coordinates and ensures it fits
      // the detector's criteria.
      RenderTask& renderTaskImpl = *mCurrentRenderTask.Get();

      Vector2 startPosition, currentPosition;
      actor->ScreenToLocal(renderTaskImpl, startPosition.x, startPosition.y, mPossiblePanPosition.x, mPossiblePanPosition.y);
      actor->ScreenToLocal(renderTaskImpl, currentPosition.x, currentPosition.y, mCurrentPanEvent->currentPosition.x, mCurrentPanEvent->currentPosition.y);
      Vector2 displacement(currentPosition - startPosition);

      Radian angle(atanf(displacement.y / displacement.x));

      /////////////////////////////
      //            |            //
      //            |            //
      //   Q3 (-,-) | Q4 (+,-)   //
      //            |            //
      //    ----------------- +x //
      //            |            //
      //   Q2 (-,+) | Q1 (+,+)   //
      //            |            //
      //            |            //
      //           +y            //
      /////////////////////////////
      // Quadrant 1: As is
      // Quadrant 2: 180 degrees + angle
      // Quadrant 3: angle - 180 degrees
      // Quadrant 4: As is
      /////////////////////////////

      if(displacement.x < 0.0f)
      {
        if(displacement.y >= 0.0f)
        {
          // Quadrant 2
          angle.radian += Math::PI;
        }
        else
        {
          // Quadrant 3
          angle.radian -= Math::PI;
        }
      }

      if(panDetector->CheckAngleAllowed(angle))
      {
        retVal = true;
      }
    }
    else
    {
      // Directional panning not required so we can use this actor and gesture detector.
      retVal = true;
    }
  }
  return retVal;
}

void PanGestureProcessor::EmitGestureSignal(Actor* actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates)
{
  DALI_ASSERT_DEBUG(mCurrentPanEvent);

  mCurrentPanEmitters.clear();
  ResetActor();

  actor->ScreenToLocal(*mCurrentRenderTask.Get(), actorCoordinates.x, actorCoordinates.y, mCurrentPanEvent->currentPosition.x, mCurrentPanEvent->currentPosition.y);

  EmitPanSignal(actor, gestureDetectors, *mCurrentPanEvent, actorCoordinates, mCurrentPanEvent->state, mCurrentRenderTask);

  if(actor->OnScene())
  {
    mCurrentPanEmitters = gestureDetectors;
    SetActor(actor);
  }
}

} // namespace Internal

} // namespace Dali
