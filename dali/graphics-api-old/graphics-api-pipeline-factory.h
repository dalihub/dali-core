#ifndef DALI_GRAPHICS_API_PIPELINE_FACTORY_H
#define DALI_GRAPHICS_API_PIPELINE_FACTORY_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-api-base-factory.h>
#include <dali/graphics-api/graphics-api-pipeline.h>

namespace Dali
{
namespace Graphics
{

/**
 * @brief Interface class for generating Pipeline types in the graphics API.
 */
class PipelineFactory
{
public:

  // not copyable
  PipelineFactory(const PipelineFactory&) = delete;
  PipelineFactory& operator=(const PipelineFactory&) = delete;

  virtual ~PipelineFactory() = default;

  virtual PipelineFactory& SetColorBlendState( const ColorBlendState& state ) = 0;

  virtual PipelineFactory& SetShaderState( const ShaderState& state ) = 0;

  virtual PipelineFactory& SetViewportState( const ViewportState& state ) = 0;

  virtual PipelineFactory& SetFramebufferState( const FramebufferState& state ) = 0;

  virtual PipelineFactory& SetBasePipeline( Pipeline& pipeline ) = 0;

  virtual PipelineFactory& SetDepthStencilState( DepthStencilState state ) = 0;

  virtual PipelineFactory& SetRasterizationState( const RasterizationState& state ) = 0;

  virtual PipelineFactory& SetVertexInputState( const VertexInputState& state ) = 0;

  virtual PipelineFactory& SetInputAssemblyState( const InputAssemblyState& state ) = 0;

  virtual PipelineFactory& SetDynamicStateMask( const PipelineDynamicStateMask mask ) = 0;

  virtual PipelineFactory& SetOldPipeline( std::unique_ptr<Pipeline> oldPipeline ) = 0;

  virtual std::unique_ptr<Pipeline> Create() = 0;

protected:
  /// @brief default constructor
  PipelineFactory() = default;

  PipelineFactory(PipelineFactory&&) = default;
  PipelineFactory& operator=(PipelineFactory&&) = default;
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_PIPELINE_FACTORY_H
