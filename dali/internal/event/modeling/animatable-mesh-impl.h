#ifndef __DALI_INTERNAL_ANIMATABLE_MESH_EFFECT_H__
#define __DALI_INTERNAL_ANIMATABLE_MESH_EFFECT_H__

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
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/geometry/animatable-mesh.h>
#include <dali/internal/event/common/proxy-object.h>
#include <dali/internal/event/modeling/modeling-declarations.h>

namespace Dali
{
namespace Internal
{
class AnimatableMesh;
typedef IntrusivePtr<AnimatableMesh> AnimatableMeshPtr;

namespace SceneGraph
{
class UpdateManager;
class AnimatableMesh;
}


class AnimatableMesh : public ProxyObject
{
public:
  /**
   * Constructor
   * @param[in] updateManager
   * @param[in] sceneObject - pointer to a newly created scene object
   * @param[in] mesh - pointer to a newly created Mesh object
   * @param[in] numberOfVertices - The number of vertices to create
   */
  AnimatableMesh( SceneGraph::UpdateManager& updateManager,
                  SceneGraph::AnimatableMesh* sceneObject,
                  MeshIPtr mesh,
                  int numberOfVertices );

  /**
   * Construct an animated mesh. Uses vertex color
   */
  static AnimatableMeshPtr New( unsigned int numVertices,
                                const Dali::AnimatableMesh::Faces& faceIndices );

  /**
   * Construct an animated mesh. Uses material
   */
  static AnimatableMeshPtr New( unsigned int numVertices,
                                const Dali::AnimatableMesh::Faces& faceIndices,
                                Dali::Material material);

  /**
   * Catch-all constructor. Performs all checks on input data
   */
  static AnimatableMeshPtr New( unsigned int numVertices,
                                const Dali::AnimatableMesh::Faces& faceIndices,
                                Dali::Material material,
                                bool useColor );

  /**
   * Destructor
   */
  ~AnimatableMesh();

  /**
   * Return the number of vertices in the mesh
   * @return vertexNumber
   */
  unsigned int GetNumberOfVertices() const;

  /**
   * Get the property index from the vertex index and the vertex property enumeration
   */
  Property::Index GetVertexPropertyIndex( unsigned int vertex, Property::Index property ) const;

  /**
   * Set the given vertex position
   */
  void SetPosition( unsigned int vertexIndex, const Vector3& position);

  /**
   * Set the given vertex color. Note, alpha is ignored with current internal vertex format
   */
  void SetColor( unsigned int vertexIndex, const Vector4& color);

  /**
   * Set the given texture coords.
   */
  void SetTextureCoords( unsigned int vertexIndex, const Vector2& coords);

  /**
   * Get the current position of the vertex
   */
  const Vector3& GetCurrentPosition( unsigned int vertexIndex) const;

  /**
   * Get the current color of the vertex
   */
  const Vector4& GetCurrentColor( unsigned int vertexIndex) const;

  /**
   * Get the current texture coord of the vertex
   */
  const Vector2& GetCurrentTextureCoords( unsigned int vertexIndex) const;

  /**
   * Set whether the mesh should use the vertex color property or not.
   * If it uses the vertex colors, then the material is not used for rendering.
   */
  void SetUseColor(bool color);

  /**
   * Get whether the mesh should use the color property or not
   * @return true if the mesh is using vertex colors
   */
  bool GetUseColor();

  /**
   * Set whether the mesh should use the vertex TextureCoords property or not.
   * If it uses the vertex TextureCoords, then the material is not used for rendering.
   */
  void SetUseTextureCoords(bool TextureCoords);

  /**
   * Get whether the mesh should use the TextureCoords property or not
   * @return true if the mesh is using vertex texture coords
   */
  bool GetUseTextureCoords();


  // Intended for internal use only.
  MeshIPtr GetMesh();

public: // Implementation of ProxyObject
  /**
   * @copydoc Dali::Internal::ProxyObject::Supports()
   */
  virtual bool Supports( Capability capability ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsSceneObjectRemovable()
   */
  virtual bool IsSceneObjectRemovable() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::ProxyObject::SetCustomProperty()
   */
  virtual void SetCustomProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value );

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::InstallSceneObjectProperty()
   */
  virtual void InstallSceneObjectProperty( SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index );

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

private:
  AnimatableMesh( const AnimatableMesh& );
  AnimatableMesh operator=(const AnimatableMesh& rhs);

private:
  SceneGraph::UpdateManager&  mUpdateManager; ///< The update manager
  SceneGraph::AnimatableMesh* mSceneObject;   ///< The mesh property owner
  MeshIPtr                    mMesh;          ///< The mesh ticket holder

  unsigned int mNumberOfVertices;
  int mPropertyCount;

};

}//Internal

inline Internal::AnimatableMesh& GetImplementation(Dali::AnimatableMesh& animatableMesh)
{
  DALI_ASSERT_ALWAYS(animatableMesh && "AnimatableMesh handle is empty");
  BaseObject& handle = animatableMesh.GetBaseObject();
  return static_cast<Internal::AnimatableMesh&>(handle);
}

inline const Internal::AnimatableMesh& GetImplementation(const Dali::AnimatableMesh& animatableMesh)
{
  DALI_ASSERT_ALWAYS(animatableMesh && "AnimatableMesh handle is empty");
  const BaseObject& handle = animatableMesh.GetBaseObject();
  return static_cast<const Internal::AnimatableMesh&>(handle);
}

}//Dali

#endif
