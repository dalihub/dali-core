#ifndef __DALI_INTERNAL_SCENE_GRAPH_IMAGE_RENDERER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_IMAGE_RENDERER_H__

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
#include <dali/public-api/actors/image-actor.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture-observer.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>

namespace Dali
{

namespace Internal
{
class GpuBuffer;

namespace SceneGraph
{
class RenderDataProvider;

/**
 * Used to render an image.
 */
class ImageRenderer : public Renderer, public TextureObserver
{
public:

  typedef Dali::ImageActor::PixelArea PixelArea;

  enum MeshType
  {
    QUAD,
    NINE_PATCH,
    GRID_QUAD,
    GRID_NINE_PATCH
  };

  /**
   * Create a new ImageRenderer.
   * @param dataprovider to render
   * @return The newly allocated ImageRenderer.
   */
  static ImageRenderer* New( RenderDataProvider& dataprovider );

  /**
   * Virtual destructor
   */
  virtual ~ImageRenderer();

  /**
   * Set the texture used to render.
   * @param[in] textureId The id of the texture used to render.
   */
  void SetTextureId( ResourceId textureId );

  /**
   * Set the pixel area for the renderer
   * @param[in] pixelArea An area within the texture, used when building UV coordinates.
   */
  void SetPixelArea( const ImageRenderer::PixelArea& pixelArea );

  /**
   * Set 9 patch border for the image renderer
   * @param[in] border The border for nine-patch meshes.
   * @param[in] borderInPixels True if border is in pixels instead of percentages.
   */
  void SetNinePatchBorder( const Vector4& border, bool inPixels );

  /**
   * Calculate the mesh data used by the ImageRenderer.
   * @param[in] type The type of mesh data required; either quad, nine-patch or grid.
   * @param[in] targetSize The size which the mesh data should fit inside.
   * @param[in] usePixelArea whether to use pixel area or not
   */
  void CalculateMeshData( MeshType type, const Vector2& targetSize, bool usePixelArea );

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

protected: // TextureObserver implementation

  /**
   * @copydoc Dali::Internal::TextureObserver::TextureDiscarded()
   */
  virtual void TextureDiscarded( ResourceId textureId );

private:
  /**
   * Helper to update the vertex buffer.
   */
  void UpdateVertexBuffer( GLsizeiptr size, const GLvoid *data );

  /**
   * Helper to update the index buffer.
   */
  void UpdateIndexBuffer( GLsizeiptr size, const GLvoid *data );

  /**
   * Helper to generate mesh data when required
   * @param[in] texture Texture from which to get UV data
   */
  void GenerateMeshData( Texture* texture );

  /**
   * Helper to fill vertex/index buffers with quad data.
   * (Quads are simple meshes, and thus have a specialised mesh generation method)
   *
   * @param[in] texture Texture from which to get UV data
   * @param[in] size
   * @param[in] pixelArea An area within the texture, used when building UV coordinates. A value of NULL means use default image size.
   */
  void SetQuadMeshData( Texture* texture, const Vector2& size, const PixelArea* pixelArea );

  /**
   * Helper to fill vertex/index buffers with nine-patch data.
   * (9-Patches are simple meshes, and thus have a specialised mesh generation method)
   *
   * @param[in] texture Texture from which to get UV data
   * @param[in] size The target size.
   * @param[in] border 9 patch border information.
   * @param[in] borderInPixels True if border is in pixels instead of percentages.
   * @param[in] pixelArea An area within the texture, used when building UV coordinates. A value of NULL means use default image size.
   */
  void SetNinePatchMeshData( Texture* texture, const Vector2& size, const Vector4& border, bool borderInPixels, const PixelArea* pixelArea );

  /**
   * Helper to fill vertex/index buffers with grid data.
   * This can build grid meshes that are either of type Quad or type 9-Patch
   *
   * @param[in] size The target size.
   * @param[in] border 9 patch border information (pass NULL for no border i.e. a standard Quad)
   * @param[in] borderInPixels True if border is in pixels instead of percentages.
   * @param[in] pixelArea An area within the texture, used when building UV coordinates. A value of NULL means use default image size.
   */
  void SetGridMeshData( Texture* texture, const Vector2& size, const Vector4* border, bool borderInPixels, const PixelArea* pixelArea );

  /**
   * Helper to fill index buffer with uniform grid data.
   * @param[in] indices pointer to indices buffer to be filled
   * @param[in] rectanglesX number of columns for rectangles in grid
   * @param[in] rectanglesY number of rows for rectangles in grid
   */
  void GenerateMeshIndices(GLushort* indices, int rectanglesX, int rectanglesY);

  /**
   * Private constructor. @see New()
   */
  ImageRenderer(RenderDataProvider& dataprovider);

  // Undefined
  ImageRenderer(const ImageRenderer&);

  // Undefined
  ImageRenderer& operator=(const ImageRenderer& rhs);

  /**
   * @return true if the renderer is outside clip space and doesn't need rendering
   */
  bool IsOutsideClipSpace(const Matrix& modelMatrix);

private:

  Texture*    mTexture;

  OwnerPointer< GpuBuffer > mVertexBuffer;
  OwnerPointer< GpuBuffer > mIndexBuffer;

  Vector4   mBorder;
  PixelArea mPixelArea;
  Vector2   mGeometrySize;
  ResourceId  mTextureId;

  // flags
  MeshType  mMeshType        : 3; // 4 values fits in 3 bits just fine
  bool      mIsMeshGenerated : 1;
  bool      mBorderInPixels  : 1;
  bool      mUsePixelArea    : 1;

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_IMAGE_RENDERER_H__
