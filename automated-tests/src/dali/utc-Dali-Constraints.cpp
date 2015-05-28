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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

///////////////////////////////////////////////////////////////////////////////
void utc_dali_constraints_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_constraints_cleanup(void)
{
  test_return_value = TET_PASS;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
namespace
{

struct PropertyInputImpl : public PropertyInput
{
public:

  // Constants
  static const bool BOOLEAN_VALUE;
  static const float FLOAT_VALUE;
  static const int INTEGER_VALUE;
  static const unsigned int UNSIGNED_INTEGER_VALUE;
  static const Vector2 VECTOR2_VALUE;
  static const Vector3 VECTOR3_VALUE;
  static const Vector4 VECTOR4_VALUE;
  static const Matrix3 MATRIX3_VALUE;
  static const Matrix MATRIX_VALUE;
  static const Quaternion QUATERNION_VALUE;

  // Construction & Destruction
  PropertyInputImpl( Property::Type type ) : mType( type ) { }
  virtual ~PropertyInputImpl() { }

  // Methods
  Property::Type GetType() const { return mType; }

  // Virtual Methods
  virtual const bool& GetBoolean() const                 { return BOOLEAN_VALUE;          }
  virtual const float& GetFloat() const                  { return FLOAT_VALUE;            }
  virtual const int& GetInteger() const                  { return INTEGER_VALUE;          }
  virtual const unsigned int& GetUnsignedInteger() const { return UNSIGNED_INTEGER_VALUE; }
  virtual const Vector2& GetVector2() const              { return VECTOR2_VALUE;          }
  virtual const Vector3& GetVector3() const              { return VECTOR3_VALUE;          }
  virtual const Vector4& GetVector4() const              { return VECTOR4_VALUE;          }
  virtual const Matrix3& GetMatrix3() const              { return MATRIX3_VALUE;          }
  virtual const Matrix& GetMatrix() const                { return MATRIX_VALUE;           }
  virtual const Quaternion& GetQuaternion() const        { return QUATERNION_VALUE;       }

  // Data
  Property::Type mType;
};

const bool PropertyInputImpl::BOOLEAN_VALUE                  = true;
const float PropertyInputImpl::FLOAT_VALUE                   = 123.0f;
const int PropertyInputImpl::INTEGER_VALUE                   = 456;
const unsigned int PropertyInputImpl::UNSIGNED_INTEGER_VALUE = 789u;
const Vector2 PropertyInputImpl::VECTOR2_VALUE               = Vector2( 10.0f, 20.0f );
const Vector3 PropertyInputImpl::VECTOR3_VALUE               = Vector3( 30.0f, 40.0f, 50.0f );
const Vector4 PropertyInputImpl::VECTOR4_VALUE               = Vector4( 60.0f, 70.0f, 80.0f, 90.0f );
const Matrix3 PropertyInputImpl::MATRIX3_VALUE               ( 1.0f, 2.0f, 3.0f,
                                                               4.0f, 5.0f, 6.0f,
                                                               7.0f, 8.0f, 9.0f );
const Matrix PropertyInputImpl::MATRIX_VALUE                 = Matrix::IDENTITY;
const Quaternion PropertyInputImpl::QUATERNION_VALUE         ( 1.0f, 2.0f, 3.0f, 4.0f );

struct Vector3PropertyInput : public PropertyInputImpl
{
public:

  // Construction & Destruction
  Vector3PropertyInput( Vector3& value )
  : PropertyInputImpl( Property::VECTOR3 ),
    mValue( value )
  {
  }

  ~Vector3PropertyInput()
  {
  }

  const Vector3& GetVector3() const
  {
    return mValue;
  }

  // Data
  Vector3& mValue;
};

struct QuaternionPropertyInput : public PropertyInputImpl
{
public:

  // Construction & Destruction
  QuaternionPropertyInput( Quaternion& value )
  : PropertyInputImpl( Property::ROTATION ),
    mValue( value )
  {
  }

  ~QuaternionPropertyInput()
  {
  }

  const Quaternion& GetQuaternion() const
  {
    return mValue;
  }

  // Data
  Quaternion& mValue;
};

} // unnamed namespace
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// EqualToConstraint
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintsEqualToConstraintFloat(void)
{
  PropertyInputContainer inputs;
  PropertyInputImpl input( Property::FLOAT );
  inputs.PushBack( &input );

  float value = 0.0f;
  DALI_TEST_CHECK( value != PropertyInputImpl::FLOAT_VALUE );

  EqualToConstraint constraint;
  constraint( value, inputs );

  DALI_TEST_EQUALS( value, PropertyInputImpl::FLOAT_VALUE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintsEqualToConstraintVector2(void)
{
  PropertyInputContainer inputs;
  PropertyInputImpl input( Property::VECTOR2 );
  inputs.PushBack( &input );

  Vector2 value;
  DALI_TEST_CHECK( value != PropertyInputImpl::VECTOR2_VALUE );

  EqualToConstraint constraint;
  constraint( value, inputs );

  DALI_TEST_EQUALS( value, PropertyInputImpl::VECTOR2_VALUE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintsEqualToConstraintVector3(void)
{
  PropertyInputContainer inputs;
  PropertyInputImpl input( Property::VECTOR3 );
  inputs.PushBack( &input );

  Vector3 value;
  DALI_TEST_CHECK( value != PropertyInputImpl::VECTOR3_VALUE );

  EqualToConstraint constraint;
  constraint( value, inputs );

  DALI_TEST_EQUALS( value, PropertyInputImpl::VECTOR3_VALUE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintsEqualToConstraintVector4(void)
{
  PropertyInputContainer inputs;
  PropertyInputImpl input( Property::VECTOR4 );
  inputs.PushBack( &input );

  Vector4 value;
  DALI_TEST_CHECK( value != PropertyInputImpl::VECTOR4_VALUE );

  EqualToConstraint constraint;
  constraint( value, inputs );

  DALI_TEST_EQUALS( value, PropertyInputImpl::VECTOR4_VALUE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintsEqualToConstraintQuaternion(void)
{
  PropertyInputContainer inputs;
  PropertyInputImpl input( Property::ROTATION );
  inputs.PushBack( &input );

  Quaternion value;
  DALI_TEST_CHECK( value != PropertyInputImpl::QUATERNION_VALUE );

  EqualToConstraint constraint;
  constraint( value, inputs );

  DALI_TEST_EQUALS( value, PropertyInputImpl::QUATERNION_VALUE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintsEqualToConstraintMatrix3(void)
{
  PropertyInputContainer inputs;
  PropertyInputImpl input( Property::MATRIX3 );
  inputs.PushBack( &input );

  Matrix3 value;
  DALI_TEST_CHECK( value != PropertyInputImpl::MATRIX3_VALUE );

  EqualToConstraint constraint;
  constraint( value, inputs );

  DALI_TEST_EQUALS( value, PropertyInputImpl::MATRIX3_VALUE, 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliConstraintsEqualToConstraintMatrix(void)
{
  PropertyInputContainer inputs;
  PropertyInputImpl input( Property::MATRIX );
  inputs.PushBack( &input );

  Matrix value;
  DALI_TEST_CHECK( value != PropertyInputImpl::MATRIX_VALUE );

  EqualToConstraint constraint;
  constraint( value, inputs );

  DALI_TEST_EQUALS( value, PropertyInputImpl::MATRIX_VALUE, TEST_LOCATION );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// RelativeToConstraint
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintsRelativeToConstraintUsingFloat(void)
{
  PropertyInputContainer inputs;
  PropertyInputImpl input( Property::VECTOR3 );
  inputs.PushBack( &input );

  Vector3 value;
  DALI_TEST_EQUALS( value, Vector3::ZERO, TEST_LOCATION );

  const float scale( 4.0f );
  RelativeToConstraint constraint( scale );
  constraint( value, inputs );

  DALI_TEST_EQUALS( value, PropertyInputImpl::VECTOR3_VALUE * scale, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintsRelativeToConstraintUsingVector3(void)
{
  PropertyInputContainer inputs;
  PropertyInputImpl input( Property::VECTOR3 );
  inputs.PushBack( &input );

  Vector3 value;
  DALI_TEST_EQUALS( value, Vector3::ZERO, TEST_LOCATION );

  const Vector3 scale( 4.0f, 5.0f, 6.0f );
  RelativeToConstraint constraint( scale );
  constraint( value, inputs );

  DALI_TEST_EQUALS( value, PropertyInputImpl::VECTOR3_VALUE * scale, TEST_LOCATION );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// RelativeToConstraintFloat
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintsRelativeToConstraintFloat(void)
{
  PropertyInputContainer inputs;
  PropertyInputImpl input( Property::VECTOR3 );
  inputs.PushBack( &input );

  const float scale( 4.0f );

  float value = 0.0f;
  DALI_TEST_CHECK( value != PropertyInputImpl::FLOAT_VALUE * scale );

  RelativeToConstraintFloat constraint( scale );
  constraint( value, inputs );

  DALI_TEST_EQUALS( value, PropertyInputImpl::FLOAT_VALUE * scale, TEST_LOCATION );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// LookAt
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintsLookAt(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_EQUALS( actor.GetCurrentWorldOrientation(), Quaternion::IDENTITY, TEST_LOCATION );

  Vector3 targetPosition;
  Vector3 cameraPosition;
  Quaternion targetOrientation;

  Vector3PropertyInput targetPositionProperty( targetPosition );
  Vector3PropertyInput cameraPositionProperty( cameraPosition );
  QuaternionPropertyInput targetOrientationProperty( targetOrientation );

  PropertyInputContainer inputs;
  inputs.PushBack( &targetPositionProperty );
  inputs.PushBack( &cameraPositionProperty );
  inputs.PushBack( &targetOrientationProperty );

  Quaternion current;

  // 180 degrees round y
  targetPosition = Vector3::ZERO;
  cameraPosition = Vector3( 0.0f, 0.0f, 1.0f );
  targetOrientation = Quaternion::IDENTITY;
  Quaternion lookAtOrientation( Quaternion( Radian( Math::PI ), Vector3::YAXIS ) );
  LookAt( current, inputs );
  DALI_TEST_EQUALS( current, lookAtOrientation, TEST_LOCATION );

  // 180 degrees round y * -45 degrees round x
  targetPosition = Vector3::ZERO;
  cameraPosition = Vector3( 0.0f, -1.0f, 1.0f );
  targetOrientation = Quaternion::IDENTITY;
  lookAtOrientation = Quaternion( Radian( Math::PI ), Vector3::YAXIS ) * Quaternion( Radian( Math::PI * 0.25f ), -Vector3::XAXIS );
  LookAt( current, inputs );
  DALI_TEST_EQUALS( current, lookAtOrientation, Math::MACHINE_EPSILON_10, TEST_LOCATION );

  // 180 degrees round y * -45 degrees round x at different points
  targetPosition = Vector3( 0.0f, 1.0f, -1.0f );
  cameraPosition = Vector3::ZERO;
  targetOrientation = Quaternion::IDENTITY;
  lookAtOrientation = Quaternion( Radian( Math::PI ), Vector3::YAXIS ) * Quaternion( Radian( Math::PI * 0.25f ), -Vector3::XAXIS );
  LookAt( current, inputs );
  DALI_TEST_EQUALS( current, lookAtOrientation, Math::MACHINE_EPSILON_10, TEST_LOCATION );

  // 225 degrees round y
  targetPosition = Vector3( -1.0f, 0.0f, 0.0f );
  cameraPosition = Vector3( 0.0f, 0.0f, 1.0f );
  targetOrientation = Quaternion::IDENTITY;
  lookAtOrientation = Quaternion( Radian( Math::PI * 1.25), Vector3::YAXIS );
  LookAt( current, inputs );
  DALI_TEST_EQUALS( current, lookAtOrientation, Math::MACHINE_EPSILON_10, TEST_LOCATION );

  // 180 degrees round y * -45 degrees round x, Up Vector: 180 degrees
  targetPosition = Vector3::ZERO;
  cameraPosition = Vector3( 0.0f, -1.0f, 1.0f );
  targetOrientation = Quaternion( Radian( Math::PI ), Vector3::ZAXIS );
  lookAtOrientation = Quaternion( Radian( Math::PI ), Vector3::YAXIS ) * Quaternion( Radian( Math::PI * 0.25f ), -Vector3::XAXIS ) * Quaternion( Radian( Math::PI ), -Vector3::ZAXIS );
  LookAt( current, inputs );
  DALI_TEST_EQUALS( current, lookAtOrientation, Math::MACHINE_EPSILON_10, TEST_LOCATION );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

int UtcDaliPropertyInputGetExtension(void)
{
  PropertyInputImpl input( Property::BOOLEAN );
  DALI_TEST_CHECK( input.GetExtension() == NULL );
  END_TEST;
}
