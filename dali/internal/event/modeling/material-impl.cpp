//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/event/modeling/material-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/internal/update/modeling/scene-graph-material.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>

namespace Dali
{

namespace Internal
{

MaterialProperties::MaterialProperties()
  : mOpacity           (Dali::Material::DEFAULT_OPACITY),
    mShininess         (Dali::Material::DEFAULT_SHININESS),
    mAmbientColor      (Dali::Material::DEFAULT_AMBIENT_COLOR),
    mDiffuseColor      (Dali::Material::DEFAULT_DIFFUSE_COLOR),
    mSpecularColor     (Dali::Material::DEFAULT_SPECULAR_COLOR),
    mEmissiveColor     (Dali::Material::DEFAULT_EMISSIVE_COLOR),
    mMapU              (Dali::Material::DEFAULT_MAPPING_MODE),
    mMapV              (Dali::Material::DEFAULT_MAPPING_MODE),
    mDiffuseUVIndex    (Dali::Material::DEFAULT_DIFFUSE_UV_INDEX),
    mOpacityUVIndex    (Dali::Material::DEFAULT_OPACITY_UV_INDEX),
    mNormalUVIndex     (Dali::Material::DEFAULT_NORMAL_UV_INDEX),
    mHasHeightMap      (Dali::Material::DEFAULT_HAS_HEIGHT_MAP)
{
}

MaterialProperties::MaterialProperties(const MaterialProperties& rhs)
  : mOpacity(rhs.mOpacity),
    mShininess(rhs.mShininess),
    mAmbientColor(rhs.mAmbientColor),
    mDiffuseColor(rhs.mDiffuseColor),
    mSpecularColor(rhs.mSpecularColor),
    mEmissiveColor(rhs.mEmissiveColor),
    mMapU(rhs.mMapU),
    mMapV(rhs.mMapV),
    mDiffuseUVIndex(rhs.mDiffuseUVIndex),
    mOpacityUVIndex(rhs.mOpacityUVIndex),
    mNormalUVIndex(rhs.mNormalUVIndex),
    mHasHeightMap(rhs.mHasHeightMap)
{
}

MaterialProperties& MaterialProperties::operator=(const MaterialProperties& rhs)
{
  mOpacity           = rhs.mOpacity;
  mShininess         = rhs.mShininess;
  mAmbientColor      = rhs.mAmbientColor;
  mDiffuseColor      = rhs.mDiffuseColor;
  mSpecularColor     = rhs.mSpecularColor;
  mEmissiveColor     = rhs.mEmissiveColor;
  mMapU              = rhs.mMapU;
  mMapV              = rhs.mMapV;
  mDiffuseUVIndex    = rhs.mDiffuseUVIndex;
  mOpacityUVIndex    = rhs.mOpacityUVIndex;
  mNormalUVIndex     = rhs.mNormalUVIndex;
  mHasHeightMap      = rhs.mHasHeightMap;

  return *this;
}


Material* Material::New(const std::string& name)
{
  return new Material(name);
}

Material::Material(const std::string& name)
  : mName(name),
    mProperties(),
    mSceneObject(NULL),
    mEventToUpdate(NULL),
    mUpdateManager(NULL),
    mConnectionCount(0)
{
}

Material::~Material()
{
  if ( mSceneObject &&
       Stage::IsInstalled()/*Guard to allow handle destruction after Core has been destroyed*/ )
  {
    RemoveMaterialMessage( *mUpdateManager, mSceneObject );
  }
}

SceneGraph::Material* Material::CreateSceneObject()
{
  DALI_ASSERT_DEBUG( mSceneObject == NULL );

  if(mEventToUpdate == NULL)
  {
    EventToUpdate& eventToUpdate = ThreadLocalStorage::Get().GetEventToUpdate();
    mEventToUpdate = &eventToUpdate;
  }

  if(mUpdateManager == NULL)
  {
    SceneGraph::UpdateManager& updateManager = ThreadLocalStorage::Get().GetUpdateManager();
    mUpdateManager = &updateManager;
  }

  mSceneObject = SceneGraph::Material::New(this);
  if( mDiffuseImage )
  {
    mSceneObject->SetDiffuseTextureId( GetImplementation(mDiffuseImage).GetResourceId() );
  }
  if( mOpacityImage )
  {
    mSceneObject->SetOpacityTextureId( GetImplementation(mOpacityImage).GetResourceId() );
  }
  if( mNormalMap )
  {
    mSceneObject->SetNormalMapId( GetImplementation(mNormalMap).GetResourceId() );
  }
  AddMaterialMessage( *mUpdateManager, mSceneObject );

  return mSceneObject;
}

// Used for staging material
const SceneGraph::Material* Material::GetSceneObject()
{
  if( mSceneObject == NULL )
  {
    mSceneObject = CreateSceneObject();
  }
  return mSceneObject;
}

void Material::Connect()
{
  if(mConnectionCount == 0)
  {
    if( mSceneObject == NULL )
    {
      mSceneObject = CreateSceneObject();
    }

    if (mDiffuseImage)
    {
      GetImplementation(mDiffuseImage).Connect();
      SendDiffuseImageToSceneObject(mDiffuseImage);
    }
    if (mOpacityImage)
    {
      GetImplementation(mOpacityImage).Connect();
      SendOpacityImageToSceneObject(mOpacityImage);
    }
    if (mNormalMap)
    {
      GetImplementation(mNormalMap).Connect();
      SendNormalMapToSceneObject(mNormalMap);
    }
  }
  else
  {
    DALI_ASSERT_DEBUG(mSceneObject);
  }
  mConnectionCount++;
}

void Material::Disconnect()
{
  if(mConnectionCount > 0)
  {
    DALI_ASSERT_DEBUG(mSceneObject);
    mConnectionCount--;
  }

  if(mConnectionCount <= 0)
  {
    if (mDiffuseImage)
    {
      GetImplementation(mDiffuseImage).Disconnect();
    }
    if (mOpacityImage)
    {
      GetImplementation(mOpacityImage).Disconnect();
    }
    if (mNormalMap)
    {
      GetImplementation(mNormalMap).Disconnect();
    }
  }
}

void Material::SendPropertiesToSceneObject()
{
  if( mSceneObject != NULL )
  {
    DALI_ASSERT_DEBUG( mEventToUpdate != NULL );
    if( mEventToUpdate != NULL )
    {
      SetPropertiesMessage( *mEventToUpdate, *mSceneObject, mProperties );
    }
  }
}

void Material::SendDiffuseImageToSceneObject(Dali::Image image)
{
  if( image && mSceneObject != NULL && mEventToUpdate != NULL )
  {
    ResourceId id = GetImplementation(image).GetResourceId();
    SetDiffuseTextureMessage( *mEventToUpdate, *mSceneObject, id );
  }
}

void Material::SendOpacityImageToSceneObject(Dali::Image image)
{
  if( image && mSceneObject != NULL && mEventToUpdate != NULL )
  {
    ResourceId id = GetImplementation(image).GetResourceId();
    SetOpacityTextureMessage( *mEventToUpdate, *mSceneObject, id );
  }
}

void Material::SendNormalMapToSceneObject(Dali::Image image)
{
  if( image && mSceneObject != NULL && mEventToUpdate != NULL )
  {
    ResourceId id = GetImplementation(image).GetResourceId();
    SetNormalMapMessage( *mEventToUpdate, *mSceneObject, id );
  }
}

void Material::SetOpacity(const float opacity)
{
  mProperties.mOpacity = opacity;
  SendPropertiesToSceneObject();
}

float Material::GetOpacity() const
{
  return mProperties.mOpacity;
}

void Material::SetShininess(const float shininess)
{
  mProperties.mShininess = shininess;
  SendPropertiesToSceneObject();
}

float Material::GetShininess() const
{
  return mProperties.mShininess;
}

void Material::SetAmbientColor(const Vector4& color)
{
  mProperties.mAmbientColor = color;
  SendPropertiesToSceneObject();
}

const Vector4& Material::GetAmbientColor() const
{
  return mProperties.mAmbientColor;
}

void Material::SetDiffuseColor(const Vector4& color)
{
  mProperties.mDiffuseColor = color;
  SendPropertiesToSceneObject();
}

const Vector4& Material::GetDiffuseColor() const
{
  return mProperties.mDiffuseColor;
}

void Material::SetSpecularColor(const Vector4& color)
{
  mProperties.mSpecularColor = color;
  SendPropertiesToSceneObject();
}

const Vector4& Material::GetSpecularColor() const
{
  return mProperties.mSpecularColor;
}

void Material::SetEmissiveColor(const Vector4& color)
{
  mProperties.mEmissiveColor = color;
  SendPropertiesToSceneObject();
}

const Vector4& Material::GetEmissiveColor() const
{
  return mProperties.mEmissiveColor;
}

void Material::SetDiffuseTexture(Dali::Image image)
{
  if( mDiffuseImage == image )
  {
    return;
  }

  if( mDiffuseImage && mConnectionCount > 0 )
  {
    GetImplementation(mDiffuseImage).Disconnect();
  }

  mDiffuseImage = image;

  if( image && mConnectionCount > 0 )
  {
    GetImplementation(image).Connect();
  }

  /// @todo currently there is a bug in material rendering if image is not loaded at this point (Trac issue #475)
  SendDiffuseImageToSceneObject(image);
}

void Material::SetDiffuseTextureFileName(const std::string filename)
{
  mDiffuseName = filename;
}

Dali::Image Material::GetDiffuseTexture() const
{
  return mDiffuseImage;
}

const std::string& Material::GetDiffuseTextureFileName() const
{
  return mDiffuseName;
}

void Material::SetOpacityTexture(Dali::Image image)
{
  if (mOpacityImage == image)
  {
    return;
  }

  if (mOpacityImage && mConnectionCount > 0)
  {
    GetImplementation(mOpacityImage).Disconnect();
  }

  mOpacityImage = image;

  if (image && mConnectionCount > 0)
  {
    GetImplementation(image).Connect();
  }

  /// @todo currently there is a bug in material rendering if image is not loaded at this point (Trac issue #475)
  SendOpacityImageToSceneObject(image);
}

void Material::SetOpacityTextureFileName(const std::string filename)
{
  mOpacityName = filename;
}

Dali::Image Material::GetOpacityTexture() const
{
  return mOpacityImage;
}

const std::string& Material::GetOpacityTextureFileName() const
{
  return mOpacityName;
}

void Material::SetNormalMap(Dali::Image image)
{
  if (mNormalMap == image)
  {
    return;
  }

  if (mNormalMap && mConnectionCount > 0)
  {
    GetImplementation(mNormalMap).Disconnect();
  }

  mNormalMap = image;

  if (image && mConnectionCount > 0)
  {
    GetImplementation(image).Connect();
  }

  /// @todo currently there is a bug in material rendering if image is not loaded at this point (Trac issue #475)
  SendNormalMapToSceneObject(image);
}

void Material::SetNormalMapFileName(const std::string filename)
{
  mNormalMapName = filename;
}

Dali::Image Material::GetNormalMap() const
{
  return mNormalMap;
}

const std::string&  Material::GetNormalMapFileName() const
{
  return mNormalMapName;
}

void Material::SetMapU(const unsigned int map)
{
  mProperties.mMapU = map;
  SendPropertiesToSceneObject();
}

const unsigned int Material::GetMapU() const
{
  return mProperties.mMapU;
}

void Material::SetMapV(const unsigned int map)
{
  mProperties.mMapV = map;
  SendPropertiesToSceneObject();
}

const unsigned int Material::GetMapV() const
{
  return mProperties.mMapV;
}

void Material::SetDiffuseUVIndex(const int index)
{
  mProperties.mDiffuseUVIndex = index;
  SendPropertiesToSceneObject();
}

const unsigned int Material::GetDiffuseUVIndex() const
{
  return mProperties.mDiffuseUVIndex;
}

void Material::SetOpacityUVIndex(const int index)
{
  mProperties.mOpacityUVIndex = index;
  SendPropertiesToSceneObject();
}

const unsigned int Material::GetOpacityUVIndex() const
{
  return mProperties.mOpacityUVIndex;
}

void Material::SetNormalUVIndex(const int index)
{
  mProperties.mNormalUVIndex = index;
  SendPropertiesToSceneObject();
}

const unsigned int Material::GetNormalUVIndex() const
{
  return mProperties.mNormalUVIndex;
}

void Material::SetHasHeightMap(const bool flag)
{
  mProperties.mHasHeightMap = true;
  SendPropertiesToSceneObject();
}

const bool Material::GetHasHeightMap() const
{
  return mProperties.mHasHeightMap;
}



} // namespace Internal

} // namespace Dali
