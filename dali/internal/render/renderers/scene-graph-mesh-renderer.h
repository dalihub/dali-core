#ifndef __DALI_INTERNAL_SCENE_GRAPH_MESH_RENDERER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_MESH_RENDERER_H__

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
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/render/renderers/render-material.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>
#include <dali/internal/update/modeling/bone-transforms.h>
#include <dali/internal/render/shaders/custom-uniform.h>
#include <dali/internal/event/effects/shader-declarations.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class RenderDataProvider;
struct BoneTransforms;
class LightController;
class RenderMaterial;
class Mesh;

/**
 * An attachment for rendering a mesh with a material.
 */
class MeshRenderer : public Renderer
{
public:

  /**
   * The information required to render a single mesh
   */
  struct MeshInfo
  {
    MeshInfo()
    : mesh(NULL),
      material(NULL),
      boneTransforms()
    {
    }

    Mesh*                 mesh;
    RenderMaterial*       material;
    BoneTransforms        boneTransforms; ///< Note, this is a std::vector. May be realloced in off-frame
  };

  /**
   * Construct a new MeshRenderer.
   * @param dataprovider to render
   * @param lightController to get the light information
   * @return The newly allocated MeshRenderer.
   */
  static MeshRenderer* New( RenderDataProvider& dataprovider, LightController& lightController );

  /**
   * Retrieve the mesh information for the next frame.
   * This should only be accessed from the update-thread, using the current update buffer.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  MeshInfo& GetMeshInfo( BufferIndex updateBufferIndex )
  {
    return mMeshInfo[ updateBufferIndex ];
  }

  /**
   * Called when the shader has changed; mCustomUniform should be reset.
   */
  void ResetCustomUniforms();

  /**
   * Set whether the renderer should be affected by scene lighting, or evenly lit
   */
  void SetAffectedByLighting( bool affectedByLighting );

  /**
   * Virtual destructor
   */
  virtual ~MeshRenderer();

private:

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlContextDestroyed()
   */
  virtual void GlContextDestroyed();

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlCleanup()
   */
  virtual void GlCleanup();

  /**
   * @copydoc Dali::Internal::SceneGraph::Renderer::RequiresDepthTest()
   */
  virtual bool RequiresDepthTest() const;

  /**
   * @copydoc Dali::Internal::SceneGraph::Renderer::CheckResources()
   */
  virtual bool CheckResources();

  /**
   * @copydoc Dali::Internal::SceneGraph::Renderer::GetGeometryTypes()
   */
  virtual void GetGeometryTypes( BufferIndex bufferIndex, GeometryType& outType, ShaderSubTypes& outSubType );

  /**
   * @copydoc Dali::Internal::SceneGraph::Renderer::DoRender()
   */
  virtual void DoRender( BufferIndex bufferIndex, const Matrix& modelViewMatrix, const Matrix& modelMatrix, const Matrix& viewMatrix, const Matrix& projectionMatrix, const Vector4& color, bool cullTest );

  /**
   * Apply the view matrix to the bone transforms, and generate inverse transforms (for normal
   * calculations)
   * @param[inout] boneTransforms The bone transform matrices to update
   * @param[in]    viewMatrix     The current view matrix
   */
  void ApplyViewToBoneTransforms( BoneTransforms& boneTransforms, const Matrix& viewMatrix );

  /**
   * Private constructor. See New()
   */
  MeshRenderer(RenderDataProvider& dataprovider);

  // Undefined
  MeshRenderer(const MeshRenderer&);

  // Undefined
  MeshRenderer& operator=(const MeshRenderer& rhs);

private:

  MeshInfo         mMeshInfo[2];          ///< Double-buffered for update/render in separate threads.
  LightController* mLightController;      ///< required to get the lights from the scene.
  bool             mAffectedByLighting;   ///< Whether the scene lights should be used
  GeometryType     mGeometryType;         ///< Records last geometry type
  ShaderSubTypes   mShaderType;           ///< Records last shader type

  static const unsigned int mNumberOfCustomUniforms = 13; // Number of uniforms needed for renderer
  CustomUniform             mCustomUniform[SHADER_SUBTYPE_LAST][ mNumberOfCustomUniforms ];
  RenderMaterialUniforms    mRenderMaterialUniforms; ///< Uniforms for render material

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_MESH_RENDERER_H__
