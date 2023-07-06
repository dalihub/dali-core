/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/devel-api/common/stage-devel.h>
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/devel-api/update/update-proxy.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_frame_callback_interface_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_frame_callback_interface_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////
namespace
{
class FrameCallbackBasic : public FrameCallbackInterface
{
public:
  FrameCallbackBasic() = default;

  virtual bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds)
  {
    mCalled = true;
    return false;
  }

  virtual void Reset()
  {
    mCalled = false;
  }

  bool mCalled{false};
};

class FrameCallbackOneActor : public FrameCallbackBasic
{
public:
  FrameCallbackOneActor(unsigned int actorId)
  : mActorId(actorId)
  {
  }

  virtual bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds) override
  {
    FrameCallbackBasic::Update(updateProxy, elapsedSeconds);
    updateProxy.GetPosition(mActorId, mPositionGetPositionCall);
    updateProxy.GetPositionAndSize(mActorId, mPositionGetPositionAndSizeCall, mSizeGetPositionAndSizeCall);
    updateProxy.GetSize(mActorId, mSizeGetSizeCall);
    updateProxy.GetOrientation(mActorId, mOrientation);
    updateProxy.GetColor(mActorId, mColor);
    updateProxy.GetScale(mActorId, mScale);

    updateProxy.GetWorldPositionScaleAndSize(mActorId, mWorldPosition, mWorldScale, mSizeGetWorldPositionAndSizeCall);
    updateProxy.GetWorldTransformAndSize(mActorId, mWorldTransformPosition, mWorldTransformScale, mWorldTransformOrientation, mSizeGetWorldTransform);

    return false;
  }

  const unsigned int mActorId;

  Vector3    mPositionGetPositionCall;
  Vector3    mPositionGetPositionAndSizeCall;
  Vector3    mSizeGetSizeCall;
  Vector3    mSizeGetPositionAndSizeCall;
  Vector4    mColor;
  Vector3    mScale;
  Quaternion mOrientation;

  Vector3 mWorldPosition;
  Vector3 mWorldScale;
  Vector3 mSizeGetWorldPositionAndSizeCall;

  Vector3    mWorldTransformPosition;
  Vector3    mWorldTransformScale;
  Quaternion mWorldTransformOrientation;
  Vector3    mSizeGetWorldTransform;
};

class FrameCallbackSetter : public FrameCallbackBasic
{
public:
  FrameCallbackSetter(
    unsigned int      actorId,
    const Vector3&    sizeToSet,
    const Vector3&    positionToSet,
    const Vector4&    colorToSet,
    const Vector3&    scaleToSet,
    const Quaternion& orientationToSet)
  : mActorId(actorId),
    mSizeToSet(sizeToSet),
    mPositionToSet(positionToSet),
    mColorToSet(colorToSet),
    mScaleToSet(scaleToSet),
    mOrientationToSet(orientationToSet)
  {
  }

  virtual bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds) override
  {
    Vector3 size;
    FrameCallbackBasic::Update(updateProxy, elapsedSeconds);
    updateProxy.SetSize(mActorId, mSizeToSet);
    updateProxy.SetPosition(mActorId, mPositionToSet);
    updateProxy.SetOrientation(mActorId, mOrientationToSet);
    updateProxy.SetColor(mActorId, mColorToSet);
    updateProxy.SetScale(mActorId, mScaleToSet);
    updateProxy.GetSize(mActorId, mSizeAfterSetting);
    updateProxy.GetPosition(mActorId, mPositionAfterSetting);
    updateProxy.GetColor(mActorId, mColorAfterSetting);
    updateProxy.GetScale(mActorId, mScaleAfterSetting);
    updateProxy.GetOrientation(mActorId, mOrientationAfterSetting);

    return false;
  }

  const unsigned int mActorId;
  const Vector3&     mSizeToSet;
  const Vector3&     mPositionToSet;
  const Vector4&     mColorToSet;
  const Vector3&     mScaleToSet;
  const Quaternion&  mOrientationToSet;

  Vector3    mSizeAfterSetting;
  Vector3    mPositionAfterSetting;
  Vector4    mColorAfterSetting;
  Vector3    mScaleAfterSetting;
  Quaternion mOrientationAfterSetting;
};

class FrameCallbackBaker : public FrameCallbackBasic
{
public:
  FrameCallbackBaker(
    unsigned int      actorId,
    const Vector3&    sizeToSet,
    const Vector3&    positionToSet,
    const Vector4&    colorToSet,
    const Vector3&    scaleToSet,
    const Quaternion& orientationToSet)
  : mActorId(actorId),
    mSizeToSet(sizeToSet),
    mPositionToSet(positionToSet),
    mColorToSet(colorToSet),
    mScaleToSet(scaleToSet),
    mOrientationToSet(orientationToSet)
  {
  }

  virtual bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds) override
  {
    Vector3 size;
    FrameCallbackBasic::Update(updateProxy, elapsedSeconds);
    updateProxy.BakeSize(mActorId, mSizeToSet);
    updateProxy.BakePosition(mActorId, mPositionToSet);
    updateProxy.BakeColor(mActorId, mColorToSet);
    updateProxy.BakeScale(mActorId, mScaleToSet);
    updateProxy.BakeOrientation(mActorId, mOrientationToSet);
    updateProxy.GetSize(mActorId, mSizeAfterSetting);
    updateProxy.GetPosition(mActorId, mPositionAfterSetting);
    updateProxy.GetColor(mActorId, mColorAfterSetting);
    updateProxy.GetScale(mActorId, mScaleAfterSetting);
    updateProxy.GetOrientation(mActorId, mOrientationAfterSetting);

    return false;
  }

  const unsigned int mActorId;
  const Vector3&     mSizeToSet;
  const Vector3&     mPositionToSet;
  const Vector4&     mColorToSet;
  const Vector3&     mScaleToSet;
  const Quaternion&  mOrientationToSet;

  Vector3    mSizeAfterSetting;
  Vector3    mPositionAfterSetting;
  Vector4    mColorAfterSetting;
  Vector3    mScaleAfterSetting;
  Quaternion mOrientationAfterSetting;
};

class FrameCallbackMultipleActors : public FrameCallbackBasic
{
public:
  FrameCallbackMultipleActors()
  {
  }

  virtual bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds) override
  {
    FrameCallbackBasic::Update(updateProxy, elapsedSeconds);
    for(auto&& i : mActorIds)
    {
      Vector3 position;
      Vector3 size;
      updateProxy.GetPositionAndSize(i, position, size);
      mPositions[i] = position;
      mSizes[i]     = size;
    }

    return false;
  }

  Vector<unsigned int> mActorIds;

  std::map<unsigned int, Vector3> mPositions;
  std::map<unsigned int, Vector3> mSizes;
};

class FrameCallbackActorIdCheck : public FrameCallbackBasic
{
public:
  FrameCallbackActorIdCheck(unsigned int actorId)
  : mActorId(actorId)
  {
  }

  virtual bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds) override
  {
    FrameCallbackBasic::Update(updateProxy, elapsedSeconds);
    Vector3    vec3;
    Vector4    vec4;
    Quaternion quat;

    mGetSizeCallSuccess                      = updateProxy.GetSize(mActorId, vec3);
    mGetPositionCallSuccess                  = updateProxy.GetPosition(mActorId, vec3);
    mGetColorCallSuccess                     = updateProxy.GetColor(mActorId, vec4);
    mGetScaleCallSuccess                     = updateProxy.GetScale(mActorId, vec3);
    mGetPositionAndSizeCallSuccess           = updateProxy.GetPositionAndSize(mActorId, vec3, vec3);
    mGetWorldPositionScaleAndSizeCallSuccess = updateProxy.GetWorldPositionScaleAndSize(mActorId, vec3, vec3, vec3);
    mSetSizeCallSuccess                      = updateProxy.SetSize(mActorId, vec3);
    mSetPositionCallSuccess                  = updateProxy.SetPosition(mActorId, vec3);
    mSetColorCallSuccess                     = updateProxy.SetColor(mActorId, vec4);
    mSetScaleCallSuccess                     = updateProxy.SetScale(mActorId, vec3);
    mBakeSizeCallSuccess                     = updateProxy.BakeSize(mActorId, vec3);
    mBakePositionCallSuccess                 = updateProxy.BakePosition(mActorId, vec3);
    mBakeColorCallSuccess                    = updateProxy.BakeColor(mActorId, vec4);
    mBakeScaleCallSuccess                    = updateProxy.BakeScale(mActorId, vec3);

    mGetOrientationCallSuccess    = updateProxy.GetOrientation(mActorId, quat);
    mSetOrientationCallSuccess    = updateProxy.SetOrientation(mActorId, quat);
    mBakeOrientationCallSuccess   = updateProxy.BakeOrientation(mActorId, quat);
    mGetWorldTransformCallSuccess = updateProxy.GetWorldTransformAndSize(mActorId, vec3, vec3, quat, vec3);

    return false;
  }

  virtual void Reset() override
  {
    // Up-call
    FrameCallbackBasic::Reset();

    mGetSizeCallSuccess                      = false;
    mGetPositionCallSuccess                  = false;
    mGetColorCallSuccess                     = false;
    mGetScaleCallSuccess                     = false;
    mGetPositionAndSizeCallSuccess           = false;
    mGetWorldPositionScaleAndSizeCallSuccess = false;
    mSetSizeCallSuccess                      = false;
    mSetPositionCallSuccess                  = false;
    mSetColorCallSuccess                     = false;
    mSetScaleCallSuccess                     = false;
    mBakeSizeCallSuccess                     = false;
    mBakePositionCallSuccess                 = false;
    mBakeColorCallSuccess                    = false;
    mBakeScaleCallSuccess                    = false;

    mSetOrientationCallSuccess  = false;
    mGetOrientationCallSuccess  = false;
    mBakeOrientationCallSuccess = false;

    mGetWorldTransformCallSuccess = false;
  }

  const uint32_t mActorId;

  bool mGetSizeCallSuccess{false};
  bool mGetPositionCallSuccess{false};
  bool mGetOrientationCallSuccess{false};
  bool mGetColorCallSuccess{false};
  bool mGetScaleCallSuccess{false};
  bool mGetPositionAndSizeCallSuccess{false};
  bool mGetWorldPositionScaleAndSizeCallSuccess{false};
  bool mSetSizeCallSuccess{false};
  bool mSetPositionCallSuccess{false};
  bool mSetColorCallSuccess{false};
  bool mSetScaleCallSuccess{false};
  bool mSetOrientationCallSuccess{false};
  bool mBakeSizeCallSuccess{false};
  bool mBakePositionCallSuccess{false};
  bool mBakeColorCallSuccess{false};
  bool mBakeScaleCallSuccess{false};
  bool mBakeOrientationCallSuccess{false};
  bool mGetWorldTransformCallSuccess{false};
};

} // namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliFrameCallbackCheckInstallationAndRemoval(void)
{
  // Basic test to check that the frame-callback can be installed and removed correctly

  TestApplication application;

  FrameCallbackBasic frameCallback;

  Stage stage = Stage::GetCurrent();
  DevelStage::AddFrameCallback(stage, frameCallback, stage.GetRootLayer());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);

  frameCallback.mCalled = false;

  DevelStage::RemoveFrameCallback(stage, frameCallback);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameCallbackGetters(void)
{
  // Test to see that the Getters all return the expected values

  TestApplication application;

  Vector2    actorSize(200, 300);
  Vector4    color(0.5f, 0.6f, 0.7f, 0.8f);
  Vector3    position(10.0f, 20.0f, 30.0f);
  Vector3    scale(2.0f, 4.0f, 6.0f);
  Quaternion orientation; //(Radian(Math::PI_2), Vector3::ZAXIS);
  Actor      actor = Actor::New();
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::SIZE, actorSize);
  actor.SetProperty(Actor::Property::COLOR, color);
  actor.SetProperty(Actor::Property::POSITION, position);
  actor.SetProperty(Actor::Property::SCALE, scale);
  actor.SetProperty(Actor::Property::ORIENTATION, orientation);

  Stage stage = Stage::GetCurrent();
  stage.Add(actor);

  FrameCallbackOneActor frameCallback(actor.GetProperty<int>(Actor::Property::ID));
  DevelStage::AddFrameCallback(stage, frameCallback, stage.GetRootLayer());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSizeGetSizeCall, Vector3(actorSize.width, actorSize.height, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mPositionGetPositionCall, position, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mPositionGetPositionAndSizeCall, position, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSizeGetPositionAndSizeCall, Vector3(actorSize.width, actorSize.height, 0.0f), TEST_LOCATION);

  DALI_TEST_EQUALS(frameCallback.mColor, color, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mScale, scale, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mOrientation, orientation, TEST_LOCATION);

  frameCallback.Reset();

  application.SendNotification();
  application.Render();

  Vector3    worldPosition, worldScale;
  Quaternion worldRotation;
  Matrix     worldTransform = DevelActor::GetWorldTransform(actor);
  worldTransform.GetTransformComponents(worldPosition, worldRotation, worldScale);

  // World position and scale values are updated after FrameCallbackInterface::Update()
  // So test them after the second rendering
  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);

  DALI_TEST_EQUALS(frameCallback.mSizeGetWorldTransform, Vector3(actorSize.width, actorSize.height, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mWorldPosition, worldPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mWorldScale, worldScale, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSizeGetWorldPositionAndSizeCall, Vector3(actorSize), TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mWorldTransformPosition, worldPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mWorldTransformScale, worldScale, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mWorldTransformOrientation, worldRotation, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameCallbackSetters(void)
{
  // Test to see that the setters set the values appropriately

  TestApplication application;
  Vector2         actorSize(200, 300);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::SIZE, actorSize);

  Stage stage = Stage::GetCurrent();
  stage.Add(actor);

  Vector3    sizeToSet(1.0f, 2.0f, 3.0f);
  Vector3    positionToSet(10.0f, 20.0f, 30.0f);
  Vector4    colorToSet(Color::MAGENTA);
  Vector3    scaleToSet(1.0f, 3.0f, 5.0f);
  Quaternion orientationToSet(Radian(Math::PI_2), Vector3::ZAXIS);

  FrameCallbackSetter frameCallback(actor.GetProperty<int>(Actor::Property::ID), sizeToSet, positionToSet, colorToSet, scaleToSet, orientationToSet);
  DevelStage::AddFrameCallback(stage, frameCallback, stage.GetRootLayer());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSizeAfterSetting, sizeToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mPositionAfterSetting, positionToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mColorAfterSetting, colorToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mScaleAfterSetting, scaleToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mOrientationAfterSetting, orientationToSet, TEST_LOCATION);

  // Ensure the actual actor values haven't changed as we didn't bake the values after removing the callback
  DevelStage::RemoveFrameCallback(stage, frameCallback);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::POSITION).Get<Vector3>(), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>(), Vector3(actorSize), TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::COLOR).Get<Vector4>(), Color::WHITE, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::SCALE).Get<Vector3>(), Vector3::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::ORIENTATION).Get<Quaternion>(), Quaternion(Radian(0.0f), Vector3::ZAXIS), TEST_LOCATION);

  // Render for a couple more frames to ensure the values are reset properly (some values are double-buffered)

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::POSITION).Get<Vector3>(), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>(), Vector3(actorSize), TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::COLOR).Get<Vector4>(), Color::WHITE, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::SCALE).Get<Vector3>(), Vector3::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::ORIENTATION).Get<Quaternion>(), Quaternion(Radian(0.0f), Vector3::ZAXIS), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::POSITION).Get<Vector3>(), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>(), Vector3(actorSize), TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::COLOR).Get<Vector4>(), Color::WHITE, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::SCALE).Get<Vector3>(), Vector3::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::ORIENTATION).Get<Quaternion>(), Quaternion(Radian(0.0f), Vector3::ZAXIS), TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameCallbackBake(void)
{
  // Test to see that the bake methods bake the values

  TestApplication application;
  Vector2         actorSize(200, 300);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::SIZE, actorSize);

  Stage stage = Stage::GetCurrent();
  stage.Add(actor);

  Vector3    sizeToSet(1.0f, 2.0f, 3.0f);
  Vector3    positionToSet(10.0f, 20.0f, 30.0f);
  Vector4    colorToSet(Color::MAGENTA);
  Vector3    scaleToSet(1.0f, 3.0f, 5.0f);
  Quaternion orientationToSet(Radian(Math::PI * 0.3), Vector3::YAXIS);

  FrameCallbackBaker frameCallback(actor.GetProperty<int>(Actor::Property::ID), sizeToSet, positionToSet, colorToSet, scaleToSet, orientationToSet);
  DevelStage::AddFrameCallback(stage, frameCallback, stage.GetRootLayer());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSizeAfterSetting, sizeToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mPositionAfterSetting, positionToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mColorAfterSetting, colorToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mScaleAfterSetting, scaleToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mOrientationAfterSetting, orientationToSet, TEST_LOCATION);

  // Ensure the new values are saved after removing the callback
  DevelStage::RemoveFrameCallback(stage, frameCallback);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::POSITION).Get<Vector3>(), positionToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>(), sizeToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::COLOR).Get<Vector4>(), colorToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::SCALE).Get<Vector3>(), scaleToSet, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty(Actor::Property::ORIENTATION).Get<Quaternion>(), orientationToSet, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameCallbackMultipleActors(void)
{
  /**
   * Test to check that the frame callback behaves appropriately with multiple actors
   *
   * Tree:
   *              root-layer
   *              /        \
   *             /          \
   *            /            \
   *           /              \
   *        actorA           actorE
   *         / \              / \
   *        /   \            /   \
   *    actorB  actorD   actorF actorG
   *      /                        \
   *   actorC                     actorH
   *
   *  Screen positions (with minor alterations due to local position):
   *  -----------------------
   *  |actorA|actorD        |
   *  |      actorB         |
   *  |      actorC         |
   *  |                     |
   *  |                     |
   *  |                     |
   *  |                     |
   *  |                     |
   *  |actorF       actorH  |
   *  |actorE|actorG        |
   *  -----------------------
   */

  TestApplication application;
  Stage           stage = Stage::GetCurrent();

  std::map<char, Vector3> sizes;
  sizes['A'] = Vector3(50.0f, 50.0f, 0.0f);
  sizes['B'] = Vector3(100.0f, 100.0f, 0.0f);
  sizes['C'] = Vector3(150.0f, 150.0f, 0.0f);
  sizes['D'] = Vector3(200.0f, 200.0f, 0.0f);
  sizes['E'] = Vector3(250.0f, 250.0f, 0.0f);
  sizes['F'] = Vector3(300.0f, 300.0f, 0.0f);
  sizes['G'] = Vector3(350.0f, 350.0f, 0.0f);
  sizes['H'] = Vector3(400.0f, 350.0f, 0.0f);

  std::map<char, Vector3> positions;
  positions['A'] = Vector3(0.0f, 1.0f, 2.0f);
  positions['B'] = Vector3(2.0f, 3.0f, 4.0f);
  positions['C'] = Vector3(5.0f, 6.0f, 7.0f);
  positions['D'] = Vector3(8.0f, 9.0f, 10.0f);
  positions['E'] = Vector3(11.0f, 12.0f, 13.0f);
  positions['F'] = Vector3(14.0f, 15.0f, 16.0f);
  positions['G'] = Vector3(17.0f, 18.0f, 19.0f);
  positions['H'] = Vector3(20.0f, 21.0f, 22.0f);

  Actor actorA = Actor::New();
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actorA.SetProperty(Actor::Property::SIZE, sizes['A']);
  actorA.SetProperty(Actor::Property::POSITION, positions['A']);
  stage.Add(actorA);

  Actor actorB = Actor::New();
  actorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_RIGHT);
  actorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actorB.SetProperty(Actor::Property::SIZE, sizes['B']);
  actorB.SetProperty(Actor::Property::POSITION, positions['B']);
  actorA.Add(actorB);

  Actor actorC = Actor::New();
  actorC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
  actorC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
  actorC.SetProperty(Actor::Property::SIZE, sizes['C']);
  actorC.SetProperty(Actor::Property::POSITION, positions['C']);
  actorB.Add(actorC);

  Actor actorD = Actor::New();
  actorD.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_RIGHT);
  actorD.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
  actorD.SetProperty(Actor::Property::SIZE, sizes['D']);
  actorD.SetProperty(Actor::Property::POSITION, positions['D']);
  actorA.Add(actorD);

  Actor actorE = Actor::New();
  actorE.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
  actorE.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT);
  actorE.SetProperty(Actor::Property::SIZE, sizes['E']);
  actorE.SetProperty(Actor::Property::POSITION, positions['E']);
  stage.Add(actorE);

  Actor actorF = Actor::New();
  actorF.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
  actorF.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
  actorF.SetProperty(Actor::Property::SIZE, sizes['F']);
  actorF.SetProperty(Actor::Property::POSITION, positions['F']);
  actorE.Add(actorF);

  Actor actorG = Actor::New();
  actorG.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_RIGHT);
  actorG.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
  actorG.SetProperty(Actor::Property::SIZE, sizes['G']);
  actorG.SetProperty(Actor::Property::POSITION, positions['G']);
  actorE.Add(actorG);

  Actor actorH = Actor::New();
  actorH.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_RIGHT);
  actorH.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT);
  actorH.SetProperty(Actor::Property::SIZE, sizes['H']);
  actorH.SetProperty(Actor::Property::POSITION, positions['H']);
  actorG.Add(actorH);

  std::map<char, unsigned int> actorIds;
  actorIds['A'] = actorA.GetProperty<int>(Actor::Property::ID);
  actorIds['B'] = actorB.GetProperty<int>(Actor::Property::ID);
  actorIds['C'] = actorC.GetProperty<int>(Actor::Property::ID);
  actorIds['D'] = actorD.GetProperty<int>(Actor::Property::ID);
  actorIds['E'] = actorE.GetProperty<int>(Actor::Property::ID);
  actorIds['F'] = actorF.GetProperty<int>(Actor::Property::ID);
  actorIds['G'] = actorG.GetProperty<int>(Actor::Property::ID);
  actorIds['H'] = actorH.GetProperty<int>(Actor::Property::ID);

  FrameCallbackMultipleActors frameCallback;
  for(auto&& i : actorIds)
  {
    frameCallback.mActorIds.PushBack(i.second);
  }

  DevelStage::AddFrameCallback(stage, frameCallback, stage.GetRootLayer());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);

  for(char i = 'A'; i <= 'H'; ++i)
  {
    DALI_TEST_EQUALS(frameCallback.mPositions[actorIds[i]], positions[i], TEST_LOCATION);
    DALI_TEST_EQUALS(frameCallback.mSizes[actorIds[i]], sizes[i], TEST_LOCATION);
  }

  // Render again to make sure it still gets called and gives the correct values (in case any optimisations break this)
  frameCallback.mCalled = false;

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);

  for(char i = 'A'; i <= 'H'; ++i)
  {
    DALI_TEST_EQUALS(frameCallback.mPositions[actorIds[i]], positions[i], TEST_LOCATION);
    DALI_TEST_EQUALS(frameCallback.mSizes[actorIds[i]], sizes[i], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliFrameCallbackCheckActorNotAdded(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::SIZE, Vector2(200, 300));

  Stage                 stage = Stage::GetCurrent();
  FrameCallbackOneActor frameCallback(actor.GetProperty<int>(Actor::Property::ID));
  DevelStage::AddFrameCallback(stage, frameCallback, stage.GetRootLayer());

  application.SendNotification();
  application.Render();

  // All should be default constructed objects
  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mPositionGetPositionCall, Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mWorldPosition, Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSizeGetSizeCall, Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mColor, Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mScale, Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mWorldScale, Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mOrientation, Quaternion::IDENTITY, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameCallbackInvalidActorId(void)
{
  // Test to ensure that there are no issues when trying to use the update-proxy methods with an invalid actor ID.

  TestApplication application;
  Stage           stage = Stage::GetCurrent();

  FrameCallbackActorIdCheck frameCallback(10000);
  DevelStage::AddFrameCallback(stage, frameCallback, stage.GetRootLayer());

  application.SendNotification();
  application.Render();

  // Invalid Actor ID so all the methods should not return successfully.

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetSizeCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetPositionCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetColorCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetScaleCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetPositionAndSizeCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetWorldPositionScaleAndSizeCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetSizeCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetPositionCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetColorCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetScaleCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeSizeCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakePositionCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeColorCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeScaleCallSuccess, false, TEST_LOCATION);

  DALI_TEST_EQUALS(frameCallback.mGetOrientationCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetOrientationCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeOrientationCallSuccess, false, TEST_LOCATION);

  DALI_TEST_EQUALS(frameCallback.mGetWorldTransformCallSuccess, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameCallbackActorRemovedAndAdded(void)
{
  // Test to ensure that we do not call methods on actors that have been removed on the stage
  // and then re-start calling the required methods if that actor is re-added back to the stage

  TestApplication application;
  Stage           stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  stage.Add(actor);

  FrameCallbackActorIdCheck frameCallback(actor.GetProperty<int>(Actor::Property::ID));
  DevelStage::AddFrameCallback(stage, frameCallback, stage.GetRootLayer());

  application.SendNotification();
  application.Render();

  // All methods should return successfully.

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetSizeCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetPositionCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetColorCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetScaleCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetPositionAndSizeCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetWorldPositionScaleAndSizeCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetSizeCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetPositionCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetColorCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetScaleCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeSizeCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakePositionCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeColorCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeScaleCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetOrientationCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetOrientationCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeOrientationCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetWorldTransformCallSuccess, true, TEST_LOCATION);

  frameCallback.Reset();

  // Remove the actor from stage, the methods should not return successfully.

  stage.Remove(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetSizeCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetPositionCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetColorCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetScaleCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetPositionAndSizeCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetWorldPositionScaleAndSizeCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetSizeCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetPositionCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetColorCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetScaleCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeSizeCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakePositionCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeColorCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeScaleCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetOrientationCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetOrientationCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeOrientationCallSuccess, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetWorldTransformCallSuccess, false, TEST_LOCATION);

  frameCallback.Reset();

  // Re-add the actor back to the stage, all the methods should once again, return successfully.

  stage.Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetSizeCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetPositionCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetColorCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetScaleCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetPositionAndSizeCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetWorldPositionScaleAndSizeCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetSizeCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetPositionCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetColorCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetScaleCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeSizeCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakePositionCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeColorCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeScaleCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetOrientationCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSetOrientationCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mBakeOrientationCallSuccess, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mGetWorldTransformCallSuccess, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameCallbackMultipleCallbacks(void)
{
  // Test to ensure multiple frame-callbacks work as expected

  TestApplication application;
  Stage           stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  stage.Add(actor);

  FrameCallbackBasic frameCallback1;
  FrameCallbackBasic frameCallback2;
  DevelStage::AddFrameCallback(stage, frameCallback1, stage.GetRootLayer());
  DevelStage::AddFrameCallback(stage, frameCallback2, stage.GetRootLayer());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback1.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback2.mCalled, true, TEST_LOCATION);
  frameCallback1.Reset();
  frameCallback2.Reset();

  // Remove the second frame-callback, only the first should be called

  DevelStage::RemoveFrameCallback(stage, frameCallback2);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback1.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback2.mCalled, false, TEST_LOCATION);
  frameCallback1.Reset();
  frameCallback2.Reset();

  // Re-add the second frame-callback and remove the first, only the second should be called

  DevelStage::AddFrameCallback(stage, frameCallback2, stage.GetRootLayer());
  DevelStage::RemoveFrameCallback(stage, frameCallback1);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback1.mCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback2.mCalled, true, TEST_LOCATION);
  frameCallback1.Reset();
  frameCallback2.Reset();

  // Attempt removal of the first frame-callback again, should be a no-op and yield the exact same results as the last run
  DevelStage::RemoveFrameCallback(stage, frameCallback1);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback1.mCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback2.mCalled, true, TEST_LOCATION);
  frameCallback1.Reset();
  frameCallback2.Reset();

  // Remove the second frame-callback as well, neither should be called
  DevelStage::RemoveFrameCallback(stage, frameCallback2);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback1.mCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback2.mCalled, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameCallbackActorDestroyed(void)
{
  // Test to ensure that the frame-callback behaves gracefully if the connected root-actor is destroyed

  TestApplication application;
  Stage           stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  stage.Add(actor);

  FrameCallbackBasic frameCallback1;
  FrameCallbackBasic frameCallback2;
  DevelStage::AddFrameCallback(stage, frameCallback1, actor);
  DevelStage::AddFrameCallback(stage, frameCallback2, actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback1.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback2.mCalled, true, TEST_LOCATION);
  frameCallback1.Reset();
  frameCallback2.Reset();

  // Remove the second frame-callback, only the first should be called

  DevelStage::RemoveFrameCallback(stage, frameCallback2);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback1.mCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback2.mCalled, false, TEST_LOCATION);
  frameCallback1.Reset();
  frameCallback2.Reset();

  // Remove and destroy the actor, the first one should not be called either
  stage.Remove(actor);
  actor.Reset();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback1.mCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback2.mCalled, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameCallbackDestroyedBeforeRemoving(void)
{
  // Ensure there's no segmentation fault if the callback is deleted without being removed

  TestApplication application;
  Stage           stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  stage.Add(actor);

  {
    FrameCallbackBasic frameCallback;
    DevelStage::AddFrameCallback(stage, frameCallback, actor);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);
    frameCallback.Reset();
  }

  // frameCallback has now been destroyed but not removed

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(true); // If it runs to here then there's no segmentation fault

  END_TEST;
}

int UtcDaliFrameCallbackDoubleAddition(void)
{
  // Ensure we don't connect the same frame-callback twice

  TestApplication application;
  Stage           stage     = Stage::GetCurrent();
  Actor           rootActor = stage.GetRootLayer();

  FrameCallbackBasic frameCallback;
  DevelStage::AddFrameCallback(stage, frameCallback, rootActor);

  try
  {
    DevelStage::AddFrameCallback(stage, frameCallback, rootActor);
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}

int UtcDaliFrameCallbackGetExtension(void)
{
  FrameCallbackBasic frameCallback;
  DALI_TEST_CHECK(frameCallback.GetExtension() == nullptr);

  END_TEST;
}
