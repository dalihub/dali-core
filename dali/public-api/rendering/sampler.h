#ifndef DALI_SAMPLER_H
#define DALI_SAMPLER_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/handle.h>   // Dali::Handle
#include <dali/public-api/actors/sampling.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Sampler;
}

/**
 * @brief Sampler is a handle to an object that can be used to provide the sampling parameters to sample textures
 *
 * @SINCE_1_1.43
 */
class DALI_IMPORT_API Sampler : public BaseHandle
{
public:

  /**
   * @brief Creates a new Sampler object
   *
   * @SINCE_1_1.43
   */
  static Sampler New();

  /**
   * @brief Default constructor, creates an empty handle
   *
   * @SINCE_1_1.43
   */
  Sampler();

  /**
   * @brief Destructor
   *
   * @SINCE_1_1.43
   */
  ~Sampler();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   */
  Sampler( const Sampler& handle );

  /**
   * @brief Downcast to a sampler handle.
   * If not the returned handle is left uninitialized.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Handle or an uninitialized handle
   */
  static Sampler DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Sampler& operator=( const Sampler& handle );


  /**
   * @brief Set the filter modes for this sampler
   * Calling this function sets the properties MINIFICATION_FILTER and MAGNIFICATION_FILTER.
   *
   * @SINCE_1_1.43
   * @param[in] minFilter The minification filter that will be used
   * @param[in] magFilter The magnification filter that will be used
   */
  void SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter );

  /**
   * @brief Set the wrap modes for this sampler
   *
   * @SINCE_1_1.43
   * param[in] uWrap Wrap mode for u coordinates
   * param[in] vWrap Wrap mode for v coordinates
   */
  void SetWrapMode( WrapMode::Type uWrap, WrapMode::Type vWrap );

  /**
   * @brief Set the wrap modes for this sampler
   *
   * @SINCE_1_1.43
   * param[in] rWrap Wrap mode for the z direction
   * param[in] sWrap Wrap mode for the x direction
   * param[in] tWrap Wrap mode for the y direction
   */
  void SetWrapMode( WrapMode::Type rWrap, WrapMode::Type sWrap, WrapMode::Type tWrap );

public:

  /**
   * @brief The constructor
   * @note  Not intended for application developers.
   * @SINCE_1_1.43
   * @param[in] pointer A pointer to a newly allocated Sampler
   */
  explicit DALI_INTERNAL Sampler( Internal::Sampler* pointer );
};

} //namespace Dali

#endif // DALI_SAMPLER_H
