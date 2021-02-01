#ifndef DALI_INTERNAL_CONSTRAINT_SOURCE_H
#define DALI_INTERNAL_CONSTRAINT_SOURCE_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/object-impl.h>
#include <dali/public-api/animation/constraint-source.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{
namespace Internal
{
struct Source;
using SourceContainer = std::vector<Source>;
using SourceIter      = SourceContainer::iterator;

/**
 * The source of an input property for a constraint.
 */
struct Source
{
  /**
   * Default constructor
   */
  Source()
  : sourceType(OBJECT_PROPERTY),
    propertyIndex(Property::INVALID_INDEX),
    object(nullptr)
  {
  }

  /**
   * Create a constraint source from a public handle.
   * The internal object is not referenced by the Internal::Source; therefore
   * the owner of this object is responsible for observing the Object's lifetime.
   */
  Source(Dali::ConstraintSource& source)
  : sourceType(source.sourceType),
    propertyIndex(source.propertyIndex),
    object(nullptr)
  {
    if(source.object)
    {
      object = &dynamic_cast<Object&>(GetImplementation(source.object));
    }
  }

  SourceType sourceType; ///< The source type

  Property::Index propertyIndex; ///< The index of the source property

  Object* object; ///< The target object; only valid if sourceType == OBJECT_PROPERTY
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_CONSTRAINT_SOURCE_H
