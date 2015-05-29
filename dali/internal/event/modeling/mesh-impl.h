#ifndef __DALI_INTERNAL_MESH_H__
#define __DALI_INTERNAL_MESH_H__

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
#include <dali/public-api/math/vector4.h>
#include <dali/devel-api/geometry/mesh.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/handle.h>
#include <dali/internal/event/modeling/modeling-declarations.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/resources/resource-ticket-observer.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/update/modeling/scene-graph-mesh-declarations.h>

namespace Dali
{

class Matrix;
class MeshData;

namespace Internal
{

class Mesh;

namespace SceneGraph
{
class Mesh;
}


typedef std::vector<MeshIPtr>              MeshContainer;      ///< Container of Mesh pointers
typedef MeshContainer::iterator            MeshIter;           ///< iterator for Mesh container
typedef MeshContainer::const_iterator      MeshConstIter;      ///< const_iterator for Mesh container

typedef Dali::MeshData::VertexContainer    VertexContainer;
typedef Dali::MeshData::FaceIndex          FaceIndex;
typedef Dali::MeshData::FaceIndices        FaceIndices;


/**
 * A single mesh in a 3D model
 */
class Mesh : public BaseObject, public ResourceTicketObserver
{
public: // construction, destruction and initialisation

  /**
   * Create a new mesh.
   * @param[in] meshData - data provided from public API.
   * @param[in] discardable - If true mesh will be discardable after being uploaded to GL.
   * @param[in] scalingRequired - True if this Mesh should be scaled to fit actor size.
   * @return A smart-pointer to the newly allocated Mesh.
   */
  static MeshIPtr New( const Dali::MeshData& meshData, bool discardable, bool scalingRequired );

protected:

  /**
   * Construct a new Mesh.
   * @param[in] meshData - data provided from public API.
   * @param[in] discardable - If true mesh will be discardable after being uploaded to GL.
   * @param[in] scalingRequired - True if this Mesh should be scaled to fit actor size.
   */
  Mesh( const Dali::MeshData& meshData, bool discardable, bool scalingRequired );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Mesh();

private:

  // Undefined
  Mesh(const Mesh&);

  // Undefined
  Mesh& operator=(const Mesh& rhs);

public:

  /**
   * @copydoc Dali::Mesh::SetData()
   */
  void SetData( const Dali::MeshData& meshData );

  /**
   * If the application changes it's mesh data, ensure the GL vertex data is updated
   */
  void Update();

  /**
   * returns the Id used for lookups
   * @return the unique ID of the resource.
   */
  ResourceId GetResourceId() const;

  /**
   * Get the default material for this mesh.
   * @return the default material for this mesh.
   */
  MaterialIPtr GetMaterial() const;

  /**
   * Get the container of bones associated with this mesh.
   * @return the container of bones for the mesh.
   */
  const BoneContainer& GetBones() const;

  /**
   * @copydoc Dali::Mesh::UpdateMeshData()
   */
  void UpdateMeshData( const Dali::MeshData& meshData );

public: // From ResourceTicketObserver

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceLoadingFailed()
   */
  virtual void ResourceLoadingFailed(const ResourceTicket& ticket);

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceLoadingSucceeded()
   */
  virtual void ResourceLoadingSucceeded(const ResourceTicket& ticket);

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceUploaded()
   */
  virtual void ResourceUploaded(const ResourceTicket& ticket);

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceSavingSucceeded()
   */
  virtual void ResourceSavingSucceeded( const ResourceTicket& ticket );

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceSavingFailed()
   */
  virtual void ResourceSavingFailed( const ResourceTicket& ticket );

protected:

  ResourceTicketPtr mTicket;

  MaterialIPtr mMaterial;       /// Default material for this mesh
  BoneContainer mBoneContainer;   /// List of bones that affect this mesh
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Mesh& GetImplementation(Dali::Mesh& mesh)
{
  DALI_ASSERT_ALWAYS( mesh && "Mesh handle is empty" );

  BaseObject& handle = mesh.GetBaseObject();

  return static_cast<Internal::Mesh&>(handle);
}

inline const Internal::Mesh& GetImplementation(const Dali::Mesh& mesh)
{
  DALI_ASSERT_ALWAYS( mesh && "Mesh handle is empty" );

  const BaseObject& handle = mesh.GetBaseObject();

  return static_cast<const Internal::Mesh&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_MESH_H__
