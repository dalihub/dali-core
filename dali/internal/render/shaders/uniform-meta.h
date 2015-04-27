#ifndef __DALI_INTERNAL_UNIFORM_META_H__
#define __DALI_INTERNAL_UNIFORM_META_H__

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

// EXTERNAL INCLUDES
#include <string>
#include <cstring> // for memcpy & memset

// INTERNAL INCLUDES
#include <dali/public-api/math/compile-time-math.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/internal/common/message.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class PropertyBase;

/**
 * Holds the metadata associated with a Shader uniform.
 */
class UniformMeta
{
public:

  /**
   * Create a UniformMeta.
   */
  static UniformMeta* New( const std::string& name, const PropertyBase& property, Dali::ShaderEffect::UniformCoordinateType coordType )
  {
    return new UniformMeta( name, property, coordType );
  }

  /**
   * Copy constructor
   * @param [in] meta The UniformMeta to copy.
   */
  UniformMeta( const UniformMeta& meta )
  : name( meta.name ),
    property( meta.property ),
    coordinateType( meta.coordinateType )
  {
    memcpy( cacheIndeces, meta.cacheIndeces, sizeof(cacheIndeces) );
  }

  /**
   * Set the coordinate type.
   * @param [in] coordType The new coordinate type.
   */
  void SetCoordinateType( Dali::ShaderEffect::UniformCoordinateType coordType )
  {
    coordinateType = coordType;
  }

private:

  /**
   * Constructor
   */
  UniformMeta( const std::string& uniformName, const PropertyBase& prop, Dali::ShaderEffect::UniformCoordinateType coordType )
  : name( uniformName ),
    property( prop ),
    coordinateType( coordType )
  {
    memset( cacheIndeces, 0, sizeof(cacheIndeces) );
  }

  // Undefined
  UniformMeta& operator=( const UniformMeta& rhs );

public:

  std::string name; ///< name of uniform to set/animate
  const PropertyBase& property; ///< reference to the corresponding property
  unsigned int cacheIndeces[ Log<GEOMETRY_TYPE_LAST>::value ][ SHADER_SUBTYPE_LAST ]; ///< internal program cache index, per program
  Dali::ShaderEffect::UniformCoordinateType coordinateType; ///< The coordinate type of the uniform

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UNIFORM_META_H__
