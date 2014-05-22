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

// INTERNAL INCLUDES
#include <dali/public-api/object/property.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/render/shaders/shader.h>

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

  typedef Dali::ShaderEffect::UniformCoordinateType CoordinateType;

  /**
   * Create a UniformMeta.
   */
  static UniformMeta* New( const std::string& name, const PropertyBase& property, CoordinateType coordType )
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
  void SetCoordinateType( CoordinateType coordType )
  {
    coordinateType = coordType;
  }

private:

  /**
   * Constructor
   */
  UniformMeta( const std::string& uniformName, const PropertyBase& prop, CoordinateType coordType )
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

  CoordinateType coordinateType; ///< The coordinate type of the uniform
};

// Messages for UniformMeta

}; // namespace SceneGraph

// value types used by messages
template <> struct ParameterType< SceneGraph::UniformMeta::CoordinateType >
: public BasicType< SceneGraph::UniformMeta::CoordinateType > {};

namespace SceneGraph
{

inline void SetCoordinateTypeMessage( EventToUpdate& eventToUpdate, const UniformMeta& meta, UniformMeta::CoordinateType type )
{
  typedef MessageValue1< UniformMeta, UniformMeta::CoordinateType > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &meta, &UniformMeta::SetCoordinateType, type );
}

} // namespace SceneGraph

/**
 * Used for accessing uniform metadata by property index
 */
typedef std::map<Property::Index, const SceneGraph::UniformMeta*> CustomUniformMetaLookup;

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UNIFORM_META_H__

