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
#include <dali/internal/event/actor-attachments/light-attachment-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/node-attachments/scene-graph-light-attachment.h>

namespace Dali
{

namespace Internal
{

LightAttachmentPtr LightAttachment::New( const SceneGraph::Node& parentNode )
{
  StagePtr stage = Stage::GetCurrent();

  LightAttachmentPtr attachment( new LightAttachment( *stage ) );

  // Transfer object ownership of scene-object to message
  SceneGraph::LightAttachment* sceneObject = SceneGraph::LightAttachment::New();
  sceneObject->SetLight( attachment->mCachedLight );
  AttachToNodeMessage( stage->GetUpdateManager(), parentNode, sceneObject );

  // Keep raw pointer for message passing
  attachment->mSceneObject = sceneObject;

  return attachment;
}

LightAttachment::LightAttachment( Stage& stage )
: ActorAttachment( stage )
{
  mCachedLight = new Internal::Light( "Unnamed" );
}

LightAttachment::~LightAttachment()
{
}

void LightAttachment::SetLight( Internal::LightPtr light )
{
  mCachedLight = new Internal::Light(*(light.Get()));

  // sceneObject is being used in a separate thread; queue a message to set
  SetLightMessage( mStage->GetUpdateInterface(), *mSceneObject, mCachedLight );
}

Internal::LightPtr LightAttachment::GetLight() const
{
  return new Internal::Light(*(mCachedLight.Get()));
}

void LightAttachment::SetName( const std::string& name )
{
  if ( mCachedLight->GetName() != name )
  {
    mCachedLight->SetName( name );

    // sceneObject is being used in a separate thread; queue a message to set
    SetNameMessage( mStage->GetUpdateInterface(), *mSceneObject, name );
  }
}

const std::string& LightAttachment::GetName() const
{
  return mCachedLight->GetName();
}

void LightAttachment::SetType( Dali::LightType type )
{
  if ( mCachedLight->GetType() != type )
  {
    mCachedLight->SetType( type );

    // sceneObject is being used in a separate thread; queue a message to set
    SetTypeMessage( mStage->GetUpdateInterface(), *mSceneObject, type );
  }
}

Dali::LightType LightAttachment::GetType() const
{
  return mCachedLight->GetType();
}

void LightAttachment::SetFallOff( const Vector2& fallOff )
{
  if ( mCachedLight->GetFallOff() != fallOff )
  {
    mCachedLight->SetFallOff( fallOff );

    // sceneObject is being used in a separate thread; queue a message to set
    SetFallOffMessage( mStage->GetUpdateInterface(), *mSceneObject, fallOff );
  }
}

const Vector2& LightAttachment::GetFallOff() const
{
  return mCachedLight->GetFallOff();
}

void LightAttachment::SetSpotAngle( const Vector2& angle )
{
  if ( mCachedLight->GetSpotAngle() != angle )
  {
    mCachedLight->SetSpotAngle( angle );

    // sceneObject is being used in a separate thread; queue a message to set
    SetSpotAngleMessage( mStage->GetUpdateInterface(), *mSceneObject, angle );
  }
}

const Vector2& LightAttachment::GetSpotAngle() const
{
  return mCachedLight->GetSpotAngle();
}

void LightAttachment::SetAmbientColor( const Vector3& color )
{
  if ( mCachedLight->GetAmbientColor() != color )
  {
    mCachedLight->SetAmbientColor( color );

    // sceneObject is being used in a separate thread; queue a message to set
    SetAmbientColorMessage( mStage->GetUpdateInterface(), *mSceneObject, color );
  }
}

const Vector3& LightAttachment::GetAmbientColor() const
{
  return mCachedLight->GetAmbientColor();
}

void LightAttachment::SetDiffuseColor( const Vector3& color )
{
  if ( mCachedLight->GetDiffuseColor() != color )
  {
    mCachedLight->SetDiffuseColor( color );

    // sceneObject is being used in a separate thread; queue a message to set
    SetDiffuseColorMessage( mStage->GetUpdateInterface(), *mSceneObject, color );
  }
}

const Vector3& LightAttachment::GetDiffuseColor() const
{
  return mCachedLight->GetDiffuseColor();
}

void LightAttachment::SetSpecularColor( const Vector3& color )
{
  if ( mCachedLight->GetSpecularColor() != color )
  {
    mCachedLight->SetSpecularColor( color );

    // sceneObject is being used in a separate thread; queue a message to set
    SetSpecularColorMessage( mStage->GetUpdateInterface(), *mSceneObject, color );
  }
}

const Vector3& LightAttachment::GetSpecularColor() const
{
  return mCachedLight->GetSpecularColor();
}

void LightAttachment::SetDirection( const Vector3& direction )
{
  if ( mCachedLight->GetDirection() != direction )
  {
    mCachedLight->SetDirection( direction );

    // sceneObject is being used in a separate thread; queue a message to set
    SetDirectionMessage( mStage->GetUpdateInterface(), *mSceneObject, direction );
  }
}

const Vector3& LightAttachment::GetDirection() const
{
  return mCachedLight->GetDirection();
}

void LightAttachment::SetActive( bool active )
{
  // sceneObject is being used in a separate thread; queue a message to set
  SetActiveMessage( mStage->GetUpdateInterface(), *mSceneObject, active );
}

void LightAttachment::OnStageConnection()
{
  // do nothing
}

void LightAttachment::OnStageDisconnection()
{
  // do nothing
}

} // namespace Internal

} // namespace Dali
