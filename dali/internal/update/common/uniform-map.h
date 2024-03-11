#ifndef DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H
#define DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <string>
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/devel-api/common/hash.h>
#include <dali/devel-api/common/owner-container.h>

namespace Dali
{
namespace Internal
{
class PropertyInputImpl;

namespace SceneGraph
{

/**
 * The uniform map is used to map a uniform name to a property value.
 */
class UniformPropertyMapping
{
public:
  using Hash = unsigned long;

  /**
   * Constructor
   */
  UniformPropertyMapping( const std::string& theUniformName, const PropertyInputImpl* thePropertyPtr )
  : propertyPtr( thePropertyPtr ),
    uniformNameHashNoArray( 0 )
  {
    arrayIndex = 0;

    // Look for array index closing bracket
    auto pos = theUniformName.rfind( "]" );

    // If found, extract the array index and store it
    if( pos != std::string::npos )
    {
      auto pos0 = theUniformName.rfind( "[", pos );
      arrayIndex = atoi( theUniformName.c_str() + pos0 + 1 );
      // Calculate hash from name without array index
      uniformNameHashNoArray = Dali::CalculateHash( theUniformName.substr( 0, pos0 ) );
    }
    uniformName = theUniformName;
    uniformNameHash = Dali::CalculateHash( theUniformName );
  }

  UniformPropertyMapping()
    : propertyPtr( NULL ),
      uniformNameHash( 0 ),
      uniformNameHashNoArray( 0 ),
      arrayIndex( 0 )
  {
  }

  const PropertyInputImpl* propertyPtr;
  std::string uniformName;
  Hash uniformNameHash;
  Hash uniformNameHashNoArray;
  int32_t arrayIndex;
};

/**
 * The UniformMap class is used to map uniform names to property values. It is available
 * in all of the classes responsible for rendering:
 * Actor, Renderer, Shader.
 *
 * It can be observed for changes to the mapping table.
 */
class UniformMap
{
public:
  using SizeType = uint32_t;

  class Observer
  {
  public:

    /**
     * Inform observer that uniform mappings have been changed
     * @param mappings
     */
    virtual void UniformMappingsChanged(const UniformMap& mappings) = 0;

  protected:

    /**
     * Virtual destructor, no deletion through this interface
     */
    virtual ~Observer() {}
  };

  /**
   * Constructor
   */
  UniformMap();

  /**
   * Destructor
   */
  ~UniformMap();

  /**
   * Add a map to the mappings table.
   */
  void Add( UniformPropertyMapping* map );

  /**
   * Remove a map from the mappings table
   */
  void Remove( const std::string& uniformName );

  /**
   * Find a property given the uniform name.
   * @return The address of the property if it's in the map, or NULL otherwise.
   */
  const PropertyInputImpl* Find( const std::string& uniformName );

  /**
   * Get the count of uniforms in the map
   * @return The number of uniform mappings
   */
  SizeType Count() const;

  /**
   * @pre index must be in the range 0 :: Count()-1
   * @param[in] index The index of the element to fetch
   * @return reference to the element in the map
   */
  const UniformPropertyMapping& operator[]( SizeType index ) const;

  /**
   * Return the change counter
   */
  inline std::size_t GetChangeCounter() const
  {
    return mChangeCounter;
  }

private:
  void MappingChanged();

private:
  typedef OwnerContainer< UniformPropertyMapping* > UniformMapContainer;

  UniformMapContainer mUniformMaps;       ///< container of uniform maps
  std::size_t         mChangeCounter{0u}; ///< Counter that is incremented when the map changes
};


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H
