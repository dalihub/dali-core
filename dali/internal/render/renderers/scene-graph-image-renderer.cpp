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
#include <dali/internal/render/renderers/scene-graph-image-renderer.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/common/vertex.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/shader.h>
#include <dali/internal/update/controllers/scene-controller.h>

using namespace std;

namespace
{
/**
 * VertexToTextureCoord
 * Represents a mapping between a 1 dimensional vertex coordinate
 * and a 1 dimensional texture coordinate.
 */
struct VertexToTextureCoord
{
  /**
   * @param[in] xVertex Vertex coordinate
   * @param[in] uTexture Texture coordinate
   */
  VertexToTextureCoord(float xVertex, float uTexture)
  : x(xVertex),
    u(uTexture)
  {
  }

  float x;    ///< 1D Vertex position
  float u;    ///< 1D Texture position
};

/**
 * Generates a list of equally spaced intervals along a line, including
 * intervals at the points specified in insertionList.
 * The line starts from insertionList.begin() and ends at insertionList.end()-1
 * The number of intervals and spacing of these intervals is specified by
 * the caller.
 * @param[out] intervalList An empty vector to be populated with the list of intervals.
 * @param[in] intervals The number of intervals to be generated.
 * @param[in] insertionList A vector containing the points on the line to be inserted.
 */
void GenerateIntervals(std::vector<VertexToTextureCoord>& intervalList, int intervals, const std::vector<VertexToTextureCoord>& insertionList)
{
  DALI_ASSERT_DEBUG(insertionList.size() >= 2);
  DALI_ASSERT_DEBUG(intervals > 0);

  std::vector<VertexToTextureCoord>::const_iterator iter = insertionList.begin();
  std::vector<VertexToTextureCoord>::const_iterator end = insertionList.end()-1;

  const float length = end->x - iter->x;
  const float intervalSize = length / static_cast<float>(intervals);
  float x = iter->x;

  for(;iter!=end;++iter)
  {
    float x0 = iter[0].x;
    float u0 = iter[0].u;
    float x1 = iter[1].x;
    float u1 = iter[1].u;

    for(;x<x1;x+=intervalSize)
    {
      float progress = (x - x0) / (x1 - x0);  // progress value between current interval and next.
      float u = u0 + (u1 - u0) * progress;    // u 1D texture coordinate value for this x position.
      intervalList.push_back( VertexToTextureCoord( x, u ) );
    }
    intervalList.push_back( VertexToTextureCoord( x1, u1 ) );
  }
}

}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

ImageRenderer* ImageRenderer::New( RenderDataProvider& dataprovider )
{
  return new ImageRenderer( dataprovider );
}

ImageRenderer::~ImageRenderer()
{
  if ( mTextureId > 0 )
  {
    mTextureCache->RemoveObserver(mTextureId, this);
  }

  GlCleanup();
}

void ImageRenderer::SetTextureId( ResourceId textureId )
{
  if ( mTextureId > 0 )
  {
    mTextureCache->RemoveObserver(mTextureId, this);
  }

  mTextureId = textureId;
  mTexture = NULL;

  if ( textureId > 0 )
  {
    mTextureCache->AddObserver(textureId, this);
  }
}

void ImageRenderer::SetPixelArea( const ImageRenderer::PixelArea& pixelArea )
{
  mUsePixelArea    = true;
  mPixelArea       = pixelArea;
  mIsMeshGenerated = false;
}

void ImageRenderer::SetNinePatchBorder( const Vector4& border, bool inPixels )
{
  mBorder          = border;
  mBorderInPixels  = inPixels;
  mIsMeshGenerated = false;
}

void ImageRenderer::CalculateMeshData( MeshType type, const Vector2& targetSize, bool usePixelArea )
{
  mMeshType        = type;
  mGeometrySize    = targetSize;
  mUsePixelArea    = usePixelArea;
  mIsMeshGenerated = false;
}

void ImageRenderer::TextureDiscarded( ResourceId textureId )
{
  DALI_ASSERT_DEBUG( mTextureId == textureId || mTextureId == 0 );

  mTextureId = 0;
  mTexture = NULL;
}

void ImageRenderer::GlContextDestroyed()
{
  if( mVertexBuffer )
  {
    mVertexBuffer->GlContextDestroyed();
  }
  if( mIndexBuffer )
  {
    mIndexBuffer->GlContextDestroyed();
  }
  // force recreation of the geometry in next render
  mIsMeshGenerated = false;
}

void ImageRenderer::GlCleanup()
{
  mVertexBuffer.Reset();
  mIndexBuffer.Reset();
}

bool ImageRenderer::RequiresDepthTest() const
{
  return false;
}

bool ImageRenderer::CheckResources()
{
  if( mTexture == NULL )
  {
    mTexture = mTextureCache->GetTexture( mTextureId );
  }

  if( mTexture == NULL )
  {
    return false;
  }

  if( ( mTexture->GetWidth() <= 0u ) ||
      ( mTexture->GetHeight() <= 0u ) )
  {
    return false;
  }

  Integration::ResourceId shaderTextureId =  mShader->GetTextureIdToRender() ;

  if( shaderTextureId &&  mTextureCache->GetTexture( shaderTextureId ) == NULL )
  {
    return false;
  }

  return true;
}

void ImageRenderer::ResolveGeometryTypes( BufferIndex bufferIndex, GeometryType& outType, ShaderSubTypes& outSubType )
{
  outType = GEOMETRY_TYPE_IMAGE;
  outSubType = SHADER_DEFAULT;
}

bool ImageRenderer::IsOutsideClipSpace( const Matrix& modelMatrix, const Matrix& modelViewProjectionMatrix )
{
  mContext->IncrementRendererCount();
  if(IsOutsideClipSpaceImpl( modelMatrix, modelViewProjectionMatrix ) )
  {
    mContext->IncrementCulledCount();
    return true;
  }
  return false;
}

void ImageRenderer::DoRender( BufferIndex bufferIndex, Program& program, const Matrix& modelViewMatrix, const Matrix& viewMatrix )
{
  DALI_ASSERT_DEBUG( 0 != mTextureId && "ImageRenderer::DoRender. mTextureId == 0." );
  DALI_ASSERT_DEBUG( NULL != mTexture && "ImageRenderer::DoRender. mTexture == NULL." );

  if(! mIsMeshGenerated )
  {
    GenerateMeshData( mTexture );
  }

  DALI_ASSERT_DEBUG( mVertexBuffer );

  mTextureCache->BindTexture( mTexture, mTextureId,  GL_TEXTURE_2D, GL_TEXTURE0 );

  // make sure the vertex is bound, this has to be done before
  // we call VertexAttribPointer otherwise you get weird output on the display
  mVertexBuffer->Bind();

  // Set sampler uniform
  GLint samplerLoc = program.GetUniformLocation( Program::UNIFORM_SAMPLER );
  if( -1 != samplerLoc )
  {
    // set the uniform
    program.SetUniform1i( samplerLoc, 0 );
  }

  samplerLoc = program.GetUniformLocation( Program::UNIFORM_SAMPLER_RECT );
  if( -1 != samplerLoc )
  {
    UvRect uv;

    if ( mUsePixelArea )
    {
      mTexture->GetTextureCoordinates( uv, &mPixelArea );
    }
    else
    {
      mTexture->GetTextureCoordinates( uv, NULL );
    }

    // set the uniform
    program.SetUniform4f( samplerLoc, uv.u0, uv.v0, uv.u2, uv.v2 );
  }

  // Check whether the program supports the expected attributes/uniforms
  const GLint positionLoc = program.GetAttribLocation( Program::ATTRIB_POSITION );
  const GLint texCoordLoc = program.GetAttribLocation( Program::ATTRIB_TEXCOORD );

  if ( positionLoc != -1 )
  {
    mContext->EnableVertexAttributeArray( positionLoc );

    const int stride = 4 * sizeof(float);
    mContext->VertexAttribPointer( positionLoc, 2, GL_FLOAT, GL_FALSE, stride, 0 );
  }

  if ( texCoordLoc != -1 )
  {
    mContext->EnableVertexAttributeArray( texCoordLoc );

    const int stride = 4 * sizeof(float);
    mContext->VertexAttribPointer( texCoordLoc, 2, GL_FLOAT, GL_FALSE, stride, (const void*) (sizeof(float)*2) );
  }

  switch(mMeshType)
  {
    case QUAD:
    case NINE_PATCH:
    {
      const GLsizei vertexCount = mVertexBuffer->GetBufferSize() / sizeof(Vertex2D); // compiler will optimize this to >> if possible
      mContext->DrawArrays( GL_TRIANGLE_STRIP, 0, vertexCount );
      DRAW_ARRAY_RECORD( vertexCount );
      break;
    }
    case GRID_QUAD:
    case GRID_NINE_PATCH:
    {
      const GLsizei indexCount = mIndexBuffer->GetBufferSize() / sizeof(GLushort); // compiler will optimize this to >> if possible
      mIndexBuffer->Bind();
      mContext->DrawElements( GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0 );
      DRAW_ELEMENT_RECORD( indexCount );
      break;
    }
  }

  if ( positionLoc != -1 )
  {
    mContext->DisableVertexAttributeArray( positionLoc );
  }

  if ( texCoordLoc != -1 )
  {
    mContext->DisableVertexAttributeArray( texCoordLoc );
  }
}

void ImageRenderer::UpdateVertexBuffer( GLsizeiptr size, const GLvoid *data )
{
  // create/destroy if needed/not needed.
  if ( size && !mVertexBuffer )
  {
    mVertexBuffer = new GpuBuffer( *mContext, GpuBuffer::ARRAY_BUFFER, GpuBuffer::DYNAMIC_DRAW );
  }
  else if ( !size && mVertexBuffer )
  {
    mVertexBuffer.Reset();
  }

  // update
  if ( mVertexBuffer )
  {
    mVertexBuffer->UpdateDataBuffer( size, data );
  }
}

void ImageRenderer::UpdateIndexBuffer( GLsizeiptr size, const GLvoid *data )
{
  // create/destroy if needed/not needed.
  if ( size && !mIndexBuffer )
  {
    mIndexBuffer = new GpuBuffer( *mContext, GpuBuffer::ELEMENT_ARRAY_BUFFER, GpuBuffer::STATIC_DRAW );
  }
  else if ( !size && mIndexBuffer )
  {
    mIndexBuffer.Reset();
  }

  // update
  if ( mIndexBuffer )
  {
    mIndexBuffer->UpdateDataBuffer(size,data);
  }
}

void ImageRenderer::GenerateMeshData( Texture* texture )
{
  const PixelArea* pixelArea = NULL;
  if( mUsePixelArea )
  {
    pixelArea = &mPixelArea;
  }

  switch( mMeshType )
  {
    case ImageRenderer::QUAD:
    {
      SetQuadMeshData( texture, mGeometrySize, pixelArea );
      break;
    }
    case ImageRenderer::NINE_PATCH:
    {
      SetNinePatchMeshData( texture, mGeometrySize, mBorder, mBorderInPixels, pixelArea );
      break;
    }
    case ImageRenderer::GRID_QUAD:
    {
      SetGridMeshData( texture, mGeometrySize, NULL, false, pixelArea );
      break;
    }
    case ImageRenderer::GRID_NINE_PATCH:
    {
      SetGridMeshData( texture, mGeometrySize, &mBorder, mBorderInPixels, pixelArea );
      break;
    }
  }
  mIsMeshGenerated = true;
}

void ImageRenderer::SetQuadMeshData( Texture* texture, const Vector2& size, const PixelArea* pixelArea )
{
  const float x0 = -0.5f * size.x;
  const float y0 = -0.5f * size.y;
  const float x1 =  0.5f * size.x;
  const float y1 =  0.5f * size.y;

  /*
   * Here we render the square as a single square, as texture
   * coordinates linearly interpolate between the 4 vertices.
   *
   * Note: a square (or a quad) is rendered as 2 triangles.
   * Vertices 0,1,2 represent triangle A.
   * Vertices 1,2,3 represent triangle B.
   *
   * No indices are needed as we tell GL to render in strip mode
   * (GL_TRIANGLE_STRIP), which is faster, and consumes less
   * memory.
   *
   *  0---------2
   *  |        /|
   *  |  A    / |
   *  |      /  |
   *  |     /   |
   *  |    /    |
   *  |   /     |
   *  |  /      |
   *  | /    B  |
   *  |/        |
   *  1---------3
   */

  Vertex2D verts[]={
                     {x0, y0, 0.0, 0.0},
                     {x0, y1, 0.0, 1.0},
                     {x1, y0, 1.0, 0.0},
                     {x1, y1, 1.0, 1.0}
                   };

  // We may only be displaying an area of the texture.
  // Calling MapUV converts the u,v values to correct values for the pixel area

  texture->MapUV( sizeof(verts)/sizeof(Vertex2D), verts, pixelArea );

  UpdateVertexBuffer( sizeof(verts), verts );
  UpdateIndexBuffer( 0, NULL );
}

void ImageRenderer::SetNinePatchMeshData( Texture* texture, const Vector2& size, const Vector4& border, bool borderInPixels, const PixelArea* pixelArea )
{
  DALI_ASSERT_ALWAYS( mTexture->GetWidth()  > 0.0f && "Invalid Texture width" );
  DALI_ASSERT_ALWAYS( mTexture->GetHeight() > 0.0f && "Invalid Texture height" );

  float textureWidth  = mTexture->GetWidth();
  float textureHeight = mTexture->GetHeight();

  float borderLeft, borderTop, borderRight, borderBottom; // pixels from edge
  float borderX0, borderY0, borderX1, borderY1; // In the range 0 -> 1

  if ( borderInPixels )
  {
    borderLeft   = border.x;
    borderTop    = border.y;
    borderRight  = border.z;
    borderBottom = border.w;

    borderX0 = border.x / textureWidth;
    borderY0 = border.y / textureHeight;
    borderX1 = 1.0f - ( border.z / textureWidth );
    borderY1 = 1.0f - ( border.w / textureHeight );
  }
  else
  {
    borderLeft   = textureWidth  * border.x;
    borderTop    = textureHeight * border.y;
    borderRight  = textureWidth  * (1.0f - border.z);
    borderBottom = textureHeight * (1.0f - border.w);

    borderX0 = border.x;
    borderY0 = border.y;
    borderX1 = border.z;
    borderY1 = border.w;
  }

  const float u0 = 0.0f;
  const float u3 = 1.0f;
  const float u1 = borderX0;
  const float u2 = borderX1;

  const float v0 = 0.0f;
  const float v3 = 1.0f;
  const float v1 = borderY0;
  const float v2 = borderY1;

  const float x0 = size.x * -0.5;
  const float x1 = x0 + borderLeft;
  const float x2 = x0 + size.x - borderRight;
  const float x3 = x0 + size.x;

  const float y0 = size.y * -0.5;
  const float y1 = y0 + borderTop;
  const float y2 = y0 + size.y - borderBottom;
  const float y3 = y0 + size.y;

  /*
   * We're breaking a quad in to 9 smaller quads, so that when it's
   * stretched the corners maintain their size.
   * For speed the 9-patch is drawn with a single triangle span, the draw
   * order of the span is 1->9.
   * Previously it would draw three separate spans (1->3, 4->6, 7->9), but now it
   * it does it one go by turning the corner when gets to the end of each row.
   *
   * No indices are needed as we tell GL to render in strip mode
   * (GL_TRIANGLE_STRIP), which is faster, and consumes less
   * memory.
   *
   *  |---|---------------|---|
   *  |  7|    --> 8      | 9 |
   *  |---|---------------|---|
   *  |   |               |   |
   *  | 6 |     <-- 5     | 4 |
   *  |   |               |   |
   *  |   |               |   |
   *  |-------------------|---|
   *  | 1 |      2 -->    | 3 |
   *  |---|---------------|---|
   */

  Vertex2D verts[]={
                     // bottom left
                     {  x0, y0, u0, v0 },
                     {  x0, y1, u0, v1 },
                     {  x1, y0, u1, v0 },
                     {  x1, y1, u1, v1 },

                     // bottom right
                     {  x2, y0, u2, v0 },
                     {  x2, y1, u2, v1 },
                     {  x3, y0, u3, v0 },
                     {  x3, y1, u3, v1 },

                     // turn the corner
                     {  x3, y1, u3, v1 },
                     {  x3, y1, u3, v1 },

                     // 10 verts here

                     // middle right
                     {  x3, y2, u3, v2 },
                     {  x2, y1, u2, v1 },
                     {  x2, y2, u2, v2 },

                     // middle left

                     {  x1, y1, u1, v1 },
                     {  x1, y2, u1, v2 },
                     {  x0, y1, u0, v1 },
                     {  x0, y2, u0, v2 },

                     // turn the corner
                     {  x0, y2, u0, v2 },
                     {  x0, y2, u0, v2 },

                     // top left
                     {  x0, y3, u0, v3 },
                     {  x1, y2, u1, v2 },
                     {  x1, y3, u1, v3 },

                     // top right

                     {  x2, y2, u2, v2 },
                     {  x2, y3, u2, v3 },
                     {  x3, y2, u3, v2 },
                     {  x3, y3, u3, v3 },
                   };

  const unsigned int vertexCount = sizeof( verts ) / sizeof( verts[0] );

  texture->MapUV( vertexCount, verts, pixelArea );
  UpdateVertexBuffer( sizeof(verts), verts );
  UpdateIndexBuffer( 0, NULL );
}

void ImageRenderer::SetGridMeshData( Texture* texture, const Vector2& size, const Vector4* border, bool borderInPixels, const PixelArea* pixelArea )
{
  /*
   * Quad Grid:
   * In Grid Mode, we tessellate the single quad into smaller quads
   * at approximately (guideGridSize x guideGridSize) in size.
   *
   * Conversion of Quad to Gridded Quad.
   *
   * |-----------|    |---|---|---|
   * |           |    |   |   |   |
   * |           | -> |---|---|---|
   * |           |    |   |   |   |
   * |-----------|    |---|---|---|
   *
   * 9-Patch Grid:
   * In Grid Mode, we tessellate each quad of a 9-patch
   * (see SetNinePatchMeshData) into smaller quads at approximately
   * (guideGridSize x guideGridSize) in size.
   *
   * This satisfies the two requirements of a 9-patch with grid:
   *
   * 1. Texture coordinates within each section of the 9-patch
   * should change linearly to that 9-patch's rules.
   * 2. The image as as whole should provide Vertex points at
   * approximate guideGridSize intervals.
   *
   * The result should be the horizontal and vertical lines of
   * a 9-patch overlayed by the horizontal and vertical lines of
   * a grid.
   *
   * Non-overlayed:
   *  |     |     |     |     |     |     | <- Grid Markers
   * -|-------|-------------------|-------|
   *  |       |                   |       |
   *  |   1   |         2         |   3   |
   * -|       |                   |       |
   *  |-------+-------------------+-------|
   *  |       |                   |       |
   * -|       |                   |       |
   *  |       |                   |       |
   *  |       |                   |       |
   * -|   4   |         5         |   6   | <- 9 Patch.
   *  |       |                   |       |
   *  |       |                   |       |
   * -|       |                   |       |
   *  |       |                   |       |
   *  |-------+-------------------+-------|
   * -|       |                   |       |
   *  |   7   |         8         |   9   |
   *  |       |                   |       |
   * -|-------|-------------------|-------|
   *
   *
   * Overlayed:
   *  |     |     |     |     |     |     | <- Grid Markers
   * -|-------|-------------------|-------|
   *  |     | |   |     |     |   | |     |
   *  |     | |   |     |     |   | |     |
   * -|-----|-|---|-----|-----|---|-|-----|
   *  |-------+-------------------+-------|
   *  |     | |   |     |     |   | |     |
   * -|-----|-|---|-----|-----|---|-|-----|
   *  |     | |   |     |     |   | |     |
   *  |     | |   |     |     |   | |     |
   * -|-----|-|---|-----|-----|---|-|-----| <- 9 Patch.
   *  |     | |   |     |     |   | |     |
   *  |     | |   |     |     |   | |     |
   * -|-----|-|---|-----|-----|---|-|-----|
   *  |     | |   |     |     |   | |     |
   *  |-------+-------------------+-------|
   * -|-----|-|---|-----|-----|---|-|-----|
   *  |     | |   |     |     |   | |     |
   *  |     | |   |     |     |   | |     |
   * -|-------|-------------------|-------|
   */

  std::vector<VertexToTextureCoord> horizontalDivisions;
  std::vector<VertexToTextureCoord> verticalDivisions;

  const float guideGridSize = mShader->GetGridDensity();

  const int textureWidth = texture->GetWidth();
  const int textureHeight = texture->GetHeight();

  const float halfWidth = size.width * 0.5f;
  const float halfHeight = size.height * 0.5f;

  // Determine how many rectangles across and down to tesselate image into.
  const int guideRectX = (size.width / guideGridSize);
  const int guideRectY = (size.height / guideGridSize);

  // Build up list of points in X axis where vertices need to go.
  std::vector<VertexToTextureCoord> insertionList;
  insertionList.reserve(4);
  insertionList.push_back( VertexToTextureCoord( -halfWidth, 0.0f ) );

  // If 9-Patch Border exists, add additional border points in list.
  if(border)
  {
    float borderX0, borderX1, borderU0, borderU1;

    if ( borderInPixels )
    {
      borderX0 = border->x - halfWidth;
      borderX1 = halfWidth - border->z;

      borderU0 = border->x / textureWidth;
      borderU1 = 1.0f - (border->z / textureWidth);
    }
    else
    {
      borderX0 = border->x * textureWidth - halfWidth;
      borderX1 = halfWidth - (1.0f - border->z) * textureWidth;
      borderU0 = border->x;
      borderU1 = border->z;
    }

    insertionList.push_back( VertexToTextureCoord( borderX0, borderU0 ) );
    insertionList.push_back( VertexToTextureCoord( borderX1, borderU1 ) );
  }

  insertionList.push_back( VertexToTextureCoord( halfWidth, 1.0f ) );
  GenerateIntervals(horizontalDivisions, guideRectX + 2, insertionList);

  // Build up list of points in Y axis where vertices need to go.
  insertionList.clear();
  insertionList.push_back( VertexToTextureCoord( -halfHeight, 0.0f ) );

  // If 9-Patch Border exists, add additional border points in list.
  if(border)
  {
    float borderY0, borderY1, borderU0, borderU1;

    if ( borderInPixels )
    {
      borderY0 = border->y - halfHeight;
      borderY1 = halfHeight - border->w;

      borderU0 = border->y / textureHeight;
      borderU1 = 1.0f - (border->w / textureHeight);
    }
    else
    {
      borderY0 = border->y * textureHeight - halfHeight;
      borderY1 = halfHeight - (1.0f - border->w) * textureHeight;

      borderU0 = border->y;
      borderU1 = border->w;
    }

    insertionList.push_back( VertexToTextureCoord( borderY0, borderU0 ) );
    insertionList.push_back( VertexToTextureCoord( borderY1, borderU1 ) );
  }

  insertionList.push_back( VertexToTextureCoord( halfHeight, 1.0f ) );
  GenerateIntervals(verticalDivisions, guideRectY + 2, insertionList);

  // Now build up Vertex pattern based on the above X and Y lists.
  const int totalVertices = horizontalDivisions.size() * verticalDivisions.size();
  Vertex2D* vertices = new Vertex2D[totalVertices];
  Vertex2D* vertex = vertices;

  for(std::vector<VertexToTextureCoord>::const_iterator yIter = verticalDivisions.begin(); yIter != verticalDivisions.end(); ++yIter )
  {
    for(std::vector<VertexToTextureCoord>::const_iterator xIter = horizontalDivisions.begin(); xIter != horizontalDivisions.end(); ++xIter )
    {
      vertex->mX = xIter->x;
      vertex->mU = xIter->u;
      vertex->mY = yIter->x;
      vertex->mV = yIter->u;
      vertex++;
    }
  }

  // Build up Triangle indicies, very predictable pattern.
  const size_t rectX = horizontalDivisions.size() - 1;
  const size_t rectY = verticalDivisions.size() - 1;
  const size_t totalIndices = rectX * rectY * 6;             // 2 triangles per quad (rect) and 3 points to define each triangle.
  GLushort* indices = new GLushort[totalIndices];

  GenerateMeshIndices(indices, rectX, rectY);

  texture->MapUV( totalVertices, vertices, pixelArea );

  UpdateVertexBuffer( totalVertices * sizeof(Vertex2D) , vertices );
  UpdateIndexBuffer( totalIndices * sizeof(GLushort), indices );

  delete[] vertices;
  delete[] indices;
}

void ImageRenderer::GenerateMeshIndices(GLushort* indices, int rectanglesX, int rectanglesY)
{
  GLushort *i = indices;
  const int meshEndIndex = rectanglesY * (rectanglesX + 1);

  int index = 0;
  while(index < meshEndIndex)
  {
    const int rowEndIndex = index + rectanglesX;
    for (; index < rowEndIndex; index++ )
    {
      *i++ = index;
      *i++ = index + 1 + rectanglesX;
      *i++ = index + 1;

      *i++ = index + 1;
      *i++ = index + 1 + rectanglesX;
      *i++ = index + 2 + rectanglesX;
    }
    index++;    // one extra vertex per row than rects.
  }
}

ImageRenderer::ImageRenderer( RenderDataProvider& dataprovider )
: Renderer( dataprovider ),
  mTexture( NULL ),
  mBorder( 0.45, 0.45, 0.1, 0.1 ),
  mPixelArea(),
  mGeometrySize(),
  mTextureId( 0 ),
  mMeshType( ImageRenderer::QUAD ),
  mIsMeshGenerated( false ),
  mBorderInPixels( false ),
  mUsePixelArea( false )
{
}

// Frustum culling using clip space and oriented bounding box checks
bool ImageRenderer::IsOutsideClipSpaceImpl(const Matrix& modelMatrix, const Matrix& modelViewProjectionMatrix)
{
  // First, calculate if the center is inside clip space:

  // Downside is mvp matrix calc per renderer per frame
  // and up to 4 matrix * vector calls.
  const Matrix& mvp = modelViewProjectionMatrix;
  const Vector4 translation = mvp.GetTranslation();

  // Upside is point test is very simple:
  if( -translation.w <= translation.x  &&  translation.x <= translation.w &&
      -translation.w <= translation.y  &&  translation.y <= translation.w &&
      -translation.w <= translation.z  &&  translation.z <= translation.w)
  {
    // Definitely inside clip space - don't do any more processing
    return false;
  }

  // Transform oriented bounding box to clip space:
  Vector4 topLeft(    mGeometrySize.x * -0.5f, mGeometrySize.y * -0.5f, 0.0f, 1.0f);
  Vector4 topRight(   mGeometrySize.x *  0.5f, mGeometrySize.y * -0.5f, 0.0f, 1.0f);
  Vector4 bottomLeft( mGeometrySize.x * -0.5f, mGeometrySize.y *  0.5f, 0.0f, 1.0f);
  Vector4 bottomRight(mGeometrySize.x *  0.5f, mGeometrySize.y *  0.5f, 0.0f, 1.0f);

  Vector4 topLeftClip = mvp * topLeft;
  if( -topLeftClip.w <= topLeftClip.x && topLeftClip.x <= topLeftClip.w &&
      -topLeftClip.w <= topLeftClip.y && topLeftClip.y <= topLeftClip.w &&
      -topLeftClip.w <= topLeftClip.z && topLeftClip.z <= topLeftClip.w )
  {
    // Definitely inside clip space - don't do any more processing
    return false;
  }

  Vector4 bottomRightClip = mvp * bottomRight;
  if( -bottomRightClip.w <= bottomRightClip.x && bottomRightClip.x <= bottomRightClip.w &&
      -bottomRightClip.w <= bottomRightClip.y && bottomRightClip.y <= bottomRightClip.w &&
      -bottomRightClip.w <= bottomRightClip.z && bottomRightClip.z <= bottomRightClip.w )
  {
    // Definitely inside clip space - don't do any more processing
    return false;
  }

  Vector4 topRightClip = mvp * topRight;
  if( -topRightClip.w <= topRightClip.x && topRightClip.x <= topRightClip.w &&
      -topRightClip.w <= topRightClip.y && topRightClip.y <= topRightClip.w &&
      -topRightClip.w <= topRightClip.z && topRightClip.z <= topRightClip.w )
  {
    // Definitely inside clip space - don't do any more processing
    return false;
  }

  Vector4 bottomLeftClip = mvp * bottomLeft;
  if( -bottomLeftClip.w <= bottomLeftClip.x && bottomLeftClip.x <= bottomLeftClip.w &&
      -bottomLeftClip.w <= bottomLeftClip.y && bottomLeftClip.y <= bottomLeftClip.w &&
      -bottomLeftClip.w <= bottomLeftClip.z && bottomLeftClip.z <= bottomLeftClip.w )
  {
    // Definitely inside clip space - don't do any more processing
    return false;
  }

  // Check to see if all four points are outside each plane (AABB would cut this processing
  // in half)

  unsigned int insideLeftPlaneCount=0;
  unsigned int insideRightPlaneCount=0;
  unsigned int insideTopPlaneCount=0;
  unsigned int insideBottomPlaneCount=0;

  if(-topLeftClip.w <= topLeftClip.x) { insideLeftPlaneCount++; }
  if(-topRightClip.w <= topRightClip.x){ insideLeftPlaneCount++; }
  if(-bottomRightClip.w <= bottomRightClip.x) {insideLeftPlaneCount++;}
  if(-bottomLeftClip.w <= bottomLeftClip.x) {insideLeftPlaneCount++;}

  if( insideLeftPlaneCount == 0 )
  {
    return true;
  }

  if(topLeftClip.x <= topLeftClip.w) { insideRightPlaneCount++;}
  if(topRightClip.x <= topRightClip.w) { insideRightPlaneCount++; }
  if(bottomRightClip.x <= bottomRightClip.w) { insideRightPlaneCount++; }
  if(bottomLeftClip.x <= bottomLeftClip.w ) { insideRightPlaneCount++; }

  if( insideRightPlaneCount == 0 )
  {
    return true;
  }

  if(-topLeftClip.w <= topLeftClip.y ) {insideTopPlaneCount++; }
  if(-topRightClip.w <= topRightClip.y) {insideTopPlaneCount++; }
  if(-bottomRightClip.w <= bottomRightClip.y) {insideTopPlaneCount++;}
  if(-bottomLeftClip.w <= bottomLeftClip.y) { insideTopPlaneCount++;}

  if( insideTopPlaneCount == 0 )
  {
    return true;
  }

  if(topLeftClip.y <= topLeftClip.w) { insideBottomPlaneCount++; }
  if(topRightClip.y <= topRightClip.w) { insideBottomPlaneCount++; }
  if(bottomRightClip.y <= bottomRightClip.w) { insideBottomPlaneCount++; }
  if(bottomLeftClip.y <= bottomLeftClip.w) { insideBottomPlaneCount++; }

  if( insideBottomPlaneCount == 0 )
  {
    return true;
  }

  // Test if any planes are bisected, if they are, then there is likely to
  // be an intersection into clip space.

  if( insideLeftPlaneCount < 4 )
  {
    return false;
  }
  if( insideRightPlaneCount < 4 )
  {
    return false;
  }
  if( insideTopPlaneCount < 4 )
  {
    return false;
  }
  if( insideBottomPlaneCount < 4 )
  {
    return false;
  }

  return true;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
