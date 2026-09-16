/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <algorithm>
#include <cstring> // for strcmp
#include <limits>

// INTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/events/pan-gesture/pan-gesture-recognizer.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/radian.h>

using Dali::Integration::ToStdStringView;

namespace DALI_NAMESPACE
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

SignalConnectorType signalConnector1(mType, Dali::String(SIGNAL_PAN_DETECTED), &PanGestureDetector::DoConnectSignal);

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_PAN_GESTURE_DETECTOR");
#endif

} // unnamed namespace

PanGestureDetectorPtr PanGestureDetector::New()
{
  const SceneGraph::PanGesture& sceneObject = ThreadLocalStorage::Get().GetGestureEventProcessor().GetPanGestureProcessor().GetSceneObject();
  return new PanGestureDetector(sceneObject);
}

void PanGestureDetector::SetMinimumTouchesRequired(uint32_t minimum)
{
  DALI_ASSERT_ALWAYS(minimum > 0 && "Can only set a positive number of required touches");

  if(mDefaultProfile.minimumTouches != minimum)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "Minimum Touches Set: %u\n", minimum);

    mDefaultProfile.minimumTouches = minimum;
    NotifyProfilesChanged();
  }
}

void PanGestureDetector::SetMaximumTouchesRequired(uint32_t maximum)
{
  DALI_ASSERT_ALWAYS(maximum > 0 && "Can only set a positive number of maximum touches");

  if(mDefaultProfile.maximumTouches != maximum)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "Maximum Touches Set: %u\n", maximum);

    mDefaultProfile.maximumTouches = maximum;
    NotifyProfilesChanged();
  }
}

void PanGestureDetector::SetMaximumMotionEventAge(uint32_t maximumAge)
{
  if(mDefaultProfile.maximumMotionEventAge != maximumAge)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "Maximum Motion Age Set: %u ms\n", maximumAge);

    mDefaultProfile.maximumMotionEventAge = maximumAge;
    NotifyProfilesChanged();
  }
}

uint32_t PanGestureDetector::GetMinimumTouchesRequired() const
{
  return mDefaultProfile.minimumTouches;
}

uint32_t PanGestureDetector::GetMaximumTouchesRequired() const
{
  return mDefaultProfile.maximumTouches;
}

uint32_t PanGestureDetector::GetMaximumMotionEventAge() const
{
  return mDefaultProfile.maximumMotionEventAge;
}

void PanGestureDetector::AddAngle(Radian angle, Radian threshold)
{
  mDefaultProfile.AddAngle(angle, threshold);
  DALI_LOG_INFO(gLogFilter, Debug::Concise, "Angle Added: %.2f, Threshold: %.2f\n", Degree(angle), Degree(threshold));
}

void PanGestureDetector::AddDirection(Radian direction, Radian threshold)
{
  mDefaultProfile.AddDirection(direction, threshold);
}

uint32_t PanGestureDetector::GetAngleCount() const
{
  return mDefaultProfile.GetAngleCount();
}

PanGestureDetector::AngleThresholdPair PanGestureDetector::GetAngle(uint32_t index) const
{
  return mDefaultProfile.GetAngle(index);
}

void PanGestureDetector::ClearAngles()
{
  mDefaultProfile.ClearAngles();
}

void PanGestureDetector::RemoveAngle(Radian angle)
{
  mDefaultProfile.RemoveAngle(angle);
}

void PanGestureDetector::RemoveDirection(Radian direction)
{
  mDefaultProfile.RemoveDirection(direction);
}

bool PanGestureDetector::RequiresDirectionalPan() const
{
  return mActiveProfile.RequiresDirectionalPan();
}

bool PanGestureDetector::CheckAngleAllowed(Radian angle) const
{
  return mActiveProfile.IsAngleAllowed(angle);
}

const PanGestureProfile& PanGestureDetector::GetDefaultProfile() const
{
  return mDefaultProfile;
}

void PanGestureDetector::SetDeviceProfile(const GestureDeviceSelector& selector, const PanGestureProfile& profile)
{
  DALI_ASSERT_ALWAYS(profile.minimumTouches > 0u && profile.maximumTouches > 0u && "Pan options require positive touch counts");
  DALI_ASSERT_ALWAYS(profile.minimumTouches <= profile.maximumTouches && "Pan options require minimum touches <= maximum touches");

  mDeviceProfiles.Set(selector, profile);
  NotifyProfilesChanged();
}

const PanGestureProfile* PanGestureDetector::GetDeviceProfile(const GestureDeviceSelector& selector) const
{
  return mDeviceProfiles.Find(selector);
}

void PanGestureDetector::ClearDeviceProfile(const GestureDeviceSelector& selector)
{
  if(mDeviceProfiles.Clear(selector))
  {
    NotifyProfilesChanged();
  }
}

const PanGestureProfile& PanGestureDetector::GetActiveProfile() const
{
  return mActiveProfile;
}

void PanGestureDetector::WidenRecognitionEnvelope(uint32_t& minimumTouches, uint32_t& maximumTouches, uint32_t& maximumMotionEventAge) const
{
  auto widen = [&](const PanGestureProfile& profile)
  {
    minimumTouches        = std::min(minimumTouches, profile.minimumTouches);
    maximumTouches        = std::max(maximumTouches, profile.maximumTouches);
    maximumMotionEventAge = std::max(maximumMotionEventAge, profile.maximumMotionEventAge);
  };

  widen(mDefaultProfile);
  mDeviceProfiles.ForEach(widen);
}

void PanGestureDetector::SelectActiveProfile(const GestureInputSource& source)
{
  const PanGestureProfile* profile = mDeviceProfiles.Resolve(source);
  mActiveProfile                   = profile ? *profile : mDefaultProfile;
}

void PanGestureDetector::NotifyProfilesChanged()
{
  MarkRecognizerSettingsDirty();

  if(!mAttachedActors.empty())
  {
    DALI_LOG_INFO(gLogFilter, Debug::General, "Updating Gesture Detector\n");
    mGestureEventProcessor.GestureDetectorUpdated(this);
  }
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

bool PanGestureDetector::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName, FunctorDelegate* functor)
{
  bool                connected(true);
  PanGestureDetector* gesture = static_cast<PanGestureDetector*>(object); // TypeRegistry guarantees that this is the correct type.

  std::string_view name = ToStdStringView(signalName);
  if(name == SIGNAL_PAN_DETECTED)
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
  mDefaultProfile(),
  mDeviceProfiles(),
  mActiveProfile(),
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
    actor.TouchEventSignal().Connect(this, &PanGestureDetector::OnTouchEvent);
  }
}

void PanGestureDetector::OnActorDetach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "PanGestureDetector detach actor(%d)\n", actor.GetId());
  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchEventSignal().Disconnect(this, &PanGestureDetector::OnTouchEvent);
  }
}

void PanGestureDetector::OnActorDestroyed(Object& object)
{
  // Do nothing
}

bool PanGestureDetector::OnTouchEvent(Dali::Actor actor, Dali::TouchEvent touch)
{
  Dali::PanGestureDetector handle = Dali::PanGestureDetector(this); ///< Keep reference until HandleEvent finished.
  return HandleEvent(actor, touch);
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

void PanGestureDetector::FillRequest(PanGestureRequest& request) const
{
  const PanGestureProcessor& panGestureProcessor = mGestureEventProcessor.GetPanGestureProcessor();

  uint32_t minimumTouches        = std::numeric_limits<uint32_t>::max();
  uint32_t maximumTouches        = 0u;
  uint32_t maximumMotionEventAge = 0u;
  WidenRecognitionEnvelope(minimumTouches, maximumTouches, maximumMotionEventAge);

  request.minTouches        = minimumTouches;
  request.maxTouches        = maximumTouches;
  request.maxMotionEventAge = maximumMotionEventAge;
  request.minimumDistance   = panGestureProcessor.GetMinimumDistance();
  request.minimumPanEvents  = panGestureProcessor.GetMinimumPanEvents();
  request.deviceThresholds  = panGestureProcessor.GetDeviceThresholds();
}

void PanGestureDetector::ProcessTouchEvent(Scene& scene, const Integration::TouchEvent& event)
{
  if(!mGestureRecognizer)
  {
    PanGestureRequest request;
    FillRequest(request);

    Size size          = scene.GetSize();
    mGestureRecognizer = new PanGestureRecognizer(*this, Vector2(size.width, size.height), request);
    ConsumeRecognizerUpdateRequired(); // The new recognizer already reflects the current settings.
  }
  else if(ConsumeRecognizerUpdateRequired())
  {
    PanGestureRequest request;
    FillRequest(request);
    mGestureRecognizer->Update(request);
  }
  mGestureRecognizer->SendEvent(scene, event);
}

void PanGestureDetector::Process(Scene& scene, const PanGestureEvent& panEvent)
{
  GestureState state = panEvent.state;
  if(state == GestureState::CONTINUING &&
     (panEvent.numberOfTouches < mActiveProfile.minimumTouches || panEvent.numberOfTouches > mActiveProfile.maximumTouches))
  {
    // The recognizer accepts every registered profile. End this detector's pan when its own
    // touch range is exceeded, even if the motion is too old to emit a CONTINUING signal.
    state = GestureState::FINISHED;
  }

  switch(state)
  {
    case GestureState::POSSIBLE:
    {
      mCurrentPanActor.SetActor(GetCurrentGesturedActor());
      mPossiblePanPosition = panEvent.currentPosition;
      break;
    }

    case GestureState::STARTED:
    {
      // The pan gesture should only be sent to the gesture detector which first received it so that
      // it can be told when the gesture ends as well.
      Actor* feededActor = GetCurrentGesturedActor();
      if(feededActor && CheckGestureDetector(&panEvent, feededActor, mRenderTask, mPossiblePanPosition))
      {
        Vector2 actorCoords;
        feededActor->ScreenToLocal(*mRenderTask.Get(), actorCoords.x, actorCoords.y, panEvent.currentPosition.x, panEvent.currentPosition.y);
        if(mCurrentPanActor.GetActor() == feededActor)
        {
          EmitPanSignal(feededActor, panEvent, actorCoords, state, mRenderTask, scene);
        }
        else
        {
          mPossiblePanPosition = panEvent.previousPosition;
          mCurrentPanActor.SetActor(feededActor);
          EmitPanSignal(feededActor, panEvent, actorCoords, state, mRenderTask, scene);
        }
      }
      break;
    }

    case GestureState::CONTINUING:
    {
      if(panEvent.motionEventAge > mActiveProfile.maximumMotionEventAge)
      {
        // Too old for the profile of this gesture: skip this motion, the pan itself stays alive.
        break;
      }

      Actor* currentGesturedActor = mCurrentPanActor.GetActor();
      Actor* feededActor          = GetCurrentGesturedActor();
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
        EmitPanSignal(currentGesturedActor, panEvent, actorCoords, state, mRenderTask, scene);
      }

      if((state == GestureState::FINISHED) || (state == GestureState::CANCELLED))
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

  // A pan is checked here once, when it starts: pick the profile for the device that started it.
  SelectActiveProfile(panEvent->source);

  bool retVal(false);

  if((panEvent->numberOfTouches >= mActiveProfile.minimumTouches) &&
     (panEvent->numberOfTouches <= mActiveProfile.maximumTouches))
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
  Internal::PanGesturePtr pan(new Internal::PanGesture(state));

  pan->SetTime(panEvent.time);

  pan->SetNumberOfTouches(panEvent.numberOfTouches);
  pan->SetScreenPosition(panEvent.currentPosition);
  pan->SetPosition(localCurrent);
  pan->SetTriggerPoint(panEvent.triggerPoint);

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

  // We should not use scene object if Core is shutting down.
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mSceneObject))
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

Actor* PanGestureDetector::GetCurrentGesturedActor()
{
  // Return the current feeded actor
  return GestureDetector::GetCurrentGesturedActor();
}

} // namespace Internal

} //namespace DALI_NAMESPACE
