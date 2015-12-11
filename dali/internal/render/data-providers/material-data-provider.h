#ifndef DALI_INTERNAL_SCENE_GRAPH_MATERIAL_DATA_PROVIDER_H
#define DALI_INTERNAL_SCENE_GRAPH_MATERIAL_DATA_PROVIDER_H
/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/internal/common/buffer-index.h>
#include <dali/devel-api/rendering/material.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/actors/blending.h>

namespace Dali
{
namespace Internal
{
struct BlendingOptions;

namespace SceneGraph
{

/**
 * Interface to provide data of the material to the renderer.
 * This interface must not be used to pass object pointers.
 */
class MaterialDataProvider
{
public:
  /**
   * Construtor
   */
  MaterialDataProvider()
  {
  }

  /**
   * Get the blend color
   * @return blend color of NULL if blend color is not set
   */
  virtual Vector4* GetBlendColor() const = 0;

  /**
   * Get the blending options
   * @return the blending options
   */
  virtual const BlendingOptions& GetBlendingOptions() const = 0;

  /**
   * Get the cull face mode
   * @return the cull face mode
   */
  virtual Dali::Material::FaceCullingMode GetFaceCullingMode() const = 0;

protected:
  /**
   * Destructor. No deletion through this interface
   */
  virtual ~MaterialDataProvider()
  {
  }
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_MATERIAL_DATA_PROVIDER_H
