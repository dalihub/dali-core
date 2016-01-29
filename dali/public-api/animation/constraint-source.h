#ifndef __DALI_CONSTRAINT_SOURCE_H__
#define __DALI_CONSTRAINT_SOURCE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property.h>

namespace Dali
{
/**
 * @addtogroup dali_core_animation
 * @{
 */

/**
 * @brief Constraint source types.
 *
 * This specifies the location of a property, which is used as an input for a constraint function.
 * @SINCE_1_0.0
 */
enum SourceType
{
  OBJECT_PROPERTY, ///< The property comes from an arbitrary object. @SINCE_1_0.0
  LOCAL_PROPERTY,  ///< The property comes from the object which the constraint is applied to. @SINCE_1_0.0
  PARENT_PROPERTY  ///< The property comes from the parent of the object, which the constraint is applied to. @SINCE_1_0.0
};


/**
 * @brief Identifies a property from an object.
 * @SINCE_1_0.0
 */
struct DALI_IMPORT_API LocalSource
{
  /**
   * @brief Create a local constraint source.
   *
   * @SINCE_1_0.0
   * @param [in] index The index of a property provided by the constrained object.
   */
  LocalSource( Property::Index index );

  Property::Index propertyIndex; ///< The index of a property provided by the constrained object.
};

/**
 * @brief Identifies a property from the parent of an object.
 * @SINCE_1_0.0
 */
struct DALI_IMPORT_API ParentSource
{
  /**
   * @brief Create a local constraint source.
   *
   * @SINCE_1_0.0
   * @param [in] index The index of a property, provided by the parent of the constrained object.
   */
  ParentSource( Property::Index index );

  Property::Index propertyIndex; ///< The index of a property provided by the parent of the constrained object.
};

/**
 * @brief Identifies a property from any object.
 * @SINCE_1_0.0
 */
struct DALI_IMPORT_API Source
{
  /**
   * @brief Create a constraint source.
   *
   * @SINCE_1_0.0
   * @param [in] object The object providing the property.
   * @param [in] index The index of a property provided by object.
   */
  Source( Handle& object, Property::Index index );

  Property::Index propertyIndex; ///< The index of a property provided by object.

  Handle object; ///< The target object
};

/**
 * @brief The source of an input property for a constraint.
 * @SINCE_1_0.0
 */
struct DALI_IMPORT_API ConstraintSource
{
  /**
   * @brief Create a constraint source.
   *
   * @SINCE_1_0.0
   * @param [in] source A constraint source from an arbitrary object.
   */
  ConstraintSource( Source source );

  /**
   * @brief Create a constraint source.
   *
   * @SINCE_1_0.0
   * @param [in] local A local constraint source.
   */
  ConstraintSource( LocalSource local );

  /**
   * @brief Create a constraint source.
   *
   * @SINCE_1_0.0
   * @param [in] parent A parent constraint source.
   */
  ConstraintSource( ParentSource parent );

  SourceType sourceType; ///< The source type

  Property::Index propertyIndex; ///< The index of the source property

  Handle object; ///< The target object; only valid if sourceType == OBJECT_PROPERTY
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_CONSTRAINT_SOURCE_H__
