#ifndef __DALI_INTERNAL_PROPERTY_CONDITIONS_H__
#define __DALI_INTERNAL_PROPERTY_CONDITIONS_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-conditions.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property-value.h>

namespace Dali
{

namespace Internal
{

/**
 * Structure to contain PropertyCondition internal data
 */

class PropertyCondition : public Dali::BaseObject
{

public:

  /**
   * Condition types.
   */
  enum Type
  {
    False,                ///< Result Always False
    LessThan,             ///< Magnitude of type is less than float value (arg0).
    GreaterThan,          ///< Magnitude of type is greater than float value (arg0).
    Inside,               ///< Magnitude of type is within float values (arg0 & arg1).
    Outside,              ///< Magnitude of type is outside float values (arg0 & arg1).
    Step,                 ///< Value of type has crossed a step amount
    VariableStep          ///< Similar to step, except user can define a list of steps from reference value
  };

  typedef Dali::PropertyCondition::ArgumentContainer ArgumentContainer;
  typedef Dali::PropertyCondition::ArgumentIter ArgumentIter;
  typedef Dali::PropertyCondition::ArgumentConstIter ArgumentConstIter;

  PropertyCondition();

  virtual ~PropertyCondition();

private:

private:

  // Not implemented
  PropertyCondition(const PropertyCondition& rhs);

  // Not implemented
  PropertyCondition& operator=(const PropertyCondition& rhs);

public:

  Type type;                        ///< The condition Type.
  ArgumentContainer arguments;      ///< The condition Arguments.

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::PropertyCondition& GetImplementation(Dali::PropertyCondition& pub)
{
  DALI_ASSERT_ALWAYS( pub && "PropertyCondition handle is empty" );

  BaseObject& handle = pub.GetBaseObject();

  return static_cast<Internal::PropertyCondition&>(handle);
}

inline const Internal::PropertyCondition& GetImplementation(const Dali::PropertyCondition& pub)
{
  DALI_ASSERT_ALWAYS( pub && "PropertyCondition handle is empty" );

  const BaseObject& handle = pub.GetBaseObject();

  return static_cast<const Internal::PropertyCondition&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_PROPERTY_CONDITIONS_H__
