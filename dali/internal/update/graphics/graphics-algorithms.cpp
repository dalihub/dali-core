/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/graphics/graphics-algorithms.h>

// EXTERNAL INCLUDES

#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics-api/graphics-api-frame.h>
#include <dali/graphics-api/graphics-api-render-command.h>

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/common/render-instruction.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
void SubmitRenderItemList( Graphics::API::Controller&           graphics,
                           Graphics::API::Frame&                frame,
                           BufferIndex                          bufferIndex,
                           Graphics::API::RenderCommandBuilder& commandBuilder,
                           Matrix                               viewProjection,
                           const RenderList&                    renderItemList )
{
  // TODO: @todo: share pipelines accross render lists
  auto pipelineBuilder = graphics.GetPipelineBuilder();
  commandBuilder.Set( pipelineBuilder.Build() );

  // TODO: @todo Clipping...

  auto numberOfRenderItems = renderItemList.Count();

  using DataT = struct
  {
    Matrix  world;
    Vector4 color;
    Vector3 size;
  } __attribute__((aligned(16));
  // TODO: @todo
  //auto uniformBuffer = graphics.CreateBuffer<DataT>( numberOfRenderItems );
  auto uniformBuffer = std::vector<DataT>( numberOfRenderItems );
  for( auto i = 0u; i < numberOfRenderItems; ++i )
  {
    auto& item = renderItemList.GetItem( i );
    Matrix::Multiply( uniformBuffer[i].world, item.mModelMatrix, viewProjection );
    uniformBuffer[i].color = item.mNode->GetWorldColor( bufferIndex );
    uniformBuffer[i].size  = item.mSize;
  }
  commandBuilder.Set( Graphics::API::PrimitiveCount{numberOfRenderItems} );
  commandBuilder.Set( Graphics::API::BufferList{std::move( uniformBuffer )} );
  frame.AddCommand( commandBuilder );
}

void SubmitInstruction( Graphics::API::Controller& graphics,
                        Graphics::API::Frame&      frame,
                        BufferIndex                bufferIndex,
                        RenderInstruction&         instruction )
{
  using namespace Graphics::API;

  // Create constant buffer with static uniforms: view matrix, progrjection matrix

  // TODO: @todo: buffer for constant uniforms
  /*
  auto contantUniforms = BuildBuffer<ProjectionMatrix, ViewMatrix>();
  constantUniforms["uProjection"] = projectionMatrix;
  constantUniforms["uViewMatrix"] = viewMatrix;
  */

  const Matrix* viewMatrix       = instruction.GetViewMatrix( bufferIndex );
  const Matrix* projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );
  Matrix        viewProjection;
  Matrix::Multiply( viewProjection.viewMatrix, projectionMatrix );

  auto commandBuilder = RenderCommandBuilder{};

  auto numberOfRenderLists = instruction.RenderListCount();
  for( auto i = 0u; i < numberOfRenderLists; ++i )
  {
    SubmitRenderItemList(
      graphics, frame, bufferIndex, commandBuilder, viewProjection, *instruction.GetRenderList( i ) );
  }
}
} // namespace

void SubmitRenderInstructions( Graphics::API::Controller&  graphics,
                               RenderInstructionContainer& renderInstructions,
                               BufferIndex                 bufferIndex )
{
  auto frame = graphics.CreateFrame();

  auto numberOfInstructions = renderInstructions.Count();
  for( size_t i = 0; i < numberOfInstructions; ++i )
  {
    RenderInstruction& instruction = renderInstructions.At( bufferIndex, i );

    SubmitInstruction( graphics, frame, bufferIndex, instruction );
  }

  graphics.SubmitFrame( frame );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
