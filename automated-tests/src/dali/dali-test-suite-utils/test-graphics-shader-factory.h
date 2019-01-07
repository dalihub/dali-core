#ifndef TEST_GRAPHICS_SHADER_FACTORY_H
#define TEST_GRAPHICS_SHADER_FACTORY_H

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
 */

#include <dali/graphics-api/graphics-api-shader-factory.h>

namespace Test
{

class GraphicsShaderFactory : public Dali::Graphics::ShaderFactory
{
public:
  GraphicsShaderFactory();
  ~GraphicsShaderFactory();

  Dali::Graphics::ShaderFactory& SetShaderModule( Dali::Graphics::ShaderDetails::PipelineStage pipelineStage,
                                                       Dali::Graphics::ShaderDetails::Language language,
                                                       const Dali::Graphics::ShaderDetails::ShaderSource& source ) override;

  Dali::Graphics::ShaderFactory::PointerType Create() const override;
};

} // Test

#endif //TEST_GRAPHICS_SHADER_FACTORY_H
