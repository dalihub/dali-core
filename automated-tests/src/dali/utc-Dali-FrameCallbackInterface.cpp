/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/devel-api/common/stage-devel.h>
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/devel-api/update/update-proxy.h>
#include <dali-test-suite-utils.h>

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

  FrameCallbackBasic()
  : mCalled( false )
  {
  }

  virtual void Update( Dali::UpdateProxy& updateProxy, float elapsedSeconds )
  {
    mCalled = true;
  }

  bool mCalled;
};

} // anon namespace

class FrameCallbackOneActor : public FrameCallbackBasic
{
public:

  FrameCallbackOneActor( unsigned int actorId )
  : mActorId( actorId )
  {
  }

  virtual void Update( Dali::UpdateProxy& updateProxy, float elapsedSeconds )
  {
    FrameCallbackBasic::Update( updateProxy, elapsedSeconds );
    updateProxy.GetWorldMatrixAndSize( mActorId, mWorldMatrix, mSize );
    mWorldMatrixGetWorldMatrixCall = updateProxy.GetWorldMatrix( mActorId );
    mSizeGetSizeCall = updateProxy.GetSize( mActorId );
    mPositionGetPositionCall = updateProxy.GetPosition( mActorId );
    updateProxy.GetPositionAndSize( mActorId, mPositionGetPositionAndSizeCall, mSizeGetPositionAndSizeCall );
    mWorldColor = updateProxy.GetWorldColor( mActorId );
  }

  const unsigned int mActorId;

  Matrix mWorldMatrix;
  Matrix mWorldMatrixGetWorldMatrixCall;
  Vector3 mSize;
  Vector3 mSizeGetSizeCall;
  Vector3 mPositionGetPositionCall;
  Vector3 mPositionGetPositionAndSizeCall;
  Vector3 mSizeGetPositionAndSizeCall;
  Vector4 mWorldColor;
};

class FrameCallbackSetter : public FrameCallbackBasic
{
public:

  FrameCallbackSetter(
      unsigned int actorId,
      const Matrix& matrixToSet,
      const Vector3& sizeToSet,
      const Vector3& positionToSet,
      const Vector4& colorToSet )
  : mActorId( actorId ),
    mMatrixToSet( matrixToSet ),
    mSizeToSet( sizeToSet ),
    mPositionToSet( positionToSet ),
    mColorToSet( colorToSet )
  {
  }

  virtual void Update( Dali::UpdateProxy& updateProxy, float elapsedSeconds )
  {
    FrameCallbackBasic::Update( updateProxy, elapsedSeconds );
    updateProxy.SetWorldMatrix( mActorId, mMatrixToSet );
    updateProxy.SetSize( mActorId, mSizeToSet );
    updateProxy.GetWorldMatrixAndSize( mActorId, mWorldMatrixAfterSetting, mSizeAfterSetting );
    updateProxy.SetPosition( mActorId, mPositionToSet );
    mPositionAfterSetting = updateProxy.GetPosition( mActorId );
    updateProxy.SetWorldColor( mActorId, mColorToSet );
    mColorAfterSetting = updateProxy.GetWorldColor( mActorId );
  }

  const unsigned int mActorId;
  const Matrix& mMatrixToSet;
  const Vector3& mSizeToSet;
  const Vector3& mPositionToSet;
  const Vector4& mColorToSet;

  Matrix mWorldMatrixAfterSetting;
  Vector3 mSizeAfterSetting;
  Vector3 mPositionAfterSetting;
  Vector4 mColorAfterSetting;
};

class FrameCallbackMultipleActors : public FrameCallbackBasic
{
public:

  FrameCallbackMultipleActors()
  {
  }

  virtual void Update( Dali::UpdateProxy& updateProxy, float elapsedSeconds )
  {
    FrameCallbackBasic::Update( updateProxy, elapsedSeconds );
    for( auto&& i : mActorIds )
    {
      Matrix matrix( false );
      Vector3 size;
      updateProxy.GetWorldMatrixAndSize( i, matrix, size );
      mWorldMatrices[ i ] = matrix;
      mSizes[ i ] = size;
    }
  }

  Vector< unsigned int > mActorIds;

  std::map< unsigned int, Matrix > mWorldMatrices;
  std::map< unsigned int, Vector3 > mSizes;
};

///////////////////////////////////////////////////////////////////////////////

int UtcDaliFrameCallbackCheckInstallationAndRemoval(void)
{
  TestApplication application;

  FrameCallbackBasic frameCallback;

  Stage stage = Stage::GetCurrent();
  DevelStage::AddFrameCallback( stage, frameCallback, stage.GetRootLayer() );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( frameCallback.mCalled, true, TEST_LOCATION );

  frameCallback.mCalled = false;

  DevelStage::RemoveFrameCallback( stage, frameCallback );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( frameCallback.mCalled, false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliFrameCallbackGetters(void)
{
  TestApplication application;
  Vector2 actorSize( 200, 300 );
  Vector4 color( 0.5f, 0.6f, 0.7f, 0.8f );
  Vector3 position( 10.0f, 20.0f, 30.0f );

  Actor actor = Actor::New();
  actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actor.SetSize( actorSize );
  actor.SetColor( color );
  actor.SetPosition( position );

  Stage stage = Stage::GetCurrent();
  stage.Add( actor );
  Vector2 stageSize = stage.GetSize();

  FrameCallbackOneActor frameCallback( actor.GetId() );
  DevelStage::AddFrameCallback( stage, frameCallback, stage.GetRootLayer() );

  application.SendNotification();
  application.Render();

  Vector3 expectedPosition( -stageSize.width * 0.5f + actorSize.width * 0.5f + position.x,
                            -stageSize.height * 0.5f + actorSize.height * 0.5f + position.y,
                            0.0f + position.z );

  Matrix expectedWorldMatrix( false );
  expectedWorldMatrix.SetIdentity();
  expectedWorldMatrix.SetTranslation( expectedPosition );

  DALI_TEST_EQUALS( frameCallback.mCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mWorldMatrix, expectedWorldMatrix, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mWorldMatrixGetWorldMatrixCall, expectedWorldMatrix, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mSize, Vector3( actorSize.width, actorSize.height, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mSizeGetSizeCall, Vector3( actorSize.width, actorSize.height, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mPositionGetPositionCall, expectedPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mPositionGetPositionAndSizeCall, expectedPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mSizeGetPositionAndSizeCall, Vector3( actorSize.width, actorSize.height, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mWorldColor, color, TEST_LOCATION );

  END_TEST;
}

int UtcDaliFrameCallbackSetters(void)
{
  TestApplication application;
  Vector2 actorSize( 200, 300 );

  Actor actor = Actor::New();
  actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actor.SetSize( actorSize );

  Stage stage = Stage::GetCurrent();
  stage.Add( actor );
  Vector2 stageSize = stage.GetSize();

  Matrix matrixToSet( Matrix::IDENTITY );
  matrixToSet.SetTranslation( Vector3( 100.0f, 500.0f, 50.0f ) );
  Vector3 sizeToSet( 1.0f, 2.0f, 3.0f );
  Vector3 positionToSet( 10.0f, 20.0f, 30.0f );
  Vector4 colorToSet( Color::MAGENTA );

  FrameCallbackSetter frameCallback( actor.GetId(), matrixToSet, sizeToSet, positionToSet, colorToSet );
  DevelStage::AddFrameCallback( stage, frameCallback, stage.GetRootLayer() );

  application.SendNotification();
  application.Render();

  Matrix expectedWorldMatrix( false );
  expectedWorldMatrix.SetIdentity();
  expectedWorldMatrix.SetTranslation( Vector3( -stageSize.width * 0.5f + actorSize.width * 0.5f,
                                               -stageSize.height * 0.5f + actorSize.height * 0.5f,
                                               0.0f ) );

  DALI_TEST_EQUALS( frameCallback.mCalled, true, TEST_LOCATION );
  DALI_TEST_CHECK( expectedWorldMatrix != matrixToSet );
  DALI_TEST_EQUALS( frameCallback.mWorldMatrixAfterSetting, matrixToSet, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mSizeAfterSetting, sizeToSet, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mPositionAfterSetting, positionToSet, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mColorAfterSetting, colorToSet, TEST_LOCATION );

  END_TEST;
}

int UtcDaliFrameCallbackMultipleActors(void)
{
  /**
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
   *  Screen positions:
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
  Stage stage = Stage::GetCurrent();
  const Vector2 stageSize = stage.GetSize();

  std::map< char, Vector3 > sizes;
  sizes['A'] = Vector3(  50.0f,  50.0f, 0.0f );
  sizes['B'] = Vector3( 100.0f, 100.0f, 0.0f );
  sizes['C'] = Vector3( 150.0f, 150.0f, 0.0f );
  sizes['D'] = Vector3( 200.0f, 200.0f, 0.0f );
  sizes['E'] = Vector3( 250.0f, 250.0f, 0.0f );
  sizes['F'] = Vector3( 300.0f, 300.0f, 0.0f );
  sizes['G'] = Vector3( 350.0f, 350.0f, 0.0f );
  sizes['H'] = Vector3( 400.0f, 350.0f, 0.0f );

  std::map< char, Matrix > matrices;
  for( char i = 'A'; i <= 'H'; ++i )
  {
    matrices[i] = Matrix::IDENTITY;
  }

  matrices['A'].SetTranslation( Vector3( -stageSize.width * 0.5f + sizes['A'].width * 0.5f,
                                         -stageSize.height * 0.5f + sizes['A'].height * 0.5f,
                                         0.0f ) );
  matrices['B'].SetTranslation( Vector3( matrices['A'].GetTranslation3() + sizes['A'] * 0.5f + sizes['B'] * 0.5f ) );
  matrices['C'].SetTranslation( Vector3( matrices['B'].GetTranslation3().x,
                                         matrices['B'].GetTranslation3().y + sizes['B'].height * 0.5f + sizes['C'].height * 0.5f,
                                         0.0f ) );
  matrices['D'].SetTranslation( Vector3( matrices['A'].GetTranslation3().x + sizes['A'].width * 0.5f + sizes['D'].width * 0.5f,
                                         matrices['A'].GetTranslation3().y,
                                         0.0f ) );
  matrices['E'].SetTranslation( Vector3( -stageSize.width * 0.5f + sizes['E'].width * 0.5f,
                                         stageSize.height * 0.5f - sizes['E'].height * 0.5f,
                                         0.0f ) );
  matrices['F'].SetTranslation( Vector3( matrices['E'].GetTranslation3().x,
                                         matrices['E'].GetTranslation3().y - sizes['E'].height * 0.5f - sizes['F'].height * 0.5f,
                                         0.0f ) );
  matrices['G'].SetTranslation( Vector3( matrices['E'].GetTranslation3().x + sizes['E'].width * 0.5f + sizes['G'].width * 0.5f,
                                         matrices['E'].GetTranslation3().y,
                                         0.0f ) );
  matrices['H'].SetTranslation( Vector3( matrices['G'].GetTranslation3().x + sizes['G'].width * 0.5f + sizes['H'].width * 0.5f,
                                         matrices['G'].GetTranslation3().y - sizes['G'].height * 0.5f - sizes['H'].height * 0.5f,
                                         0.0f ) );

  Actor actorA = Actor::New();
  actorA.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actorA.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actorA.SetSize( sizes['A'] );
  stage.Add( actorA );

  Actor actorB = Actor::New();
  actorB.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
  actorB.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actorB.SetSize( sizes['B'] );
  actorA.Add( actorB );

  Actor actorC = Actor::New();
  actorC.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  actorC.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  actorC.SetSize( sizes['C'] );
  actorB.Add( actorC );

  Actor actorD = Actor::New();
  actorD.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
  actorD.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  actorD.SetSize( sizes['D'] );
  actorA.Add( actorD );

  Actor actorE = Actor::New();
  actorE.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
  actorE.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
  actorE.SetSize( sizes['E'] );
  stage.Add( actorE );

  Actor actorF = Actor::New();
  actorF.SetParentOrigin( ParentOrigin::TOP_CENTER );
  actorF.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  actorF.SetSize( sizes['F'] );
  actorE.Add( actorF );

  Actor actorG = Actor::New();
  actorG.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
  actorG.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  actorG.SetSize( sizes['G'] );
  actorE.Add( actorG );

  Actor actorH = Actor::New();
  actorH.SetParentOrigin( ParentOrigin::TOP_RIGHT );
  actorH.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
  actorH.SetSize( sizes['H'] );
  actorG.Add( actorH );

  std::map< char, unsigned int > actorIds;
  actorIds['A'] = actorA.GetId();
  actorIds['B'] = actorB.GetId();
  actorIds['C'] = actorC.GetId();
  actorIds['D'] = actorD.GetId();
  actorIds['E'] = actorE.GetId();
  actorIds['F'] = actorF.GetId();
  actorIds['G'] = actorG.GetId();
  actorIds['H'] = actorH.GetId();

  FrameCallbackMultipleActors frameCallback;
  for( auto&& i : actorIds )
  {
    frameCallback.mActorIds.PushBack( i.second );
  }

  DevelStage::AddFrameCallback( stage, frameCallback, stage.GetRootLayer() );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( frameCallback.mCalled, true, TEST_LOCATION );

  for( char i = 'A'; i <= 'H'; ++i )
  {
    DALI_TEST_EQUALS( frameCallback.mWorldMatrices[ actorIds[ i ] ], matrices[ i ], TEST_LOCATION );
    DALI_TEST_EQUALS( frameCallback.mSizes[ actorIds[ i ] ], sizes[ i ], TEST_LOCATION );
  }

  // Render again to make sure it still gets called and gives the correct values (in case any optimisations break this)
  frameCallback.mCalled = false;

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( frameCallback.mCalled, true, TEST_LOCATION );

  for( char i = 'A'; i <= 'H'; ++i )
  {
    DALI_TEST_EQUALS( frameCallback.mWorldMatrices[ actorIds[ i ] ], matrices[ i ], TEST_LOCATION );
    DALI_TEST_EQUALS( frameCallback.mSizes[ actorIds[ i ] ], sizes[ i ], TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliFrameCallbackCheckActorNotAdded(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actor.SetSize( 200, 300 );

  Stage stage = Stage::GetCurrent();
  FrameCallbackOneActor frameCallback( actor.GetId() );
  DevelStage::AddFrameCallback( stage, frameCallback, stage.GetRootLayer() );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( frameCallback.mCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mWorldMatrix, Matrix(true) /* Unchanged Matrix */, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mWorldMatrixGetWorldMatrixCall, Matrix::IDENTITY, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mSize, Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( frameCallback.mSizeGetSizeCall, Vector3::ZERO, TEST_LOCATION );

  END_TEST;
}
