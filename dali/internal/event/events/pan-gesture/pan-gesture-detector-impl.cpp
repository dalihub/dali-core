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
#include <dali/internal/event/events/pan-gesture/pan-gesture-detector-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/events/pan-gesture/pan-gesture-recognizer.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Internal
{
namespace
{
const unsigned long MAXIMUM_TIME_WITH_VALID_LAST_VELOCITY(50u);
// Properties

//              Name                  Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY("screenPosition", VECTOR2, false, false, true, Dali::PanGestureDetector::Property::SCREEN_POSITION)
DALI_PROPERTY("screenDisplacement", VECTOR2, false, false, true, Dali::PanGestureDetector::Property::SCREEN_DISPLACEMENT)
DALI_PROPERTY("screenVelocity", VECTOR2, false, false, true, Dali::PanGestureDetector::Property::SCREEN_VELOCITY)
DALI_PROPERTY("localPosition", VECTOR2, false, false, true, Dali::PanGestureDetector::Property::LOCAL_POSITION)
DALI_PROPERTY("localDisplacement", VECTOR2, false, false, true, Dali::PanGestureDetector::Property::LOCAL_DISPLACEMENT)
DALI_PROPERTY("localVelocity", VECTOR2, false, false, true, Dali::PanGestureDetector::Property::LOCAL_VELOCITY)
DALI_PROPERTY("panning", BOOLEAN, false, false, true, Dali::PanGestureDetector::Property::PANNING)
DALI_PROPERTY_TABLE_END(DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX, PanGestureDetectorDefaultProperties)

// Signals

const char* const SIGNAL_PAN_DETECTED = "panDetected";

BaseHandle Create()
{
  return Dali::PanGestureDetector::New();
}

TypeRegistration mType(typeid(Dali::PanGestureDetector), typeid(Dali::GestureDetector), Create, PanGestureDetectorDefaultProperties);

SignalConnectorType signalConnector1(mType, SIGNAL_PAN_DETECTED, &PanGestureDetector::DoConnectSignal);

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_PAN_GESTURE_DETECTOR");
#endif

/**
 * Returns the angle going in the opposite direction to that specified by angle.
 */
float GetOppositeAngle(float angle)
{
  // Calculate the opposite angle so that we cover both directions.
  if(angle <= 0.0f)
  {
    angle += Math::PI;
  }
  else
  {
    angle -= Math::PI;
  }

  return angle;
}

} // unnamed namespace

PanGestureDetectorPtr PanGestureDetector::New()
{
  const SceneGraph::PanGesture& sceneObject = ThreadLocalStorage::Get().GetGestureEventProcessor().GetPanGestureProcessor().GetSceneObject();
  return new PanGestureDetector(sceneObject);
}

void PanGestureDetector::SetMinimumTouchesRequired(uint32_t minimum)
{
  DALI_ASSERT_ALWAYS(minimum > 0 && "Can only set a positive number of required touches");

  if(mMinimumTouches != minimum)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "Minimum Touches Set: %u\n", minimum);

    mMinimumTouches = minimum;

    if(!mAttachedActors.empty())
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "Updating Gesture Detector\n");

      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

void PanGestureDetector::SetMaximumTouchesRequired(uint32_t maximum)
{
  DALI_ASSERT_ALWAYS(maximum > 0 && "Can only set a positive number of maximum touches");

  if(mMaximumTouches != maximum)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "Maximum Touches Set: %u\n", maximum);

    mMaximumTouches = maximum;

    if(!mAttachedActors.empty())
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "Updating Gesture Detector\n");

      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

void PanGestureDetector::SetMaximumMotionEventAge(uint32_t maximumAge)
{
  if(mMaximumMotionEventAge != maximumAge)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "Maximum Motion Age Set: %u ms\n", maximumAge);

    mMaximumMotionEventAge = maximumAge;

    if(!mAttachedActors.empty())
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "Updating Gesture Detector\n");

      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

uint32_t PanGestureDetector::GetMinimumTouchesRequired() const
{
  return mMinimumTouches;
}

uint32_t PanGestureDetector::GetMaximumTouchesRequired() const
{
  return mMaximumTouches;
}

uint32_t PanGestureDetector::GetMaximumMotionEventAge() const
{
  return mMaximumMotionEventAge;
}

void PanGestureDetector::AddAngle(Radian angle, Radian threshold)
{
  threshold = fabsf(threshold); // Ensure the threshold is positive.

  // If the threshold is greater than PI, then just use PI
  // This means that any panned angle will invoke the pan gesture. We should still add this angle as
  // an angle may have been added previously with a small threshold.
  if(threshold > Math::PI)
  {
    threshold = Math::PI;
  }

  angle = WrapInDomain(angle, -Math::PI, Math::PI);

  DALI_LOG_INFO(gLogFilter, Debug::Concise, "Angle Added: %.2f, Threshold: %.2f\n", Degree(angle), Degree(threshold));

  AngleThresholdPair pair(angle, threshold);
  mAngleContainer.push_back(pair);
}

void PanGestureDetector::AddDirection(Radian direction, Radian threshold)
{
  AddAngle(direction, threshold);

  // Calculate the opposite angle so that we cover the entire direction.
  direction = GetOppositeAngle(direction);

  AddAngle(direction, threshold);
}

uint32_t PanGestureDetector::GetAngleCount() const
{
  return static_cast<uint32_t>(mAngleContainer.size());
}

PanGestureDetector::AngleThresholdPair PanGestureDetector::GetAngle(uint32_t index) const
{
  PanGestureDetector::AngleThresholdPair ret(Radian(0), Radian(0));

  if(index < mAngleContainer.size())
  {
    ret = mAngleContainer[index];
  }

  return ret;
}

void PanGestureDetector::ClearAngles()
{
  mAngleContainer.clear();
}

void PanGestureDetector::RemoveAngle(Radian angle)
{
  angle = WrapInDomain(angle, -Math::PI, Math::PI);

  for(AngleContainer::iterator iter = mAngleContainer.begin(), endIter = mAngleContainer.end(); iter != endIter; ++iter)
  {
    if(iter->first == angle)
    {
      mAngleContainer.erase(iter);
      break;
    }
  }
}

void PanGestureDetector::RemoveDirection(Radian direction)
{
  RemoveAngle(direction);

  // Calculate the opposite angle so that we cover the entire direction.
  direction = GetOppositeAngle(direction);

  RemoveAngle(direction);
}

bool PanGestureDetector::RequiresDirectionalPan() const
{
  // If no directional angles have been added to the container then we do not require directional panning
  return !mAngleContainer.empty();
}

bool PanGestureDetector::CheckAngleAllowed(Radian angle) const
{
  bool allowed(false);
  if(mAngleContainer.empty())
  {
    allowed = true;
  }
  else
  {
    for(AngleContainer::const_iterator iter = mAngleContainer.begin(), endIter = mAngleContainer.end(); iter != endIter; ++iter)
    {
      float angleAllowed(iter->first);
      float threshold(iter->second);

      DALI_LOG_INFO(gLogFilter, Debug::General, "AngleToCheck: %.2f, CompareWith: %.2f, Threshold: %.2f\n", Degree(angle), Degree(angleAllowed), Degree(threshold));

      float relativeAngle(fabsf(WrapInDomain(angle - angleAllowed, -Math::PI, Math::PI)));
      if(relativeAngle <= threshold)
      {
        allowed = true;
        break;
      }
    }
  }

  return allowed;
}

void PanGestureDetector::EmitPanGestureSignal(Dali::Actor actor, const Dali::PanGesture& pan)
{
  if(!mDetectedSignal.Empty())
  {
    // Guard against destruction during signal emission
    Dali::PanGestureDetector handle(this);

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Emitting Signal (%p)\n", this);
    if(pan.GetState() != GestureState::CONTINUING)
    {
      DALI_LOG_DEBUG_INFO("emitting pan gesture actor id(%d) state(%d)\n", actor.GetProperty<int32_t>(Dali::Actor::Property::ID), pan.GetState());
    }
    mDetectedSignal.Emit(actor, pan);
  }
}

bool PanGestureDetector::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  bool                connected(true);
  PanGestureDetector* gesture = static_cast<PanGestureDetector*>(object); // TypeRegistry guarantees that this is the correct type.

  if(0 == strcmp(signalName.c_str(), SIGNAL_PAN_DETECTED))
  {
    gesture->DetectedSignal().Connect(tracker, functor);
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void PanGestureDetector::SetPanGestureProperties(const Dali::PanGesture& pan)
{
  ThreadLocalStorage::Get().GetGestureEventProcessor().SetGestureProperties(pan);
}

PanGestureDetector::PanGestureDetector(const SceneGraph::PanGesture& sceneObject)
: GestureDetector(GestureType::PAN),
  mMinimumTouches(1),
  mMaximumTouches(1),
  mMaximumMotionEventAge(std::numeric_limits<uint32_t>::max()),
  mPossiblePanPosition(0.f, 0.f),
  mSceneObject(const_cast<SceneGraph::PanGesture*>(&sceneObject))
{
}

PanGestureDetector::~PanGestureDetector() = default;

const SceneGraph::PanGesture& PanGestureDetector::GetPanGestureSceneObject() const
{
  return static_cast<const SceneGraph::PanGesture&>(*mSceneObject);
}

void PanGestureDetector::OnActorAttach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "PanGestureDetector attach actor(%d)\n", actor.GetId());
  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchedSignal().Connect(this, &PanGestureDetector::OnTouchEvent);
  }
}

void PanGestureDetector::OnActorDetach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "PanGestureDetector detach actor(%d)\n", actor.GetId());
  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchedSignal().Disconnect(this, &PanGestureDetector::OnTouchEvent);
  }
}

void PanGestureDetector::OnActorDestroyed(Object& object)
{
  // Do nothing
}

bool PanGestureDetector::OnTouchEvent(Dali::Actor actor, const Dali::TouchEvent& touch)
{
  Dali::TouchEvent touchEvent(touch);
  return HandleEvent(actor, touchEvent);
}

void PanGestureDetector::SetDefaultProperty(Property::Index index, const Property::Value& property)
{
  // None of our properties should be settable from Public API
}

Property::Value PanGestureDetector::GetDefaultProperty(Property::Index index) const
{
  return GetDefaultPropertyCurrentValue(index); // Scene-graph only properties
}

Property::Value PanGestureDetector::GetDefaultPropertyCurrentValue(Property::Index index) const
{
  Property::Value value;

  switch(index)
  {
    case Dali::PanGestureDetector::Property::SCREEN_POSITION:
    {
      value = GetPanGestureSceneObject().GetScreenPositionProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::SCREEN_DISPLACEMENT:
    {
      value = GetPanGestureSceneObject().GetScreenDisplacementProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::SCREEN_VELOCITY:
    {
      value = GetPanGestureSceneObject().GetScreenVelocityProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_POSITION:
    {
      value = GetPanGestureSceneObject().GetLocalPositionProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_DISPLACEMENT:
    {
      value = GetPanGestureSceneObject().GetLocalDisplacementProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_VELOCITY:
    {
      value = GetPanGestureSceneObject().GetLocalVelocityProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::PANNING:
    {
      value = GetPanGestureSceneObject().GetPanningProperty().Get();
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS(false && "PanGestureDetector Property index invalid"); // should not come here
      break;
    }
  }

  return value;
}

const PropertyInputImpl* PanGestureDetector::GetSceneObjectInputProperty(Property::Index index) const
{
  const PropertyInputImpl* property = nullptr;

  switch(index)
  {
    case Dali::PanGestureDetector::Property::SCREEN_POSITION:
    {
      property = &GetPanGestureSceneObject().GetScreenPositionProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::SCREEN_DISPLACEMENT:
    {
      property = &GetPanGestureSceneObject().GetScreenDisplacementProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::SCREEN_VELOCITY:
    {
      property = &GetPanGestureSceneObject().GetScreenVelocityProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_POSITION:
    {
      property = &GetPanGestureSceneObject().GetLocalPositionProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_DISPLACEMENT:
    {
      property = &GetPanGestureSceneObject().GetLocalDisplacementProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_VELOCITY:
    {
      property = &GetPanGestureSceneObject().GetLocalVelocityProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::PANNING:
    {
      property = &GetPanGestureSceneObject().GetPanningProperty();
      break;
    }

    default:
      break;
  }
  if(!property)
  {
    // not our property, ask base
    property = Object::GetSceneObjectInputProperty(index);
  }

  return property;
}

void PanGestureDetector::ProcessTouchEvent(Scene& scene, const Integration::TouchEvent& event)
{
  if(!mGestureRecognizer)
  {
    const PanGestureProcessor& panGestureProcessor = mGestureEventProcessor.GetPanGestureProcessor();
    int32_t                    minDistance         = panGestureProcessor.GetMinimumDistance();
    int32_t                    minPanEvents        = panGestureProcessor.GetMinimumPanEvents();

    PanGestureRequest request;
    request.minTouches        = GetMinimumTouchesRequired();
    request.maxTouches        = GetMaximumTouchesRequired();
    request.maxMotionEventAge = GetMaximumMotionEventAge();

    Size size          = scene.GetSize();
    mGestureRecognizer = new PanGestureRecognizer(*this, Vector2(size.width, size.height), static_cast<const PanGestureRequest&>(request), minDistance, minPanEvents);
  }
  mGestureRecognizer->SendEvent(scene, event);
}

void PanGestureDetector::Process(Scene& scene, const PanGestureEvent& panEvent)
{
  switch(panEvent.state)
  {
    case GestureState::POSSIBLE:
    {
      mCurrentPanActor.SetActor(mFeededActor.GetActor());
      mPossiblePanPosition = panEvent.currentPosition;
      break;
    }

    case GestureState::STARTED:
    {
      // The pan gesture should only be sent to the gesture detector which first received it so that
      // it can be told when the gesture ends as well.
      Actor* feededActor = mFeededActor.GetActor();
      if(feededActor && CheckGestureDetector(&panEvent, feededActor, mRenderTask, mPossiblePanPosition))
      {
        Vector2 actorCoords;
        feededActor->ScreenToLocal(*mRenderTask.Get(), actorCoords.x, actorCoords.y, panEvent.currentPosition.x, panEvent.currentPosition.y);
        if(mCurrentPanActor.GetActor() == feededActor)
        {
          EmitPanSignal(feededActor, panEvent, actorCoords, panEvent.state, mRenderTask, scene);
        }
        else
        {
          mPossiblePanPosition = panEvent.previousPosition;
          mCurrentPanActor.SetActor(feededActor);
          EmitPanSignal(feededActor, panEvent, actorCoords, panEvent.state, mRenderTask, scene);
        }
      }
      break;
    }

    case GestureState::CONTINUING:
    {
      Actor* currentGesturedActor = mCurrentPanActor.GetActor();
      Actor* feededActor          = mFeededActor.GetActor();
      if(currentGesturedActor && currentGesturedActor->NeedGesturePropagation() && feededActor && feededActor != currentGesturedActor)
      {
        if(feededActor->IsHittable() && CheckGestureDetector(&panEvent, feededActor, mRenderTask, mPossiblePanPosition))
        {
          Vector2 actorCoords;
          feededActor->ScreenToLocal(*mRenderTask.Get(), actorCoords.x, actorCoords.y, panEvent.currentPosition.x, panEvent.currentPosition.y);

          mPossiblePanPosition = panEvent.currentPosition;
          mCurrentPanActor.SetActor(feededActor);
          EmitPanSignal(feededActor, panEvent, actorCoords, GestureState::STARTED, mRenderTask, scene);
        }
        break;
      }
      DALI_FALLTHROUGH;
    }

    case GestureState::FINISHED:
    case GestureState::CANCELLED:
    {
      // Only send subsequent pan gesture signals if we processed the pan gesture when it started.
      // Check if actor is still touchable.
      Actor* currentGesturedActor = mCurrentPanActor.GetActor();
      if(currentGesturedActor && currentGesturedActor->IsHittable() && mRenderTask && IsDetected())
      {
        Vector2 actorCoords;
        currentGesturedActor->ScreenToLocal(*mRenderTask.Get(), actorCoords.x, actorCoords.y, panEvent.currentPosition.x, panEvent.currentPosition.y);
        EmitPanSignal(currentGesturedActor, panEvent, actorCoords, panEvent.state, mRenderTask, scene);
      }

      if((panEvent.state == GestureState::FINISHED) || (panEvent.state == GestureState::CANCELLED))
      {
        mCurrentPanActor.SetActor(nullptr);
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

bool PanGestureDetector::CheckGestureDetector(const GestureEvent* gestureEvent, Actor* actor, RenderTaskPtr renderTask, Vector2 possiblePanPosition)
{
  CheckGestureDetector(gestureEvent, actor, renderTask);
  const PanGestureEvent* panEvent(static_cast<const PanGestureEvent*>(gestureEvent));

  bool retVal(false);

  if((panEvent->numberOfTouches >= GetMinimumTouchesRequired()) &&
     (panEvent->numberOfTouches <= GetMaximumTouchesRequired()))
  {
    // Check if the detector requires directional panning.
    if(RequiresDirectionalPan() && renderTask)
    {
      // It does, calculate the angle of the pan in local actor coordinates and ensures it fits
      // the detector's criteria.
      RenderTask& renderTaskImpl = *renderTask.Get();

      Vector2 startPosition, currentPosition;
      actor->ScreenToLocal(renderTaskImpl, startPosition.x, startPosition.y, possiblePanPosition.x, possiblePanPosition.y);
      actor->ScreenToLocal(renderTaskImpl, currentPosition.x, currentPosition.y, panEvent->currentPosition.x, panEvent->currentPosition.y);
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

      if(CheckAngleAllowed(angle))
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

void PanGestureDetector::CancelProcessing()
{
  if(mGestureRecognizer)
  {
    mGestureRecognizer->CancelEvent();
  }
}

void PanGestureDetector::EmitPanSignal(Actor*                 actor,
                                       const PanGestureEvent& panEvent,
                                       Vector2                localCurrent,
                                       GestureState           state,
                                       RenderTaskPtr          renderTask,
                                       Scene&                 scene)
{
  SetDetected(true);
  Internal::PanGesturePtr pan(new Internal::PanGesture(panEvent.state));

  pan->SetTime(panEvent.time);

  pan->SetNumberOfTouches(panEvent.numberOfTouches);
  pan->SetScreenPosition(panEvent.currentPosition);
  pan->SetPosition(localCurrent);
  pan->SetSourceType(panEvent.sourceType);
  pan->SetSourceData(panEvent.sourceData);

  RenderTask& renderTaskImpl(*renderTask.Get());

  Vector2 localPrevious;
  actor->ScreenToLocal(renderTaskImpl, localPrevious.x, localPrevious.y, panEvent.previousPosition.x, panEvent.previousPosition.y);

  pan->SetDisplacement(localCurrent - localPrevious);
  Vector2 previousPos(panEvent.previousPosition);
  if(panEvent.state == GestureState::STARTED)
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
  if((panEvent.state == GestureState::FINISHED) && (pan->GetScreenVelocity() == Vector2::ZERO) &&
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

  // We should not use scene object if Core is shutting down.
  if(DALI_LIKELY(Dali::Stage::IsInstalled() && mSceneObject))
  {
    // We update the scene object directly rather than sending a message.
    // Sending a message could cause unnecessary delays, the scene object ensure thread safe behaviour.
    mSceneObject->AddGesture(*pan.Get());
  }

  // store the state;
  scene.SetLastPanGestureState(state);

  Dali::Actor actorHandle(actor);

  EmitPanGestureSignal(actorHandle, Dali::PanGesture(pan.Get()));
}

} // namespace Internal

} // namespace Dali
