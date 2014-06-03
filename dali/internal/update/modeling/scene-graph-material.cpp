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
#include <dali/internal/update/modeling/scene-graph-material.h>

// INTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-material.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

Material* Material::New()
{
  ResourceManager& resourceManager = ThreadLocalStorage::Get().GetResourceManager();
  return new Material(resourceManager);
}

Material* Material::New(Internal::Material* material)
{
  ResourceManager& resourceManager = ThreadLocalStorage::Get().GetResourceManager();
  return new Material(resourceManager, material);
}

Material::Material(ResourceManager& resourceManager)
  :
    mResourceManager(resourceManager),
    mSceneController(NULL),
    mRenderMaterial(NULL),
    mProperties(),
    mDiffuseResourceId(0),
    mOpacityResourceId(0),
    mNormalMapResourceId(0),
    mDiffuseTextureState(RESOURCE_NOT_USED),
    mOpacityTextureState(RESOURCE_NOT_USED),
    mNormalMapTextureState(RESOURCE_NOT_USED),
    mIsReady(false),
    mIsDirty(false)
{
}

Material::Material(ResourceManager& resourceManager, Internal::Material* material)
  : mResourceManager(resourceManager),
    mSceneController(NULL),
    mRenderMaterial(NULL),
    mProperties(material->GetProperties()),
    mDiffuseResourceId(),
    mOpacityResourceId(0),
    mNormalMapResourceId(0),
    mDiffuseTextureState(RESOURCE_NOT_USED),
    mOpacityTextureState(RESOURCE_NOT_USED),
    mNormalMapTextureState(RESOURCE_NOT_USED),
    mIsReady(false),
    mIsDirty(true)
{
  // Not on stage during construction, so OK to use Dali::Image directly.

  Dali::Image diffuseImage = material->GetDiffuseTexture();
  if (diffuseImage)
  {
    SetDiffuseTextureId(GetImplementation(diffuseImage).GetResourceId() );
  }

  Dali::Image opacityImage = material->GetOpacityTexture();
  if (opacityImage)
  {
    SetOpacityTextureId(GetImplementation(opacityImage).GetResourceId() );
  }

  Dali::Image normalMapImage = material->GetNormalMap();
  if (normalMapImage)
  {
    SetNormalMapId(GetImplementation(normalMapImage).GetResourceId() );
  }
}

Material::~Material()
{
}

void Material::OnStageConnection(SceneController& sceneController)
{
  mSceneController = &sceneController;

  // if we're added to the stage, force the re-sending of texture-id's to the renderer
  // this required if we are removed / then added back to the stage.
  if (mDiffuseTextureState !=  RESOURCE_NOT_USED)
  {
    mDiffuseTextureState = BITMAP_NOT_LOADED;
  }
  if (mOpacityTextureState !=  RESOURCE_NOT_USED)
  {
    mOpacityTextureState = BITMAP_NOT_LOADED;
  }
  if (mNormalMapTextureState !=  RESOURCE_NOT_USED)
  {
    mNormalMapTextureState = BITMAP_NOT_LOADED;
  }
}

void Material::PrepareResources( BufferIndex bufferIndex, ResourceManager& resourceManager )
{
  RenderQueue& renderQueue = mSceneController->GetRenderQueue();

  if( mRenderMaterial != NULL )
  {
    mIsReady = UpdateTextureStates( renderQueue , bufferIndex );
  }
}

void Material::PrepareRender( BufferIndex bufferIndex )
{
  DALI_ASSERT_DEBUG(mSceneController);
  RenderQueue& renderQueue = mSceneController->GetRenderQueue();

  if( mIsDirty && mRenderMaterial != NULL )
  {
    {
      typedef MessageValue1<RenderMaterial, float> DerivedType;

      // Reserve some memory inside the render queue
      unsigned int* slot = renderQueue.ReserveMessageSlot( bufferIndex, sizeof( DerivedType ) );

      // Construct message in the render queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( mRenderMaterial, &RenderMaterial::SetOpacity, mProperties.mOpacity );
    }

    {
      typedef MessageValue1<RenderMaterial, float> DerivedType;

      // Reserve some memory inside the render queue
      unsigned int* slot = renderQueue.ReserveMessageSlot( bufferIndex, sizeof( DerivedType ) );

      // Construct message in the render queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( mRenderMaterial, &RenderMaterial::SetShininess, mProperties.mShininess );
    }

    {
      typedef MessageValue1<RenderMaterial, Vector4> DerivedType;

      // Reserve some memory inside the render queue
      unsigned int* slot = renderQueue.ReserveMessageSlot( bufferIndex, sizeof( DerivedType ) );

      // Construct message in the render queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( mRenderMaterial, &RenderMaterial::SetAmbientColor, mProperties.mAmbientColor );
    }

    {
      typedef MessageValue1<RenderMaterial, Vector4> DerivedType;

      // Reserve some memory inside the render queue
      unsigned int* slot = renderQueue.ReserveMessageSlot( bufferIndex, sizeof( DerivedType ) );

      // Construct message in the render queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( mRenderMaterial, &RenderMaterial::SetDiffuseColor, mProperties.mDiffuseColor );
    }

    {
      typedef MessageValue1<RenderMaterial, Vector4> DerivedType;

      // Reserve some memory inside the render queue
      unsigned int* slot = renderQueue.ReserveMessageSlot( bufferIndex, sizeof( DerivedType ) );

      // Construct message in the render queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( mRenderMaterial, &RenderMaterial::SetSpecularColor, mProperties.mSpecularColor );
    }

    {
      typedef MessageValue1<RenderMaterial, Vector4> DerivedType;

      // Reserve some memory inside the render queue
      unsigned int* slot = renderQueue.ReserveMessageSlot( bufferIndex, sizeof( DerivedType ) );

      // Construct message in the render queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( mRenderMaterial, &RenderMaterial::SetEmissiveColor, mProperties.mEmissiveColor );
    }

    mIsDirty = false;
  }
}

bool Material::AreResourcesReady() const
{
  return mIsReady;
}

void Material::SetDiffuseTextureId(ResourceId id)
{
  mDiffuseResourceId = id;

  // if id == 0, then resource is not used
  mDiffuseTextureState = id ? BITMAP_NOT_LOADED : RESOURCE_NOT_USED;
}

void Material::SetOpacityTextureId(ResourceId id)
{
  mOpacityResourceId = id;

  // if id == 0, then resource is not used
  mOpacityTextureState = id ? BITMAP_NOT_LOADED : RESOURCE_NOT_USED;
}

void Material::SetNormalMapId(ResourceId id)
{
  mNormalMapResourceId = id;

  // if id == 0, then resource is not used
  mNormalMapTextureState  = id ? BITMAP_NOT_LOADED : RESOURCE_NOT_USED;
}

bool Material::HasDiffuseTexture() const
{
  return mDiffuseResourceId != 0;
}

bool Material::HasOpacityTexture() const
{
  return mOpacityResourceId != 0;
}

bool Material::HasNormalMap() const
{
  return mNormalMapResourceId != 0;
}

void Material::SetProperties( const MaterialProperties& properties )
{
  mProperties = properties;
  mIsDirty = true;
}

MaterialProperties Material::GetProperties() const
{
  return mProperties;
}

ResourceId Material::GetDiffuseTextureId() const
{
  return mDiffuseResourceId;
}

ResourceId Material::GetOpacityTextureId() const
{
  return mOpacityResourceId;
}

ResourceId Material::GetNormalMapId() const
{
  return mNormalMapResourceId;
}

bool Material::IsOpaque() const
{
  bool diffuseOpaqueness = true;

  if( mDiffuseResourceId > 0 )
  {
    BitmapMetadata bitmapMetadata = mResourceManager.GetBitmapMetadata( mDiffuseResourceId );
    diffuseOpaqueness = bitmapMetadata.IsFullyOpaque();
  }

  return ( diffuseOpaqueness &&
           GetProperties().mOpacity >= FULLY_OPAQUE &&
           GetProperties().mDiffuseColor.a >= FULLY_OPAQUE );
}

void Material::SetRenderMaterial( RenderMaterial* renderMaterial )
{
  mRenderMaterial = renderMaterial;

  // When a new render material is added to the update material, ensure it's properties are set
  mIsDirty = true;

  // And that the scene object is made unready.
  mIsReady = false;
}

RenderMaterial* Material::GetRenderMaterial() const
{
  return mRenderMaterial;
}

bool Material::ResourceReady(TextureState state)
{
  return (( state == RESOURCE_NOT_USED  ) || (state == BITMAP_LOADED ));
}

bool Material::UpdateTextureStates( RenderQueue& renderQueue, BufferIndex bufferIndex )
{
  // Check if the state of all 3 texture resources

  if( mDiffuseTextureState == BITMAP_NOT_LOADED )
  {
    if( mResourceManager.IsResourceLoaded( mDiffuseResourceId ) )
    {
      mDiffuseTextureState = BITMAP_LOADED;

      typedef MessageValue1<RenderMaterial, unsigned int> DerivedType;

      // Reserve some memory inside the render queue
      unsigned int* slot = renderQueue.ReserveMessageSlot( bufferIndex, sizeof( DerivedType ) );

      // Construct message in the render queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( mRenderMaterial, &RenderMaterial::SetDiffuseTextureId, mDiffuseResourceId );
    }
  }

  if( mOpacityTextureState == BITMAP_NOT_LOADED )
  {
    if( mResourceManager.IsResourceLoaded( mOpacityResourceId ) )
    {
      mOpacityTextureState = BITMAP_LOADED;

      typedef MessageValue1<RenderMaterial, unsigned int> DerivedType;

      // Reserve some memory inside the render queue
      unsigned int* slot = renderQueue.ReserveMessageSlot( bufferIndex, sizeof( DerivedType ) );

      // Construct message in the render queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( mRenderMaterial, &RenderMaterial::SetOpacityTextureId, mOpacityResourceId );
    }
  }

  if( mNormalMapTextureState == BITMAP_NOT_LOADED )
  {
    if( mResourceManager.IsResourceLoaded( mNormalMapResourceId ) )
    {
      mNormalMapTextureState = BITMAP_LOADED;

      typedef MessageValue1<RenderMaterial, unsigned int> DerivedType;

      // Reserve some memory inside the render queue
      unsigned int* slot = renderQueue.ReserveMessageSlot( bufferIndex, sizeof( DerivedType ) );

      // Construct message in the render queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( mRenderMaterial, &RenderMaterial::SetNormalMapTextureId, mNormalMapResourceId );
    }
  }

  // Check if all the resources are ready;
  if( ResourceReady( mDiffuseTextureState) &&
      ResourceReady( mOpacityTextureState) &&
      ResourceReady( mNormalMapTextureState) )
  {
    return true;
  }
  else
  {
    return false;
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
