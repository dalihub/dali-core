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

#ifndef DALI_INTEGRATION_GRAPHICS_SURFACE_FACTORY_H
#define DALI_INTEGRATION_GRAPHICS_SURFACE_FACTORY_H

namespace Dali
{
namespace Integration
{
namespace Graphics
{

class SurfaceFactory
{
public:
  SurfaceFactory()          = default;
  virtual ~SurfaceFactory() = default;

  SurfaceFactory( const SurfaceFactory& ) = delete;
  SurfaceFactory& operator=( const SurfaceFactory& ) = delete;
};

} // Namespace Graphics
} // Namespace Integration
} // Namespace Dali

#endif // DALI_INTEGRATION_GRAPHICS_SURFACE_FACTORY_H
