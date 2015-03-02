#ifndef DALI_RENDERER_H
#define DALI_RENDERER_H

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
 *
 */

// EXTERNAL INCLUDES
#include <string> // std::string

// INTERNAL INCLUDES
#include <dali/public-api/geometry/geometry.h> // Dali::Geometry
#include <dali/public-api/object/handle.h> // Dali::Handle
#include <dali/public-api/object/property-index-ranges.h> // DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX
#include <dali/public-api/shader-effects/material.h> // Dali::Material

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Renderer;
}

/**
 * @brief Renderer is a handle to an object that can be used to provide an image to a material.
 */
class DALI_IMPORT_API Renderer : public Handle
{
public:

  /**
   * @brief An enumeration of properties belonging to the Renderer class.
   */
  struct Property
  {
    enum
    {
      DEPTH_INDEX = DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX,  ///< name "depth-index",  type INTEGER
    };
  };

  /**
   * @brief Creates a new Renderer object
   *
   * @param[in] geometry Geometry to be used by this renderer
   * @param[in] material Material to be used by this renderer
   */
  static Renderer New( Geometry geometry, Material material );

  /**
   * @brief Default constructor, creates an empty handle
   */
  Renderer();

  /**
   * @brief Destructor
   */
  ~Renderer();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @param[in] handle Handle to an object
   */
  Renderer( const Renderer& handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Renderer& operator=( const Renderer& handle );

  /**
   * @brief Sets the geometry to be used by this renderer
   *
   * @param[in] geometry The geometry to be used by this renderer
   */
  void SetGeometry( Geometry geometry );

  /**
   * @brief Sets the material to be used by this renderer
   *
   * @param[in] material The material to be used by this renderer
   */
  void SetMaterial( Material material );

  /**
   * @brief Set the depth index of this renderer
   *
   * Renderer with higher depth indices are rendered in front of other with smaller values
   *
   * @param[in] depthIndex The depth index of this renderer
   */
  void SetDepthIndex( int depthIndex );

public:
  /**
   * @brief The constructor
   *
   * @param [in] pointer A pointer to a newly allocated Renderer
   */
  explicit DALI_INTERNAL Renderer( Internal::Renderer* pointer );
};

} //namespace Dali

#endif // DALI_RENDERER_H
