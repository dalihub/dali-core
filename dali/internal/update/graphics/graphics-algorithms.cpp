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
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/common/buffer-index.h>


namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
void SubmitInstruction( Graphics::API::Controller& graphics,
                        Graphics::API::Frame&      frame,
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

  auto commandBuilder = RenderCommandBuilder{};

  for( const auto& list : instruction )
  {
    SubmitRenderItemList( graphics, frame, commandBuilder, list );
  }
}

void SubmitRenderItemList( Graphics::API::Controller&           graphics,
                           Graphics::API::Frame&                frame,
                           Graphics::API::RenderCommandBuilder& commandBuilder,
                           const RenderList&                    renderItemList )
{
  // TODO: @todo: share pipelines accross render lists
  auto pipeline = BuildPipeline( graphics );
  commandBuilder.Set( pipeline );

  // TODO: @todo Clipping...

  auto numberOfRenderItems = renderItemList.Count();
  auto uniformBuffer       = BuildBufferOf<Matrix, Color, Size>( graphics, numberOfRenderItems );
  for( auto i = 0u; i < numberOfRenderItems; ++i )
  {
    uniformBuffer[i][0] = Matrix( item.mViewProjection ) * Matrix( item.mNode->GetModelMatrix() );
    uniformBuffer[i][1] = Color( item.mNode->GetRenderColor() );
    uniformBuffer[i][2] = Size( item.mSize );
  }
  commandBuilder.Set( PrimitiveCount(numberOfRenderItems) );
  commandBuilder.Set( BufferList{ {Buffer::UsageHint::UNIFORM, uniformBuffer} } );
  frame.AddCommand(commandBuilder);
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

    SubmitInstruction( graphics, frame, instruction );
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
