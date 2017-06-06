#ifndef DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H
#define DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H

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

#include <dali/devel-api/common/hash.h>
#include <dali/devel-api/common/owner-container.h>
#include <string>

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
  typedef unsigned long Hash;

  /**
   * Constructor
   */
  UniformPropertyMapping( const std::string& theUniformName, const PropertyInputImpl* thePropertyPtr )
  : propertyPtr( thePropertyPtr ),
    uniformName( theUniformName ),
    uniformNameHash( Dali::CalculateHash( theUniformName ) )
  {
  }

  UniformPropertyMapping()
  : propertyPtr( NULL ),
    uniformName( "" ),
    uniformNameHash( 0 )
  {
  }


  const PropertyInputImpl* propertyPtr;
  std::string uniformName;
  Hash uniformNameHash;
};

/**
 * The UniformMap class is used to map uniform names to property values. It is available
 * in all of the classes responsible for rendering:
 * Actor, Renderer, Geometry, TextureSet, Shader.
 *
 * It can be observed for changes to the mapping table.
 */
class UniformMap
{
public:
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
   * Add an observer that watches for changes in the mappings
   */
  void AddObserver( Observer& observer );

  /**
   * Remove an observer
   */
  void RemoveObserver( Observer& observer );

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
  unsigned int Count() const;

  /**
   * @pre index must be in the range 0 :: Count()-1
   * @param[in] index The index of the element to fetch
   * @return reference to the element in the map
   */
  const UniformPropertyMapping& operator[]( unsigned int index ) const;

private:
  /**
   * Helper to call the observers when the mappings have changed
   */
  void MappingChanged();

private:
  typedef OwnerContainer< UniformPropertyMapping* > UniformMapContainer;
  typedef UniformMapContainer::Iterator UniformMapIter;
  typedef Dali::Vector< Observer* > Observers;
  typedef Observers::Iterator ObserversIter;

  UniformMapContainer mUniformMaps; // Owner container of uniform maps

  Observers mObservers;
};


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H
