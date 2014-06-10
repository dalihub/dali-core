#ifndef __DALI_INTERNAL_MODEL_LOGGER_H__
#define __DALI_INTERNAL_MODEL_LOGGER_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include "model-impl.h"

namespace Dali
{
class Entity;

namespace Internal
{
class ModelData;

/**
 * This class writes the model data to standard output.
 */
class ModelLogger
{
public:
  ModelLogger(ModelData& model) : mModel(model), mWriteVertices(false) {}
  ~ModelLogger() {}

  /**
   * Write the mesh data. Note that vertex data will only be written
   * if the mWriteVertices attribute is set to true.
   */
  void WriteMeshes();

  /**
   * Write the material data.
   */
  void WriteMaterials();

  /**
   * Write the lighting data.
   */
  void WriteLights();

  /**
   * Write an entity and all its children.
   * @param[in] entity - the entity to write.
   * @param[in] indentLevel - the current indent level to use.
   */
  void WriteEntity(Dali::Entity entity, int indentLevel);

  /**
   * write the animation data.
   */
  void WriteAnimations();

  /**
   * Write the model.
   */
  void Write();

private:
  ModelData& mModel; ///< The model data to write

public:
  bool   mWriteVertices; ///< Set to true if the vertex data should be written
}; // class ModelLogger

} // namespace Internal

} // namespace Dali

#endif // ndef __DALI_INTERNAL_MODEL_LOGGER_H
