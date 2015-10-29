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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/actors/blending.h>

namespace Dali
{
namespace Internal
{
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

  virtual const Vector4& GetBlendColor( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the RGB source factor
   * @return the RGB source factor
   */
  virtual BlendingFactor::Type GetBlendSrcFactorRgb( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the Alpha source factor
   * @return the Alpha source factor
   */
  virtual BlendingFactor::Type GetBlendSrcFactorAlpha( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the RGB destination factor
   * @return the RGB destination factor
   */
  virtual BlendingFactor::Type GetBlendDestFactorRgb( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the Alpha destination factor
   * @return the Alpha destination factor
   */
  virtual BlendingFactor::Type GetBlendDestFactorAlpha( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the RGB blending equation
   * @return the RGB blending equation
   */
  virtual BlendingEquation::Type GetBlendEquationRgb( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the Alpha blending equation
   * @return the Alpha blending equation
   */
  virtual BlendingEquation::Type GetBlendEquationAlpha( BufferIndex bufferIndex ) const = 0;

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
