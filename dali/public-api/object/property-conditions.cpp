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

// CLASS HEADER
#include <dali/public-api/object/property-conditions.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/property-value.h>
#include <dali/internal/event/common/property-conditions-impl.h>

namespace Dali
{

PropertyCondition::PropertyCondition()
: BaseHandle(new Internal::PropertyCondition())
{

}

PropertyCondition::~PropertyCondition()
{

}

PropertyCondition::ArgumentContainer PropertyCondition::GetArguments()
{
  return GetImplementation(*this).arguments;
}

const PropertyCondition::ArgumentContainer& PropertyCondition::GetArguments() const
{
  return GetImplementation(*this).arguments;
}

PropertyCondition LessThanCondition(float arg)
{
  PropertyCondition condition;
  GetImplementation(condition).type = Internal::PropertyCondition::LessThan;
  GetImplementation(condition).arguments.push_back(Property::Value(arg));

  return condition;
}

PropertyCondition GreaterThanCondition(float arg)
{
  PropertyCondition condition;
  GetImplementation(condition).type = Internal::PropertyCondition::GreaterThan;
  GetImplementation(condition).arguments.push_back(Property::Value(arg));

  return condition;
}

PropertyCondition InsideCondition(float arg0, float arg1)
{
  PropertyCondition condition;
  GetImplementation(condition).type = Internal::PropertyCondition::Inside;
  GetImplementation(condition).arguments.push_back(Property::Value(arg0));
  GetImplementation(condition).arguments.push_back(Property::Value(arg1));

  return condition;
}

PropertyCondition OutsideCondition(float arg0, float arg1)
{
  PropertyCondition condition;
  GetImplementation(condition).type = Internal::PropertyCondition::Outside;
  GetImplementation(condition).arguments.push_back(Property::Value(arg0));
  GetImplementation(condition).arguments.push_back(Property::Value(arg1));

  return condition;
}

PropertyCondition StepCondition(float stepAmount, float referenceValue)
{
  PropertyCondition condition;
  GetImplementation(condition).type = Internal::PropertyCondition::Step;
  GetImplementation(condition).arguments.push_back(Property::Value(referenceValue));
  GetImplementation(condition).arguments.push_back(Property::Value(1.0f / stepAmount));
  GetImplementation(condition).arguments.push_back(Property::Value(0.0f)); // current step

  return condition;
}

PropertyCondition VariableStepCondition(const std::vector<float>& stepAmount)
{
  PropertyCondition condition;
  GetImplementation(condition).type = Internal::PropertyCondition::VariableStep;
  GetImplementation(condition).arguments.push_back(Property::Value(0.0f)); // current step
  int size = stepAmount.size();
  GetImplementation(condition).arguments.push_back(Property::Value(static_cast<float>(size))); // store number of steps
  for( std::vector<float>::const_iterator it = stepAmount.begin(), endIt = stepAmount.end(); it != endIt; ++it )
  {
    GetImplementation(condition).arguments.push_back(Property::Value( *it ));
  }

  return condition;
}

} // namespace Dali
