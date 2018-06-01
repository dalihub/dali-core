#ifndef DALI_GRAPHICS_API_PIPELINE_FACTORY_H
#define DALI_GRAPHICS_API_PIPELINE_FACTORY_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
namespace API
{
class PipelineCache;

/**
 * @brief Interface class for ShaderFactory types in the graphics API.
 */
class PipelineFactory : public BaseFactory<Pipeline>
{
public:

  // not copyable
  PipelineFactory(const PipelineFactory&) = delete;
  PipelineFactory& operator=(const PipelineFactory&) = delete;

  virtual ~PipelineFactory() = default;

  virtual PipelineFactory& SetPipelineCache( PipelineCache& pipelineCache ) = 0;

  virtual PipelineFactory& SetColorBlendState( const Pipeline::ColorBlendState& state ) = 0;

  virtual PipelineFactory& SetShaderState( const Pipeline::ShaderState& state ) = 0;

  virtual PipelineFactory& SetViewportState( const Pipeline::ViewportState& state ) = 0;

  virtual PipelineFactory& SetBasePipeline( Pipeline& pipeline ) = 0;

  virtual PipelineFactory& SetDepthStencilState( Pipeline::DepthStencilState state ) = 0;

  virtual PipelineFactory& SetRasterizationState( const Pipeline::RasterizationState& state ) = 0;

  virtual PipelineFactory& SetVertexInputState( const Pipeline::VertexInputState& state ) = 0;

  virtual PipelineFactory& SetInputAssemblyState( const Pipeline::InputAssemblyState& state ) = 0;

protected:
  /// @brief default constructor
  PipelineFactory() = default;

  PipelineFactory(PipelineFactory&&) = default;
  PipelineFactory& operator=(PipelineFactory&&) = default;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_PIPELINE_FACTORY_H

