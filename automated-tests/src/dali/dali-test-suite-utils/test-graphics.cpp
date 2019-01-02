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

#include "test-graphics.h"
#include "test-graphics-controller.h"

namespace Test
{

struct Graphics::Impl
{
public:
  GraphicsController controller;
};



Graphics::Graphics( const Dali::Integration::Graphics::GraphicsCreateInfo& info,
                    Dali::Integration::DepthBufferAvailable depthBufferAvailable,
                    Dali::Integration::StencilBufferAvailable stencilBufferRequired )
: GraphicsInterface( info, depthBufferAvailable, stencilBufferRequired ),
  mImpl(new Graphics::Impl() )
{
}

Graphics::~Graphics() = default;

void Graphics::Initialize()
{
}

void Graphics::Create()
{
}

void Graphics::Destroy()
{
}

void Graphics::Pause()
{
}

void Graphics::Resume()
{
}

void Graphics::PreRender()
{
}

void Graphics::PostRender()
{
}

Dali::Graphics::API::Controller& Graphics::GetController()
{
  return mImpl->controller;
}

void Graphics::SurfaceResized( unsigned int width, unsigned int height )
{
}


} // Namespace Test
