#ifndef __DALI_ANIMATABLE_VERTEX__H__
#define __DALI_ANIMATABLE_VERTEX__H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

namespace Dali DALI_IMPORT_API
{
class AnimatableMesh;

namespace Internal DALI_INTERNAL
{
class AnimatableMesh;
}

/**
 * @brief Represents a vertex in an AnimatableMesh.
 *
 * It is used by AnimatableMesh to offer an array interface for
 * setting properties:
 *
 * @code
 * AnimatableMesh mesh = AnimatableMesh(numVerts, faces);
 * mesh[vertex].SetColor(color);
 * @endcode
 */
class AnimatableVertex
{
public:

  // Default Properties
  static const Property::Index POSITION;        ///< Property 0, name prefix "position-",        type VECTOR3
  static const Property::Index COLOR;           ///< Property 1, name prefix "color-",           type VECTOR4
  static const Property::Index TEXTURE_COORDS;  ///< Property 2, name prefix "texture-coords-",  type VECTOR2

  /**
   * @brief Destructor
   */
  ~AnimatableVertex();

  /**
   * @brief Set the position of the vertex.
   *
   * @param[in] position (note, -0.5 - +0.5 are inside the actor's boundaries)
   */
  void SetPosition(const Vector3& position);

  /**
   * @brief Set the vertex color.
   * @param[in] color The vertex color
   */
  void SetColor(const Vector4& color);

  /**
   * @brief Set the texture coordinates.
   *
   * @param[in] textureCoords The texture coordinates
   */
  void SetTextureCoords(const Vector2& textureCoords);

  /**
   * @brief Get the current position of the vertex.
   *
   * @return position
   */
  Vector3 GetCurrentPosition();

  /**
   * @brief Get the current vertex color.
   *
   * @return The vertex color
   */
  Vector4 GetCurrentColor();

  /**
   * @brief Get the current texture coordinates.
   *
   * @return textureCoords The texture coordinates
   */
  Vector2 GetCurrentTextureCoords();

private:
  // Only allow AnimatableMesh to construct this object.
  friend class Dali::AnimatableMesh;

  /**
   * @brief Constructor.
   *
   * @param[in] vertex The index of the vertex in the parent mesh
   * @param[in] mesh   The parent mesh
   */
  AnimatableVertex( unsigned int vertex, AnimatableMesh mesh );

  /**
   * @brief Undefined Copy Constructor
   */
  AnimatableVertex(const AnimatableVertex& vertex);

  /**
   * @brief Undefined Assignment Operator
   */
  AnimatableVertex& operator=(const AnimatableVertex& vertex);

  int mVertex; ///< The index of this vertex in the parent mesh
  Internal::AnimatableMesh& mMesh;  ///< The parent mesh
};


}//Dali

#endif
