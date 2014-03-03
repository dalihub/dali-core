#ifndef __DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H__
#define __DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H__

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

// INTERNAL INCLUDES
#include <dali/internal/event/modeling/material-impl.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-to-update.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class RenderMaterial;
class SceneController;

/**
 * This class represents an Internal::Material on the stage by storing ticket id's to each image.
 *
 * TODO: It will eventually inherit from PropertyOwner to manage properties properly.
 */
class Material
{
public:
  /**
   * Create a new scene graph object
   * @return Material the scene graph object for this material
   */
  static Material* New();

  /**
   * Create a new scene graph object from the existing material implementation
   * @return Material the scene graph object for this material
   */
  static Material* New(Internal::Material* material);

  /**
   * Constructor
   * @param[in] resourceManager the resource manager object
   */
  Material(ResourceManager& resourceManager);

  /**
   * Constructor based on existing material implementation
   * @param[in] resourceManager the resource manager object
   * @param[in] material an existing material implementation to copy from
   */
  Material(ResourceManager& resourceManager, Internal::Material* material);

  /**
   * Destructor
   */
  ~Material();

  /**
   * Secondary initialization when added to the stage (UpdateManager)
   * @param[in] sceneController The scene controller
   */
  void OnStageConnection(SceneController& sceneController);

  /**
   * Prepare image resources.
   */
  void PrepareResources( BufferIndex bufferIndex, ResourceManager& resourceManager );

  /**
   * Return true if the images are ready
   * @return TRUE if all resources are ready, false otherwise
   */
  bool AreResourcesReady() const;

  /**
   * Prepare renderMaterial
   */
  void PrepareRender( BufferIndex bufferIndex );

  /**
   * Set the diffuse texture Id
   * @param[in] id the resource id of the diffuse texture
   */
  void SetDiffuseTextureId(Internal::ResourceId id);

  /**
   * Get a resource id of the diffuse texture.
   * @return zero if no diffuse texture set, otherwise valid resource id
   */
  Internal::ResourceId GetDiffuseTextureId() const;

  /**
   * Return true if the material has a valid diffuse texture id
   * @return true if valid, false if never set or set to zero.
   */
  bool HasDiffuseTexture() const;

  /**
   * Set the opacity texture Id
   * @param[in] id the resource id of the opacity texture
   */
  void SetOpacityTextureId(Internal::ResourceId id);

  /**
   * Get a resource id of the opacity texture.
   * @return zero if no opacity texture set, otherwise valid resource id
   */
  Internal::ResourceId GetOpacityTextureId() const;

  /**
   * Return true if the material has a valid opacity texture id
   * @return true if valid, false if never set or set to zero.
   */
  bool HasOpacityTexture() const;

  /**
   * Set the normal map texture Id
   * @param[in] id the resource id of the normal map
   */
  void SetNormalMapId(Internal::ResourceId id);

  /**
   * Get a texture pointer to the normal map or height map
   * @return texturePointer zero if no normal map set, otherwise points at valid texture
   */
  Internal::ResourceId GetNormalMapId() const;

  /**
   * Return true if the material has a valid normal map texture id
   * @return true if valid, false if never set or set to zero.
   */
  bool HasNormalMap() const;

  /**
   * Set the material properties
   * @param[in] properties the value properties to set
   */
  void SetProperties( const MaterialProperties& properties );

  /**
   * Get the material properties
   * @return a copy of the internal properties structure.
   */
  MaterialProperties GetProperties() const;

  /**
   * Query whether the material is opaque
   * @return True if the material is opaque.
   */
  bool IsOpaque() const;

  /**
   * Set the render material. Used for sending messages.
   * @param[in] renderMaterial A pointer to the render material
   */
  void SetRenderMaterial( RenderMaterial* renderMaterial );

  /**
   * Return the currently set render material
   * @return NULL or the render material if connected to stage
   */
  RenderMaterial* GetRenderMaterial() const;

private:

  /**
   * Material texture state
   */
  enum TextureState
  {
    RESOURCE_NOT_USED,            ///< The texture is not used
    BITMAP_NOT_LOADED,            ///< bitmap is not loaded
    BITMAP_LOADED,                ///< bitmap has been loaded
  };

  /**
   * Helper to see if a resource is ready, which
   * means it's either loaded, or not used
   * @param  state texture state
   * @return true if the resource ready
   */
  bool ResourceReady(TextureState state);

  /**
   * Update the status of the texture resources.
   * The material can have up to 3 texture resources.
   * @param[in] renderQueue the render queue
   * @param[in] bufferIndex the update buffer index
   * @return true if all textures that need to be loaded are loaded, false if not
   */
  bool UpdateTextureStates( RenderQueue& renderQueue, BufferIndex bufferIndex );

private:
  ResourceManager& mResourceManager;
  SceneController* mSceneController;
  RenderMaterial*  mRenderMaterial;

  Internal::MaterialProperties mProperties;

  Internal::ResourceId mDiffuseResourceId;
  Internal::ResourceId mOpacityResourceId;
  Internal::ResourceId mNormalMapResourceId;

  TextureState      mDiffuseTextureState;
  TextureState      mOpacityTextureState;
  TextureState      mNormalMapTextureState;

  bool mIsReady:1;
  bool mIsDirty:1;
};

// Messages for Material

inline void SetPropertiesMessage( EventToUpdate& eventToUpdate, const Material& material, const MaterialProperties& properties )
{
  typedef MessageValue1< Material, MaterialProperties > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::SetProperties, properties );
}

inline void SetDiffuseTextureMessage( EventToUpdate& eventToUpdate, const Material& material, ResourceId id )
{
  typedef MessageValue1< Material, ResourceId > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::SetDiffuseTextureId, id );
}

inline void SetOpacityTextureMessage( EventToUpdate& eventToUpdate, const Material& material, ResourceId id )
{
  typedef MessageValue1< Material, ResourceId > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::SetOpacityTextureId, id );
}

inline void SetNormalMapMessage( EventToUpdate& eventToUpdate, const Material& material, ResourceId id )
{
  typedef MessageValue1< Material, ResourceId > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::SetNormalMapId, id );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H__
