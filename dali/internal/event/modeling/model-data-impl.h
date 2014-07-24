#ifndef __DALI_INTERNAL_MODEL_DATA_H__
#define __DALI_INTERNAL_MODEL_DATA_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/light.h>
#include <dali/public-api/modeling/material.h>
#include <dali/public-api/modeling/entity.h>
#include <dali/public-api/modeling/model-data.h>
#include <dali/public-api/modeling/model-animation-map.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/common/owner-container.h>

namespace Dali
{

class Light;
class Entity;

namespace Internal
{

class ModelData;
class ResourceClient;

typedef IntrusivePtr<ModelData> ModelDataPtr;

/**
 * Encapsulates a Dali 3D model/scene.
 */
class ModelData : public BaseObject
{
public:
  /**
   * Constructor
   */
  ModelData();

  /**
   * Constructor
   * @param[in] name The name of the model.
   * @return          None.
   */
  static ModelDataPtr New(const std::string& name);

  /**
   * Sets the model name
   * @param[in] name The model name
   */
  void SetName(const std::string& name);

  /**
   * Returns the name of the model
   * @return the model name
   */
  const std::string& GetName() const;

  /**
   * Set the root Entity of the model.
   * @param root - the root entity
   */
  void SetRootEntity(Dali::Entity root);

  /**
   * Get the root Entity of the model.
   * @return the root entity
   */
  Dali::Entity GetRootEntity() const;

  /**
   * Add a mesh to the model
   * @param mesh - the mesh to add.
   */
  void AddMesh(const Dali::MeshData& mesh);

  /**
   * Add a mesh ticket to the model. ModelFactory should ensure that the index
   * of this ticket is the same as the MeshData it represents.
   * @param[in] ticket   The ticket of the mesh resource.
   */
  void AddMeshTicket(ResourceTicketPtr ticket);

  /**
   * Get a mesh by index.
   * @param[in] index The zero based index to a mesh
   * @return          The MeshData object.
   */
  const Dali::MeshData& GetMesh(unsigned int index) const;

  /**
   * Get a mesh by index.
   * @param[in] index The zero based index to a mesh
   * @return          The MeshData object.
   */
  Dali::MeshData& GetMesh(unsigned int index);

  /**
   * Get a mesh ticket by index
   * @param[in] index The index of a mesh
   * @return    The resource ticket for that mesh
   */
  const ResourceTicketPtr GetMeshTicket(unsigned int index) const;

  /**
   * Get Mesh count
   * @returns number of meshes
   */
  unsigned int NumberOfMeshes() const;

  /**
   * Add material to the model
   * @param[in] material - the material to add to the model
   */
  void AddMaterial(Dali::Material material);

  /**
   * Get a material by index.
   * @param[in] index The zero based index to a material
   * @return          A pointer to a material, or NULL.
   */
  Dali::Material GetMaterial(unsigned int index) const;

  /**
   * Get material count
   * @return number of materials
   */
  unsigned int NumberOfMaterials() const;

  /**
   * Get animation map container.
   * @return reference to the animation map container.
   */
   ModelAnimationMapContainer& GetAnimationMapContainer();

  /**
   * Get the animation for the given index. Returns a null pointer
   * if the index is out of range.
   * @param[in] index The index of the animation
   * @return A pointer to the animation data
   */
  const ModelAnimationMap* GetAnimationMap (unsigned int index) const;

  /**
   * Get the animation for the given name. Returns a null pointer
   * if the name is not found.
   * @param[in] name Name of animation for which to search
   * @return A pointer to the animation data
   */
  const ModelAnimationMap* GetAnimationMap (const std::string& name) const;

  /**
   * Get the duration for a given animation. Returns zero if not found.
   * @param[in] animationIndex - the index of the animation
   * @return the duration in seconds, or zero if not found
   */
  float GetAnimationDuration(size_t animationIndex) const;

  /**
   * Get the index for the given name. Returns false if not found.
   * @param[in] name The name of the animation to find
   * @param[out] index The index of the named animation if found
   * @return true if the animation was foud, false if not found.
   */
  bool FindAnimation (const std::string& name, unsigned int& index) const;

  /**
   * Get the number of animation maps
   * @return the number of animation maps
   */
  unsigned int NumberOfAnimationMaps() const;

  /**
   * Add a light to the model
   * @param light - the light to add to the model
   */
  void AddLight(Dali::Light light);

  /**
   * Get a light by index.
   * @param[in] index The zero based index to a light
   * @return          A pointer to a light, or NULL.
   */
  Dali::Light GetLight(unsigned int index) const;

  /**
   * Get the number of lights contained in the model.
   * @return The number of lights contained in the model.
   */
  unsigned int NumberOfLights() const;

  /**
   * Unpacking is where new resources are created, for the meshes provided in the model data.
   * This should be called after the model data has loaded.
   * @param[in] resourceClient Used to create new resource tickets.
   */
  void Unpack( ResourceClient& resourceClient );

  /**
   * @copydoc Dali::ModelData::Read()
   */
  bool Read(std::streambuf& buf);

  /**
   * @copydoc Dali::ModelData::Write()
   */
  bool Write(std::streambuf& buf) const;

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ModelData();

private:

  std::string                    mName;            ///< 3D model name
  std::vector<Dali::Material>    mMaterials;       ///< Container of Materials
  OwnerContainer< MeshData* >    mMeshes;          ///< Container of Meshes
  std::vector<Dali::Light>       mLights;          ///< Collection of Lights
  ModelAnimationMapContainer     mAnimationMaps;   ///< List of Animation maps
  Dali::Entity                   mRoot;            ///< Root of the entity hierarchy tree

  bool                           mUnpacked;        ///< True if resources have been created, for the meshes provided in the model data.
  std::vector<ResourceTicketPtr> mMeshTickets;     ///< List of Mesh tickets to scene graph objects
}; // class ModelData

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ModelData& GetImplementation(Dali::ModelData& modelData)
{
  DALI_ASSERT_ALWAYS( modelData && "ModelData handle is empty" );

  BaseObject& handle = modelData.GetBaseObject();

  return static_cast<Internal::ModelData&>(handle);
}

inline const Internal::ModelData& GetImplementation(const Dali::ModelData& modelData)
{
  DALI_ASSERT_ALWAYS( modelData && "ModelData handle is empty" );

  const BaseObject& handle = modelData.GetBaseObject();

  return static_cast<const Internal::ModelData&>(handle);
}


} // namespace Dali


#endif // __DALI_INTERNAL_MODEL_DATA_H__
