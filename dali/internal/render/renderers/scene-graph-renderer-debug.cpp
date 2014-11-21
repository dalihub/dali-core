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
 */

#include "scene-graph-renderer-debug.h"
#include <dali/integration-api/shader-data.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/renderers/render-data-provider.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

namespace
{
/* rewritten in another patch
#if defined ( DEBUG_ENABLED )

// Create shader for debug drawing
const std::string DEBUG_DRAW_VERTEX_SHADER(
  "attribute mediump vec3 aPosition;\n"
  "uniform mediump mat4 uMvpMatrix;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = uMvpMatrix * vec4(aPosition, 1.0);\n"
  "}\n" );

const std::string DEBUG_DRAW_FRAGMENT_SHADER(
  "uniform lowp vec4 uColor;\n"
  "void main()\n"
  "{\n"
  "  gl_FragColor = uColor;\n"
  "}\n" );

static Program* gDebugProgram(NULL); ///< a simple debug shader

#endif
*/
} // anonymous namespace

void DebugBoundingBox(Context& context, Rect<float> boundingBox, const Matrix& mvp)
{
/* rewritten in another patch
#if defined ( DEBUG_ENABLED )
  if( gDebugProgram == NULL )
  {
    Integration::ShaderDataPtr shaderData( new Integration::ShaderData( DEBUG_DRAW_VERTEX_SHADER, DEBUG_DRAW_FRAGMENT_SHADER ) );
    gDebugProgram = Program::New( shaderData.Get(), context, true );
  }

  context.SetBlend( false );
  context.CullFace( CullNone );

  const unsigned int numPoints=8;
  GLfloat vertices[numPoints*3] = {
    boundingBox.x,                      boundingBox.y, 0.0f,
    boundingBox.x,                      boundingBox.y + boundingBox.height, 0.0f,

    boundingBox.x,                      boundingBox.y + boundingBox.height, 0.0f,
    boundingBox.x + boundingBox.width,  boundingBox.y + boundingBox.height, 0.0f,

    boundingBox.x + boundingBox.width,  boundingBox.y + boundingBox.height, 0.0f,
    boundingBox.x + boundingBox.width,  boundingBox.y, 0.0f,

    boundingBox.x + boundingBox.width,  boundingBox.y, 0.0f,
    boundingBox.x,                      boundingBox.y, 0.0f,
  };

  gDebugProgram->Use();

  GpuBuffer vertexBuffer(context,GpuBuffer::ARRAY_BUFFER, GpuBuffer::STATIC_DRAW);
  vertexBuffer.UpdateDataBuffer(numPoints * 3 * sizeof(float), &vertices[0]);
  vertexBuffer.Bind();

  GLint positionLoc    = gDebugProgram->GetAttribLocation(Program::ATTRIB_POSITION);
  context.VertexAttribPointer( positionLoc, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0 );
  context.EnableVertexAttributeArray( positionLoc );

  GLint mvpLoc = gDebugProgram->GetUniformLocation(Program::UNIFORM_MVP_MATRIX);
  if( mvpLoc != Program::UNIFORM_UNKNOWN )
  {
    gDebugProgram->SetUniformMatrix4fv( mvpLoc, 1, mvp.AsFloat() );
  }
  GLint colorLoc = gDebugProgram->GetUniformLocation(Program::UNIFORM_COLOR);
  if( colorLoc != Program::UNIFORM_UNKNOWN )
  {
    gDebugProgram->SetUniform4f( colorLoc, 0.0f, 1.0f, 1.0f, 1.0f );
  }

  context.DrawArrays(GL_LINES, 0, numPoints);

  context.DisableVertexAttributeArray( positionLoc );
#endif
*/
}


} // SceneGraph
} // Internal
} // Dali
