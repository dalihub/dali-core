#ifndef __DALI_BONE_H__
#define __DALI_BONE_H__

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/matrix.h>

namespace Dali DALI_IMPORT_API
{

class Matrix;

class Bone;
typedef std::vector< Bone >           BoneContainer; ///< Container for bones
typedef BoneContainer::iterator       BoneIter;      ///< @ref Dali::BoneContainer iterator
typedef BoneContainer::const_iterator BoneConstIter; ///< @ref Dali::BoneContainer const iterator

/**
 * @brief A single bone in a mesh.
 *
 * A Bone is a named actor that can be used to deform a mesh. @see Dali::MeshData for more
 * information.
 */
class Bone
{
public:
  /**
   * @brief Default constructor.
   */
  Bone ();

  /**
   * @brief Constructor.
   *
   * @param name of the bone
   * @param offsetMatrix for the bone
   */
  Bone( const std::string& name, const Matrix& offsetMatrix );

  /**
   * @brief Destructor.
   */
  ~Bone();

  /**
   * @brief Copy constructor.
   */
  Bone( const Bone& rhs );

  /**
   * @brief Assignment operator.
   */
  Bone& operator=(const Bone& rhs);

  /**
   * @brief Get name.
   *
   * @return returns the name of the bone
   */
  const std::string& GetName() const;

  /**
   * @brief Get offset matrix.
   *
   * @return returns the offset matrix for this bone
   */
   const Matrix& GetOffsetMatrix() const;

private:
  // Attributes
  std::string   mName;         ///< Name of bone
  Matrix        mOffsetMatrix; ///< Transform from mesh space to bone space in bind pose.

}; // struct Bone

} // namespace Dali

#endif // __DALI_BONE_H__
