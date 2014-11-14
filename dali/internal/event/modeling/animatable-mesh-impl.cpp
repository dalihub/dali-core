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

#include <dali/internal/event/common/property-index-ranges.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/modeling/animatable-mesh-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/event/modeling/material-impl.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/modeling/scene-graph-animatable-mesh.h>

namespace Dali
{

const Property::Index AnimatableVertex::POSITION          = 0;
const Property::Index AnimatableVertex::COLOR             = 1;
const Property::Index AnimatableVertex::TEXTURE_COORDS    = 2;

namespace Internal
{

namespace
{

const std::string DEFAULT_PROPERTY_NAMES[] =
{
  "position",
  "vertex-color",
  "texture-coords"
};
const int VERTEX_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTY_NAMES ) / sizeof( std::string );

const Property::Type DEFAULT_PROPERTY_TYPES[ VERTEX_PROPERTY_COUNT ] =
{
  Property::VECTOR3,  // position
  Property::VECTOR4,  // Color
  Property::VECTOR2,  // Texture Coords
};

} // namespace

AnimatableMesh::DefaultPropertyLookup* AnimatableMesh::mDefaultPropertyLookup = NULL;


AnimatableMesh::AnimatableMesh(
  SceneGraph::UpdateManager& updateManager,
  SceneGraph::AnimatableMesh* sceneObject,
  MeshIPtr mesh,
  int numberOfVertices )
: mUpdateManager( updateManager ),
  mSceneObject( sceneObject ),
  mMesh( mesh ),
  mNumberOfVertices( numberOfVertices ),
  mPropertyCount( numberOfVertices * VERTEX_PROPERTY_COUNT )
{
  // Transfer animatable ownership to a scene message
  AddAnimatableMeshMessage( mUpdateManager, *mSceneObject );
}


AnimatableMeshPtr AnimatableMesh::New(
  unsigned int numVertices,
  const Dali::AnimatableMesh::Faces& faceIndices )
{
  return New( numVertices, faceIndices, Dali::Material::New("dummy material"), true );
}

AnimatableMeshPtr AnimatableMesh::New(
  unsigned int numVertices,
  const Dali::AnimatableMesh::Faces& faceIndices,
  Dali::Material material)
{
  return New( numVertices, faceIndices, material, false );
}


AnimatableMeshPtr AnimatableMesh::New(
  unsigned int numVertices,
  const Dali::AnimatableMesh::Faces& faceIndices,
  Dali::Material material,
  bool useVertexColor )
{
  DALI_ASSERT_ALWAYS( numVertices > 0 && "Mesh has no vertices" );
  DALI_ASSERT_ALWAYS( ( numVertices * 3 ) < DEFAULT_PROPERTY_MAX_COUNT && "Mesh exceeds maximum supported vertices" );
  DALI_ASSERT_ALWAYS( faceIndices.size() > 0 && "Mesh has no faces" );
  for( Dali::AnimatableMesh::FacesConstIter faceIter=faceIndices.begin();
       faceIter != faceIndices.end();
       ++faceIter )
  {
    unsigned int faceIndex = *faceIter;
    DALI_ASSERT_ALWAYS( faceIndex < numVertices && "Face index out of range" );
  }

  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  SceneGraph::UpdateManager& updateManager = tls.GetUpdateManager();
  ResourceManager& resourceManager = tls.GetResourceManager();

  Dali::MeshData meshData;

  // We need a valid mesh data object to create the scene graph mesh
  Dali::MeshData::VertexContainer vertices( numVertices );
  BoneContainer bones;
  meshData.SetData(vertices, faceIndices, bones, material);
  bool useColor = false;
  if(useVertexColor)
  {
    useColor=true;
  }
  meshData.SetHasColor(useColor);
  meshData.SetHasNormals(false);
  meshData.SetHasTextureCoords(true);

  MeshIPtr mesh( Mesh::New( meshData, false /* not discardable, data is updated in the update thread */, true /* Scaling is required */ ) );

  // Create the scene object
  SceneGraph::AnimatableMesh* sceneObject = new SceneGraph::AnimatableMesh( resourceManager, mesh->GetResourceId(), meshData.GetVertices() );

  // Create the event object
  AnimatableMeshPtr animatableMeshPtr( new AnimatableMesh( updateManager, sceneObject, mesh, meshData.GetVertexCount() ) );

  return animatableMeshPtr;
}


AnimatableMesh::~AnimatableMesh()
{
  // Guard to allow handle destruction after Core has been destroyed
  if ( Stage::IsInstalled() )
  {
    if( mSceneObject )
    {
      RemoveAnimatableMeshMessage( mUpdateManager, *mSceneObject );
    }
  }
}

unsigned int AnimatableMesh::GetNumberOfVertices() const
{
  return mNumberOfVertices;
}

Property::Index AnimatableMesh::GetVertexPropertyIndex(
  unsigned int vertex,
  Property::Index property ) const
{
  DALI_ASSERT_DEBUG(property < VERTEX_PROPERTY_COUNT );
  return ( vertex * VERTEX_PROPERTY_COUNT ) + property;
}

void AnimatableMesh::SetPosition( unsigned int vertexIndex, const Vector3& position)
{
  StagePtr stage = Stage::GetCurrent();
  BakeVertexPositionMessage( stage->GetUpdateInterface(), *mSceneObject, vertexIndex, position );
}

void AnimatableMesh::SetColor( unsigned int vertexIndex, const Vector4& color)
{
  StagePtr stage = Stage::GetCurrent();
  BakeVertexColorMessage( stage->GetUpdateInterface(), *mSceneObject, vertexIndex, color );
}

void AnimatableMesh::SetTextureCoords( unsigned int vertexIndex, const Vector2& coords)
{
  StagePtr stage = Stage::GetCurrent();
  BakeVertexTextureCoordsMessage( stage->GetUpdateInterface(), *mSceneObject, vertexIndex, coords );
}

const Vector3& AnimatableMesh::GetCurrentPosition( unsigned int vertexIndex) const
{
  StagePtr stage = Stage::GetCurrent();
  return mSceneObject->GetPosition(stage->GetEventBufferIndex(), vertexIndex);
}

const Vector4& AnimatableMesh::GetCurrentColor( unsigned int vertexIndex) const
{
  StagePtr stage = Stage::GetCurrent();
  return mSceneObject->GetColor(stage->GetEventBufferIndex(), vertexIndex);
}

const Vector2& AnimatableMesh::GetCurrentTextureCoords( unsigned int vertexIndex) const
{
  StagePtr stage = Stage::GetCurrent();
  return mSceneObject->GetTextureCoords(stage->GetEventBufferIndex(), vertexIndex);
}

MeshIPtr AnimatableMesh::GetMesh()
{
  return mMesh;
}

/********************************************************************************
 ********************************************************************************
 ********************************   PROPERTY METHODS   **************************
 ********************************************************************************
 ********************************************************************************/

bool AnimatableMesh::Supports( Capability capability ) const
{
  return false;
}

bool AnimatableMesh::IsSceneObjectRemovable() const
{
  return false; // The scene object is permanently "on stage" whilst this object is alive
}

unsigned int AnimatableMesh::GetDefaultPropertyCount() const
{
  return mPropertyCount;
}

void AnimatableMesh::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( mPropertyCount );

  for ( int i = 0; i < mPropertyCount; ++i )
  {
    indices.push_back( i );
  }
}

const std::string& AnimatableMesh::GetDefaultPropertyName( Property::Index index ) const
{
  if ( ( index >= 0 ) && ( index < mPropertyCount ) )
  {
    return DEFAULT_PROPERTY_NAMES[index % VERTEX_PROPERTY_COUNT];
  }
  else
  {
    // Index out-of-range... return empty string.
    static const std::string INVALID_PROPERTY_NAME;
    return INVALID_PROPERTY_NAME;
  }
}

Property::Index AnimatableMesh::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // TODO: Property names should be modified to append the vertex index

  return index;
}

bool AnimatableMesh::IsDefaultPropertyWritable(Property::Index index) const
{
  return true;
}

bool AnimatableMesh::IsDefaultPropertyAnimatable(Property::Index index) const
{
  return true;
}

bool AnimatableMesh::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return true;
}

Property::Type AnimatableMesh::GetDefaultPropertyType(Property::Index index) const
{
  if ( ( index >= 0 ) && ( index < mPropertyCount ) )
  {
    return DEFAULT_PROPERTY_TYPES[index % VERTEX_PROPERTY_COUNT ];
  }
  else
  {
    // Index out-of-bounds
    return Property::NONE;
  }
}

void AnimatableMesh::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  DALI_ASSERT_ALWAYS( ( index >= 0 ) && ( index < mPropertyCount ) );

  int vertexProperty = index % VERTEX_PROPERTY_COUNT;
  int vertexIndex    = index / VERTEX_PROPERTY_COUNT;
  switch ( vertexProperty )
  {
    case Dali::AnimatableVertex::POSITION:
    {
      SetPosition( vertexIndex, property.Get<Vector3>() );
      break;
    }
    case Dali::AnimatableVertex::COLOR:
    {
      SetColor( vertexIndex, property.Get<Vector4>() );
      break;
    }
    case Dali::AnimatableVertex::TEXTURE_COORDS:
    {
      SetTextureCoords( vertexIndex, property.Get<Vector2>() );
      break;
    }
  }
}

void AnimatableMesh::SetCustomProperty( Property::Index /*index*/, const CustomProperty& /*entry*/, const Property::Value& /*value*/ )
{
  DALI_ASSERT_ALWAYS( 0 && "AnimatableMesh does not support custom properties");
}

Property::Value AnimatableMesh::GetDefaultProperty(Property::Index index) const
{
  Property::Value value;
  int vertexProperty = index % VERTEX_PROPERTY_COUNT;
  int vertexIndex    = index / VERTEX_PROPERTY_COUNT;

  switch ( vertexProperty )
  {
    case Dali::AnimatableVertex::POSITION:
    {
      value = GetCurrentPosition(vertexIndex);
      break;
    }
    case Dali::AnimatableVertex::COLOR:
    {
      value = GetCurrentColor(vertexIndex);
      break;
    }
    case Dali::AnimatableVertex::TEXTURE_COORDS:
    {
      value = GetCurrentTextureCoords(vertexIndex);
      break;
    }
  }

  return value;
}

void AnimatableMesh::InstallSceneObjectProperty( SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index )
{
  DALI_ASSERT_ALWAYS( 0 && "AnimatableMesh does not support custom properties" );
}

const SceneGraph::PropertyOwner* AnimatableMesh::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* AnimatableMesh::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable(index) && "Property is not animatable" );

  const SceneGraph::PropertyBase* property( NULL );

  // This method should only return a property which is part of the scene-graph
  if( mSceneObject != NULL )
  {
    int vertexProperty = index % VERTEX_PROPERTY_COUNT;
    int vertexIndex    = index / VERTEX_PROPERTY_COUNT;

    switch ( vertexProperty )
    {
      case Dali::AnimatableVertex::POSITION:
        property = &mSceneObject->mVertices[vertexIndex].position;
        break;
      case Dali::AnimatableVertex::COLOR:
        property = &mSceneObject->mVertices[vertexIndex].color;
        break;
      case Dali::AnimatableVertex::TEXTURE_COORDS:
        property = &mSceneObject->mVertices[vertexIndex].textureCoords;
        break;
    }
  }

  return property;
}

const PropertyInputImpl* AnimatableMesh::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property( NULL );
  if( mSceneObject != NULL )
  {
    int vertexProperty = index % VERTEX_PROPERTY_COUNT;
    int vertexIndex    = index / VERTEX_PROPERTY_COUNT;

    switch ( vertexProperty )
    {
      case Dali::AnimatableVertex::POSITION:
        property = &mSceneObject->mVertices[vertexIndex].position;
        break;
      case Dali::AnimatableVertex::COLOR:
        property = &mSceneObject->mVertices[vertexIndex].color;
        break;
      case Dali::AnimatableVertex::TEXTURE_COORDS:
        property = &mSceneObject->mVertices[vertexIndex].textureCoords;
        break;
    }
  }

  return property;
}

} // internal
} // dali
