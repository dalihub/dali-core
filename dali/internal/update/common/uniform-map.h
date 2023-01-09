#ifndef DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H
#define DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <cstdint> // uint32_t
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

#include <dali/devel-api/common/hash.h>

#include <dali/internal/common/const-string.h>

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
  UniformPropertyMapping(ConstString theUniformName, const PropertyInputImpl* thePropertyPtr)
  : propertyPtr(thePropertyPtr),
    uniformName(theUniformName),
    uniformNameHash(0u),
    uniformNameHashNoArray(0u),
    arrayIndex(0u)
  {
    // Look for array index closing bracket
    auto nameStringView = theUniformName.GetStringView();
    auto pos            = nameStringView.rfind("]");

    // If found, extract the array index and store it, if it's an element in an array of basic types.
    if(pos != std::string::npos)
    {
      auto pos0 = theUniformName.GetStringView().rfind("[", pos);
      if(pos == nameStringView.length() - 1) // if element is in struct, don't set array index.
      {
        arrayIndex = atoi(theUniformName.GetCString() + pos0 + 1);
      }
      // Calculate hash from name without array index
      uniformNameHashNoArray = Dali::CalculateHash(theUniformName.GetStringView().substr(0, pos0).data(), '[');
    }
    uniformName     = theUniformName;
    uniformNameHash = Dali::CalculateHash(theUniformName.GetCString());
  }

  UniformPropertyMapping() = default;

  const PropertyInputImpl* propertyPtr{nullptr};
  ConstString              uniformName{};
  Hash                     uniformNameHash{0u};
  Hash                     uniformNameHashNoArray{0u};
  int32_t                  arrayIndex{0u};
};

/**
 * The UniformMap class is used to map uniform names to property values. It is available
 * in the following rendering classes: Node, Renderer, Shader.
 *
 * They can test the ChangeCounter to see if the mapping has been updated.
 */
class UniformMap
{
public:
  using SizeType = uint32_t;

  /**
   * Add a map to the mappings table.
   */
  void Add(UniformPropertyMapping map);

  /**
   * Remove a map from the mappings table
   */
  void Remove(ConstString uniformName);

  /**
   * Find a property given the uniform name.
   * @return The address of the property if it's in the map, or NULL otherwise.
   */
  const PropertyInputImpl* Find(ConstString uniformName);

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
  const UniformPropertyMapping& operator[](SizeType index) const;

  /**
   * Return the change counter
   */
  inline std::size_t GetChangeCounter() const
  {
    return mChangeCounter;
  }

private:
  /**
   * Helper to call the observers when the mappings have changed
   */
  void MappingChanged();

private:
  using UniformMapContainer = Dali::Vector<UniformPropertyMapping>;
  using UniformMapIter      = UniformMapContainer::Iterator;

  UniformMapContainer mUniformMaps;       ///< container of uniform maps
  std::size_t         mChangeCounter{0u}; ///< Counter that is incremented when the map changes
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H
