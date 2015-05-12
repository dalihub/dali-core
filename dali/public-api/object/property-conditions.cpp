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

PropertyCondition::PropertyCondition( const PropertyCondition& handle )
: BaseHandle(handle)
{
}

PropertyCondition& PropertyCondition::operator=( const PropertyCondition& rhs )
{
  BaseHandle::operator=(rhs);
  return *this;
}

std::size_t PropertyCondition::GetArgumentCount() const
{
  return GetImplementation(*this).arguments.Count();
}

float PropertyCondition::GetArgument( std::size_t index ) const
{
  return GetImplementation(*this).arguments[ index ];
}

PropertyCondition LessThanCondition( float arg )
{
  PropertyCondition condition;
  Internal::PropertyCondition& impl = GetImplementation( condition );
  impl.type = Internal::PropertyCondition::LessThan;
  impl.arguments.PushBack( arg );

  return condition;
}

PropertyCondition GreaterThanCondition( float arg )
{
  PropertyCondition condition;
  Internal::PropertyCondition& impl = GetImplementation( condition );
  impl.type = Internal::PropertyCondition::GreaterThan;
  impl.arguments.PushBack( arg );

  return condition;
}

PropertyCondition InsideCondition( float arg0, float arg1 )
{
  PropertyCondition condition;
  Internal::PropertyCondition& impl = GetImplementation( condition );
  impl.type = Internal::PropertyCondition::Inside;
  impl.arguments.PushBack( arg0 );
  impl.arguments.PushBack( arg1 );

  return condition;
}

PropertyCondition OutsideCondition( float arg0, float arg1 )
{
  PropertyCondition condition;
  Internal::PropertyCondition& impl = GetImplementation( condition );
  impl.type = Internal::PropertyCondition::Outside;
  impl.arguments.PushBack( arg0 );
  impl.arguments.PushBack( arg1 );

  return condition;
}

PropertyCondition StepCondition( float stepAmount, float referenceValue )
{
  PropertyCondition condition;
  Internal::PropertyCondition& impl = GetImplementation( condition );
  impl.type = Internal::PropertyCondition::Step;
  impl.arguments.PushBack( referenceValue );
  impl.arguments.PushBack( 1.0f / stepAmount );
  impl.arguments.PushBack( 0.0f ); // current step

  return condition;
}

PropertyCondition VariableStepCondition( const Dali::Vector<float>& stepAmount )
{
  PropertyCondition condition;
  Internal::PropertyCondition& impl = GetImplementation( condition );
  impl.type = Internal::PropertyCondition::VariableStep;
  impl.arguments.PushBack( 0.0f ); // current step
  Dali::Vector<float>::SizeType count = stepAmount.Count();
  impl.arguments.PushBack( float( count ) ); // store number of steps
  for( Dali::Vector<float>::SizeType index = 0; index < count; ++index )
  {
    impl.arguments.PushBack( stepAmount[index] );
  }

  return condition;
}

} // namespace Dali
