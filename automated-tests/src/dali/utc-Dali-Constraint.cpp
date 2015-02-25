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

void utc_dali_constraint_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_constraint_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{

struct EqualToQuaternion
{
  EqualToQuaternion()
  {
  }

  Quaternion operator()( const Quaternion& current, const PropertyInput& property )
  {
    return property.GetQuaternion();
  }
};

struct EqualToVector4
{
  EqualToVector4()
  {
  }

  Vector4 operator()( const Vector4& current, const PropertyInput& property )
  {
    return property.GetVector4();
  }
};

class PropertyInputAbstraction : public Dali::PropertyInput
{
public:
  PropertyInputAbstraction(const bool& val) : mType(Dali::Property::BOOLEAN), mBoolData( val )  {}
  PropertyInputAbstraction(const float& val) : mType(Dali::Property::FLOAT), mFloatData( val )  {}
  PropertyInputAbstraction(const int& val) : mType(Dali::Property::INTEGER), mIntData( val )  {}
  PropertyInputAbstraction(const Vector2& val) : mType(Dali::Property::VECTOR2), mVector2Data( val )  {}
  PropertyInputAbstraction(const Vector3& val) : mType(Dali::Property::VECTOR3), mVector3Data( val )  {}
  PropertyInputAbstraction(const Vector4& val) : mType(Dali::Property::VECTOR4), mVector4Data( val )  {}
  PropertyInputAbstraction(const Matrix3& val) : mType(Dali::Property::MATRIX3), mMatrix3Data( val )  {}
  PropertyInputAbstraction(const Matrix& val) : mType(Dali::Property::MATRIX), mMatrixData( val )  {}
  PropertyInputAbstraction(const Quaternion& val) : mType(Dali::Property::ROTATION), mQuaternionData( val )  {}

  ~PropertyInputAbstraction() {}

  Dali::Property::Type GetType() const { return mType; }

  const bool& GetBoolean() const { return mBoolData; }

  const float& GetFloat() const { return mFloatData; }

  const int& GetInteger() const { return mIntData; }

  const Vector2& GetVector2() const { return mVector2Data; }
  const Vector3& GetVector3() const { return mVector3Data; }
  const Vector4& GetVector4() const { return mVector4Data; }

  const Matrix3& GetMatrix3() const { return mMatrix3Data; }
  const Matrix& GetMatrix() const { return mMatrixData; }

  const Quaternion& GetQuaternion() const { return mQuaternionData; }

private:
  Dali::Property::Type mType;
  bool mBoolData;
  float mFloatData;
  int mIntData;
  Vector2 mVector2Data;
  Vector3 mVector3Data;
  Vector4 mVector4Data;
  Matrix3 mMatrix3Data;
  Matrix mMatrixData;
  Quaternion mQuaternionData;
};

static const float POSITION_EPSILON = 0.0001f;
static const float ROTATION_EPSILON = 0.0001f;

struct TestConstraint
{
  Vector4 operator()(const Vector4&    color)
  {
    return Vector4(color.x, color.y, color.z, 0.1f);
  }
};

struct TestConstraintToVector3
{
  TestConstraintToVector3(Vector3 target)
  : mTarget(target)
  {
  }

  Vector3 operator()(const Vector3& current)
  {
    return mTarget;
  }

  Vector3 mTarget;
};

struct TestColorConstraint
{
  TestColorConstraint(Vector4 target)
  : mTarget(target)
  {
  }

  Vector4 operator()(const Vector4&    color)
  {
    return mTarget;
  }

  Vector4 mTarget;
};

struct TestPositionConstraint
{
  TestPositionConstraint(Vector3 target)
  : mTarget(target)
  {
  }

  Vector3 operator()(const Vector3&    position)
  {
    return mTarget;
  }

  Vector3 mTarget;
};


struct TestAlwaysTrueConstraint
{
  bool operator()( const bool& current )
  {
    return true;
  }
};

struct TestAlwaysEqualOrGreaterThanConstraintFloat
{
  TestAlwaysEqualOrGreaterThanConstraintFloat( float value )
  : mValue( value )
  {
  }

  float operator()( const float& current )
  {
    return ( current < mValue ) ? mValue : current;
  }

  float mValue;
};

struct TestAlwaysEqualOrGreaterThanConstraintInteger
{
  TestAlwaysEqualOrGreaterThanConstraintInteger( int value )
  : mValue( value )
  {
  }

  int operator()( const int& current )
  {
    return ( current < mValue ) ? mValue : current;
  }

  int mValue;
};

struct TestAlwaysEqualOrGreaterThanConstraintVector2
{
  TestAlwaysEqualOrGreaterThanConstraintVector2( Vector2 value )
  : mValue( value )
  {
  }

  Vector2 operator()( const Vector2& current )
  {
    return Vector2( ( current.x < mValue.x ) ? mValue.x : current.x,
                    ( current.y < mValue.y ) ? mValue.y : current.y
                  );
  }

  Vector2 mValue;
};

struct TestAlwaysEqualOrGreaterThanConstraintVector3
{
  TestAlwaysEqualOrGreaterThanConstraintVector3( Vector3 value )
  : mValue( value )
  {
  }

  Vector3 operator()( const Vector3& current )
  {
    return Vector3( ( current.x < mValue.x ) ? mValue.x : current.x,
                    ( current.y < mValue.y ) ? mValue.y : current.y,
                    ( current.z < mValue.z ) ? mValue.z : current.z
                  );
  }

  Vector3 mValue;
};

struct TestAlwaysEqualOrGreaterThanConstraintVector4
{
  TestAlwaysEqualOrGreaterThanConstraintVector4( Vector4 value )
  : mValue( value )
  {
  }

  Vector4 operator()( const Vector4& current )
  {
    return Vector4( ( current.x < mValue.x ) ? mValue.x : current.x,
                    ( current.y < mValue.y ) ? mValue.y : current.y,
                    ( current.z < mValue.z ) ? mValue.z : current.z,
                    ( current.w < mValue.w ) ? mValue.w : current.w
                  );
  }

  Vector4 mValue;
};

struct TestConstraintFloat
{
  TestConstraintFloat( float value )
  : mValue( value )
  {
  }

  float operator()( const float& current )
  {
    return mValue;
  }

  float mValue;
};

struct TestConstraintInteger
{
  TestConstraintInteger( int value )
  : mValue( value )
  {
  }

  int operator()( const int& current )
  {
    return mValue;
  }

  int mValue;
};

struct TestConstraintVector2
{
  TestConstraintVector2( Vector2 value )
  : mValue( value )
  {
  }

  Vector2 operator()( const Vector2& current )
  {
    return mValue;
  }

  Vector2 mValue;
};

struct TestConstraintVector3
{
  TestConstraintVector3( Vector3 value )
  : mValue( value )
  {
  }

  Vector3 operator()( const Vector3& current )
  {
    return mValue;
  }

  Vector3 mValue;
};

struct TestConstraintVector4
{
  TestConstraintVector4( Vector4 value )
  : mValue( value )
  {
  }

  Vector4 operator()( const Vector4& current )
  {
    return mValue;
  }

  Vector4 mValue;
};

struct TestConstraintRotation
{
  TestConstraintRotation( Quaternion rotation )
  : mRotation( rotation )
  {
  }

  Quaternion operator()( const Quaternion& current )
  {
    return mRotation;
  }

  Quaternion mRotation;
};

struct TestConstraintMatrix3
{
  TestConstraintMatrix3(Matrix3 matrix3)
  : mMatrix3( matrix3 )
  {
  }

  Matrix3 operator()( const Matrix3& current )
  {
    return mMatrix3;
  }

  Matrix3 mMatrix3;
};

struct TestConstraintMatrix
{
  TestConstraintMatrix(Matrix matrix)
  : mMatrix( matrix )
  {
  }

  Matrix operator()( const Matrix& current )
  {
    return mMatrix;
  }

  Matrix mMatrix;
};

struct MoveAwayWithFadeConstraint
{
  MoveAwayWithFadeConstraint( float distance )
  : mDistance( distance )
  {
  }

  Vector3 operator()( const Vector3& current,
                      const PropertyInput& color )
  {
    return Vector3( current.x,
                    current.y,
                    -mDistance * (1.0f - color.GetVector4().a) );
  }

  float mDistance;
};

struct TestBottomRightAlignConstraint
{
  Vector3 operator()( const Vector3& current,
                      const PropertyInput& parentSize )
  {
    return Vector3( parentSize.GetVector3().x, parentSize.GetVector3().y, 0.0f );
  }
};

struct MeanPositionConstraint1
{
  Vector3 operator()( const Vector3& current,
                      const PropertyInput& position1 )
  {
    return Vector3( position1.GetVector3() );
  }
};

struct MeanPositionConstraint2
{
  Vector3 operator()( const Vector3& current,
                      const PropertyInput& position1,
                      const PropertyInput& position2 )
  {
    Vector3 meanValue = position1.GetVector3() +
                        position2.GetVector3();

    return meanValue * 0.5f; // div 2
  }
};

struct MeanPositionConstraint3
{
  Vector3 operator()( const Vector3& current,
                      const PropertyInput& position1,
                      const PropertyInput& position2,
                      const PropertyInput& position3 )
  {
    Vector3 meanValue = position1.GetVector3() +
                        position2.GetVector3() +
                        position3.GetVector3();

    return meanValue * (1.0f / 3.0f); // div 3
  }
};

struct MeanPositionConstraint4
{
  Vector3 operator()( const Vector3& current,
                      const PropertyInput& position1,
                      const PropertyInput& position2,
                      const PropertyInput& position3,
                      const PropertyInput& position4 )
  {
    Vector3 meanValue = position1.GetVector3() +
                        position2.GetVector3() +
                        position3.GetVector3() +
                        position4.GetVector3();

    return meanValue * 0.25f; // div 4
  }
};

struct MeanPositionConstraint5
{
  Vector3 operator()( const Vector3& current,
                      const PropertyInput& position1,
                      const PropertyInput& position2,
                      const PropertyInput& position3,
                      const PropertyInput& position4,
                      const PropertyInput& position5 )
  {
    Vector3 meanValue = position1.GetVector3() +
                        position2.GetVector3() +
                        position3.GetVector3() +
                        position4.GetVector3() +
                        position5.GetVector3();

    return meanValue * 0.2f; // div 5
  }
};

struct MeanPositionConstraint6
{
  Vector3 operator()( const Vector3& current,
                      const PropertyInput& position1,
                      const PropertyInput& position2,
                      const PropertyInput& position3,
                      const PropertyInput& position4,
                      const PropertyInput& position5,
                      const PropertyInput& position6 )
  {
    Vector3 meanValue = position1.GetVector3() +
                        position2.GetVector3() +
                        position3.GetVector3() +
                        position4.GetVector3() +
                        position5.GetVector3() +
                        position6.GetVector3();

    return meanValue * (1.0f / 6.0f); // div 6
  }
};

struct TestRelativeConstraintFloat
{
  TestRelativeConstraintFloat(float scale)
  : mScale(scale)
  {
  }

  float operator()( const float& current, const PropertyInput& relative )
  {
    return relative.GetFloat() * mScale;
  }

  float mScale;
};

struct TestRelativeConstraintVector3
{
  TestRelativeConstraintVector3(float scale)
  : mScale(scale)
  {
  }

  Vector3 operator()( const Vector3& current, const PropertyInput& relative )
  {
    return relative.GetVector3() * mScale;
  }

  float mScale;
};

} // anonymous namespace






int UtcDaliConstraintNewBoolean(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  // Apply constraint

  Constraint constraint = Constraint::New<bool>( index, TestAlwaysTrueConstraint() );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), false, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );

  // Try to fight with the constraint - this shouldn't work!
  actor.SetProperty( index, false );

  application.SendNotification();
  application.Render(0);

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );

  // Remove the constraint, then set new value
  actor.RemoveConstraints();
  actor.SetProperty( index, false );

  // Constraint should have been removed
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), false, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewFloat(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(1.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<float>(index) == startValue );

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<float>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<float>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<float>(index) == startValue );

  // Apply constraint

  float minValue( 2.0f );
  Constraint constraint = Constraint::New<float>( index, TestAlwaysEqualOrGreaterThanConstraintFloat( minValue ) );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), minValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), minValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), minValue, TEST_LOCATION );

  // Set to greater than 2.0f, the constraint will allow this
  actor.SetProperty( index, 3.0f );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<float>(index), 3.0f, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), 3.0f, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), 3.0f, TEST_LOCATION );

  // Set to less than 2.0f, the constraint will NOT allow this
  actor.SetProperty( index, 1.0f );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<float>(index), minValue/*not 1.0f*/, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), minValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), minValue, TEST_LOCATION );

  // Remove the constraint, then set new value
  actor.RemoveConstraints();
  actor.SetProperty( index, 1.0f );

  // Constraint should have been removed
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), 1.0f, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), 1.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewInteger(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue( 1 );
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<int>(index) == startValue );

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<int>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<int>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<int>(index) == startValue );

  // Apply constraint

  int minValue( 2 );
  Constraint constraint = Constraint::New<int>( index, TestAlwaysEqualOrGreaterThanConstraintInteger( minValue ) );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), minValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), minValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), minValue, TEST_LOCATION );

  // Set to greater than 2f, the constraint will allow this
  actor.SetProperty( index, 3 );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<int>(index), 3, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), 3, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), 3, TEST_LOCATION );

  // Set to less than 2, the constraint will NOT allow this
  actor.SetProperty( index, 1 );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<int>(index), minValue/*not 1*/, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), minValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), minValue, TEST_LOCATION );

  // Remove the constraint, then set new value
  actor.RemoveConstraints();
  actor.SetProperty( index, 1 );

  // Constraint should have been removed
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), 1, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), 1, TEST_LOCATION );
  END_TEST;

}

int UtcDaliConstraintNewVector2(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue( 1.0f, 1.0f );
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<Vector2>(index) == startValue );

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<Vector2>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<Vector2>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<Vector2>(index) == startValue );

  // Apply constraint

  Vector2 minValue( 2.0f, 2.0f );
  Constraint constraint = Constraint::New<Vector2>( index, TestAlwaysEqualOrGreaterThanConstraintVector2( minValue ) );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), minValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), minValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), minValue, TEST_LOCATION );

  // Set to greater than 2.0f, the constraint will allow this
  Vector2 greaterValue( 3.0f, 3.0f );
  actor.SetProperty( index, greaterValue );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), greaterValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), greaterValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), greaterValue, TEST_LOCATION );

  // Set to less than 2.0f, the constraint will NOT allow this
  Vector2 lesserValue( 1.0f, 1.0f );
  actor.SetProperty( index, lesserValue );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), minValue/*not lesserValue*/, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), minValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), minValue, TEST_LOCATION );

  // Remove the constraint, then set new value
  actor.RemoveConstraints();
  actor.SetProperty( index, lesserValue );

  // Constraint should have been removed
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), lesserValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), lesserValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewVector3(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(1.0f, 1.0f, 1.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<Vector3>(index) == startValue );

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<Vector3>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<Vector3>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<Vector3>(index) == startValue );

  // Apply constraint

  Vector3 minValue( 2.0f, 2.0f, 2.0f );
  Constraint constraint = Constraint::New<Vector3>( index, TestAlwaysEqualOrGreaterThanConstraintVector3( minValue ) );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), minValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), minValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), minValue, TEST_LOCATION );

  // Set to greater than 2.0f, the constraint will allow this
  Vector3 greaterValue( 3.0f, 3.0f, 3.0f );
  actor.SetProperty( index, greaterValue );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), greaterValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), greaterValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), greaterValue, TEST_LOCATION );

  // Set to less than 2.0f, the constraint will NOT allow this
  Vector3 lesserValue( 1.0f, 1.0f, 1.0f );
  actor.SetProperty( index, lesserValue );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), minValue/*not lesserValue*/, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), minValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), minValue, TEST_LOCATION );

  // Remove the constraint, then set new value
  actor.RemoveConstraints();
  actor.SetProperty( index, lesserValue );

  // Constraint should have been removed
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), lesserValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), lesserValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewVector4(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue( 1.0f, 1.0f, 1.0f, 1.0f );
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<Vector4>(index) == startValue );

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<Vector4>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<Vector4>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<Vector4>(index) == startValue );

  // Apply constraint

  Vector4 minValue( 2.0f, 2.0f, 2.0f, 2.0f );
  Constraint constraint = Constraint::New<Vector4>( index, TestAlwaysEqualOrGreaterThanConstraintVector4( minValue ) );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), minValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), minValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), minValue, TEST_LOCATION );

  // Set to greater than 2.0f, the constraint will allow this
  Vector4 greaterValue( 3.0f, 3.0f, 3.0f, 3.0f );
  actor.SetProperty( index, greaterValue );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), greaterValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), greaterValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), greaterValue, TEST_LOCATION );

  // Set to less than 2.0f, the constraint will NOT allow this
  Vector4 lesserValue( 1.0f, 1.0f, 1.0f, 1.0f );
  actor.SetProperty( index, lesserValue );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), minValue/*not lesserValue*/, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), minValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), minValue, TEST_LOCATION );

  // Remove the constraint, then set new value
  actor.RemoveConstraints();
  actor.SetProperty( index, lesserValue );

  // Constraint should have been removed
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), lesserValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), lesserValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewMatrix(void)
{
  try
  {
    TestApplication application;

    Actor actor = Actor::New();

    // Register a Matrix property
    Matrix startValue = Matrix::IDENTITY;
    Property::Index index = actor.RegisterProperty( "test-property", startValue );
    DALI_TEST_CHECK( index != Property::INVALID_INDEX );
    if (index != Property::INVALID_INDEX)
    {
      Stage::GetCurrent().Add(actor);
      DALI_TEST_CHECK( actor.GetProperty<Matrix>(index) == startValue );

      // Apply constraint
      Matrix constraintLimit;
      constraintLimit.SetTransformComponents(Vector3::ONE, Quaternion(Radian(Degree(30.0f)), Vector3::YAXIS), Vector3::ZAXIS );
      Constraint constraint = Constraint::New<Matrix>( index, TestConstraintMatrix(constraintLimit));
      actor.ApplyConstraint( constraint );
      DALI_TEST_EQUALS( actor.GetProperty<Matrix>(index), startValue, TEST_LOCATION );

      application.SendNotification();
      application.Render(0);

      DALI_TEST_EQUALS( actor.GetProperty<Matrix>(index), constraintLimit, TEST_LOCATION );
    }
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_CHECK(0);
  }
  END_TEST;
}

int UtcDaliConstraintNewMatrix3(void)
{
  try
  {
    TestApplication application;

    Actor actor = Actor::New();

    // Register a Matrix3 property
    Matrix3 startValue(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    Property::Index index = actor.RegisterProperty( "test-property", startValue );
    DALI_TEST_CHECK( index != Property::INVALID_INDEX );
    if (index != Property::INVALID_INDEX)
    {
      Stage::GetCurrent().Add(actor);
      DALI_TEST_CHECK( actor.GetProperty<Matrix3>(index) == startValue );

      // Apply constraint
      Matrix3 constraintLimit(42.0f, 0.0f, 0.0f, 0.0f, 42.0f, 0.0f, 0.0f, 0.0f, 1.0f);
      Constraint constraint = Constraint::New<Matrix3>( index, TestConstraintMatrix3(constraintLimit));
      actor.ApplyConstraint( constraint );
      DALI_TEST_EQUALS( actor.GetProperty<Matrix3>(index), startValue, 0.001f, TEST_LOCATION );

      application.SendNotification();
      application.Render(0);

      DALI_TEST_EQUALS( actor.GetProperty<Matrix3>(index), constraintLimit, 0.001f, TEST_LOCATION );
    }
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_CHECK(0);
  }
  END_TEST;
}

int UtcDaliConstraintNewQuaternion(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Quaternion property
  Quaternion startValue( 0.0f, Vector3::YAXIS );
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );

  // Apply constraint

  Quaternion constrainedRotation( M_PI*0.25f, Vector3::YAXIS );
  Constraint constraint = Constraint::New<Quaternion>( index, TestConstraintRotation( constrainedRotation ) );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );

  // Set to a different rotation, the constraint will NOT allow this
  Quaternion differentRotation( M_PI*0.5f, Vector3::YAXIS );
  actor.SetProperty( index, differentRotation );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation/*not differentRotation*/, ROTATION_EPSILON, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );

  // Remove the constraint, then set new value
  actor.RemoveConstraints();
  actor.SetProperty( index, differentRotation );

  // Constraint should have been removed
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), differentRotation, ROTATION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), differentRotation, ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewOffStageBoolean(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  // Apply constraint to off-stage Actor
  Constraint constraint = Constraint::New<bool>( index, TestAlwaysTrueConstraint() );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), false, TEST_LOCATION );

  // Add actor to stage
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );

  // Take the actor off-stage
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );

  // Set a new value; the constraint will not prevent this
  actor.SetProperty( index, false );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), false, TEST_LOCATION );

  // Add actor to stage (2nd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied (2nd time)
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );

  // Take the actor off-stage (2nd-time)
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), true, TEST_LOCATION );

  // Remove the constraint, and set a new value
  actor.RemoveConstraints();
  actor.SetProperty( index, false );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), false, TEST_LOCATION );

  // Add actor to stage (3rd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be gone
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), false, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), false, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<bool>(index), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewOffStageFloat(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(1.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  DALI_TEST_CHECK( actor.GetProperty<float>(index) == startValue );

  // Apply constraint to off-stage Actor
  float constrainedValue( 2.0f );
  Constraint constraint = Constraint::New<float>( index, TestConstraintFloat( constrainedValue ) );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Add actor to stage
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), constrainedValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), constrainedValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), constrainedValue, TEST_LOCATION );

  // Take the actor off-stage
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), constrainedValue, TEST_LOCATION );

  // Set back to startValue; the constraint will not prevent this
  actor.SetProperty( index, startValue );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Add actor to stage (2nd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied (2nd time)
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), constrainedValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), constrainedValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), constrainedValue, TEST_LOCATION );

  // Take the actor off-stage (2nd-time)
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), constrainedValue, TEST_LOCATION );

  // Remove the constraint, and set back to startValue
  actor.RemoveConstraints();
  actor.SetProperty( index, startValue );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Add actor to stage (3rd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be gone
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewOffStageInteger(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(1);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  DALI_TEST_CHECK( actor.GetProperty<int>(index) == startValue );

  // Apply constraint to off-stage Actor
  int constrainedValue( 2 );
  Constraint constraint = Constraint::New<int>( index, TestConstraintInteger( constrainedValue ) );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );

  // Add actor to stage
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), constrainedValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), constrainedValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), constrainedValue, TEST_LOCATION );

  // Take the actor off-stage
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), constrainedValue, TEST_LOCATION );

  // Set back to startValue; the constraint will not prevent this
  actor.SetProperty( index, startValue );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );

  // Add actor to stage (2nd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied (2nd time)
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), constrainedValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), constrainedValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), constrainedValue, TEST_LOCATION );

  // Take the actor off-stage (2nd-time)
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), constrainedValue, TEST_LOCATION );

  // Remove the constraint, and set back to startValue
  actor.RemoveConstraints();
  actor.SetProperty( index, startValue );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );

  // Add actor to stage (3rd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be gone
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewOffStageVector2(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(1.0f, 1.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  DALI_TEST_CHECK( actor.GetProperty<Vector2>(index) == startValue );

  // Apply constraint to off-stage Actor
  Vector2 constrainedValue( 2.0f, 2.0f );
  Constraint constraint = Constraint::New<Vector2>( index, TestConstraintVector2( constrainedValue ) );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  // Add actor to stage
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), constrainedValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), constrainedValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), constrainedValue, TEST_LOCATION );

  // Take the actor off-stage
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), constrainedValue, TEST_LOCATION );

  // Set back to startValue; the constraint will not prevent this
  actor.SetProperty( index, startValue );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  // Add actor to stage (2nd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied (2nd time)
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), constrainedValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), constrainedValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), constrainedValue, TEST_LOCATION );

  // Take the actor off-stage (2nd-time)
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), constrainedValue, TEST_LOCATION );

  // Remove the constraint, and set back to startValue
  actor.RemoveConstraints();
  actor.SetProperty( index, startValue );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  // Add actor to stage (3rd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be gone
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewOffStageVector3(void)
{
  TestApplication application;
  Vector3 startValue(1.0f, 1.0f, 1.0f);
  Vector3 constrainedValue = Vector3( 2.0f, 3.0f, 4.0f );

  Actor actor = Actor::New();
  // Register a Vector3 property

  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  DALI_TEST_CHECK( actor.GetProperty<Vector3>(index) == startValue );

  // Apply constraint to off-stage Actor
  Constraint constraint = Constraint::New<Vector3>( index, TestConstraintVector3( constrainedValue ) );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  // Add actor to stage
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), constrainedValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), constrainedValue, TEST_LOCATION );
  application.Render();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), constrainedValue, TEST_LOCATION );

  // Take the actor off-stage
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), constrainedValue, TEST_LOCATION );

  // Set back to startValue; the constraint will not prevent this
  Vector3 intermediateValue(5.0f, 6.0f, 7.0f);
  actor.SetProperty( index, intermediateValue );
  application.SendNotification();
  application.Render();
  application.Render(); // ensure both buffers are set to intermediateValue
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), intermediateValue, TEST_LOCATION );

  // Add actor to stage (2nd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();

  // Constraint should be fully applied (2nd time)
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), constrainedValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), constrainedValue, TEST_LOCATION );
  application.Render();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), constrainedValue, TEST_LOCATION );

  // Take the actor off-stage (2nd-time)
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), constrainedValue, TEST_LOCATION );

  // Remove the constraint, and set back to startValue
  actor.RemoveConstraints();
  actor.SetProperty( index, startValue );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  // Add actor to stage (3rd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();

  // Constraint should be gone
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );
  application.Render();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewOffStageVector4(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(1.0f, 1.0f, 1.0f, 1.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  DALI_TEST_CHECK( actor.GetProperty<Vector4>(index) == startValue );

  // Apply constraint to off-stage Actor
  Vector4 constrainedValue( 2.0f, 2.0f, 2.0f, 2.0f );
  Constraint constraint = Constraint::New<Vector4>( index, TestConstraintVector4( constrainedValue ) );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  // Add actor to stage
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), constrainedValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), constrainedValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), constrainedValue, TEST_LOCATION );

  // Take the actor off-stage
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), constrainedValue, TEST_LOCATION );

  // Set back to startValue; the constraint will not prevent this
  actor.SetProperty( index, startValue );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  // Add actor to stage (2nd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied (2nd time)
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), constrainedValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), constrainedValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), constrainedValue, TEST_LOCATION );

  // Take the actor off-stage (2nd-time)
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), constrainedValue, TEST_LOCATION );

  // Remove the constraint, and set back to startValue
  actor.RemoveConstraints();
  actor.SetProperty( index, startValue );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  // Add actor to stage (3rd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be gone
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewOffStageQuaternion(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Quaternion property
  Quaternion startValue( 0.0f, Vector3::YAXIS );
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );

  // Apply constraint to off-stage Actor
  Quaternion constrainedRotation( M_PI*0.25f, Vector3::YAXIS );
  Constraint constraint = Constraint::New<Quaternion>( index, TestConstraintRotation( constrainedRotation ) );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );

  // Add actor to stage
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );

  // Take the actor off-stage
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );

  // Set back to startValue; the constraint will not prevent this
  actor.SetProperty( index, startValue );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );

  // Add actor to stage (2nd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied (2nd time)
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );

  // Take the actor off-stage (2nd-time)
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), constrainedRotation, ROTATION_EPSILON, TEST_LOCATION );

  // Remove the constraint, and set back to startValue
  actor.RemoveConstraints();
  actor.SetProperty( index, startValue );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );

  // Add actor to stage (3rd time)
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Constraint should be gone
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Quaternion>(index), startValue, ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewLocalInput(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  Vector3 startValue( 0.0f, 0.0f, 0.0f );
  float distanceWhenFullyTransparent( 100.0f );

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  // Apply constraint with a local input property

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    LocalSource( Actor::COLOR ),
                                                    MoveAwayWithFadeConstraint(distanceWhenFullyTransparent) );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  // Gradually set the color to fully-transparent; the actor should move back

  for ( float progress = 0.0f; progress < 1.1f; progress += 0.1f )
  {
    actor.SetOpacity( 1.0f - progress );

    application.SendNotification();
    application.Render(0);
    DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), ( startValue - Vector3(0.0f, 0.0f, progress*distanceWhenFullyTransparent) ), POSITION_EPSILON, TEST_LOCATION );
  }
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), ( startValue - Vector3(0.0f, 0.0f, distanceWhenFullyTransparent) ), POSITION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewParentInput(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentStartSize( 100.0f, 100.0f, 0.0f );
  parent.SetSize( parentStartSize );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  parent.Add( actor );

  Vector3 startValue( 0.0f, 0.0f, 0.0f );

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  // Apply constraint with a parent input property

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    ParentSource( Actor::SIZE ),
                                                    TestBottomRightAlignConstraint() );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), parentStartSize,         TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), parent.GetCurrentSize(), TEST_LOCATION );

  // Gradually shrink the parent; the actor should move inwards

  for ( float progress = 0.0f; progress < 1.1f; progress += 0.1f )
  {
    Vector3 size( parentStartSize * std::max(0.0f, 1.0f - progress) );
    parent.SetSize( size );

    application.SendNotification();
    application.Render(0);

    DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), size,                    POSITION_EPSILON, TEST_LOCATION );
    DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), parent.GetCurrentSize(), POSITION_EPSILON, TEST_LOCATION );
  }
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), Vector3::ZERO, POSITION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewInput1(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentStartSize( 100.0f, 100.0f, 0.0f );
  parent.SetSize( parentStartSize );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  parent.Add( actor );

  Actor sibling1 = Actor::New();
  sibling1.SetPosition( Vector3(1.0f, 2.0f, 3.0f) );
  parent.Add( sibling1 );

  Vector3 startValue( 0.0f, 0.0f, 0.0f );

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  // Apply constraint with a parent input property

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    Source( sibling1, Actor::POSITION ),
                                                    MeanPositionConstraint1() );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), sibling1.GetCurrentPosition(), TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), sibling1.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), sibling1.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewInput2(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentStartSize( 100.0f, 100.0f, 0.0f );
  parent.SetSize( parentStartSize );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  parent.Add( actor );

  Actor sibling1 = Actor::New();
  sibling1.SetPosition( Vector3(1.0f, 2.0f, 3.0f) );
  parent.Add( sibling1 );

  Actor sibling2 = Actor::New();
  sibling2.SetPosition( Vector3(300.0f, 300.0f, 300.0f) );
  parent.Add( sibling2 );

  application.SendNotification();
  application.Render(0);

  Vector3 startValue( 0.0f, 0.0f, 0.0f );
  Vector3 meanValue = sibling1.GetCurrentPosition() +
                      sibling2.GetCurrentPosition();
  meanValue *= (1.0f / 2.0f); // divide by number of siblings

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  // Apply constraint with a parent input property

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    Source( sibling1, Actor::POSITION ),
                                                    Source( sibling2, Actor::POSITION ),
                                                    MeanPositionConstraint2() );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewInput3(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentStartSize( 100.0f, 100.0f, 0.0f );
  parent.SetSize( parentStartSize );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  parent.Add( actor );

  Actor sibling1 = Actor::New();
  sibling1.SetPosition( Vector3(1.0f, 2.0f, 3.0f) );
  parent.Add( sibling1 );

  Actor sibling2 = Actor::New();
  sibling2.SetPosition( Vector3(300.0f, 300.0f, 300.0f) );
  parent.Add( sibling2 );

  Actor sibling3 = Actor::New();
  sibling3.SetPosition( Vector3(-100.0f, -10.0f, -1.0f) );
  parent.Add( sibling3 );

  application.SendNotification();
  application.Render(0);

  Vector3 startValue( 0.0f, 0.0f, 0.0f );
  Vector3 meanValue = sibling1.GetCurrentPosition() +
                      sibling2.GetCurrentPosition() +
                      sibling3.GetCurrentPosition();
  meanValue *= (1.0f / 3.0f); // divide by number of siblings

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  // Apply constraint with a parent input property

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    Source( sibling1, Actor::POSITION ),
                                                    Source( sibling2, Actor::POSITION ),
                                                    Source( sibling3, Actor::POSITION ),
                                                    MeanPositionConstraint3() );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewInput4(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentStartSize( 100.0f, 100.0f, 0.0f );
  parent.SetSize( parentStartSize );
  parent.SetPosition( 10.0f, 10.0f, 10.0f );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  parent.Add( actor );

  Actor sibling1 = Actor::New();
  sibling1.SetPosition( Vector3(1.0f, 2.0f, 3.0f) );
  parent.Add( sibling1 );

  Actor sibling2 = Actor::New();
  sibling2.SetPosition( Vector3(300.0f, 300.0f, 300.0f) );
  parent.Add( sibling2 );

  Actor sibling3 = Actor::New();
  sibling3.SetPosition( Vector3(-100.0f, -10.0f, -1.0f) );
  parent.Add( sibling3 );

  application.SendNotification();
  application.Render(0);

  Vector3 startValue( 0.0f, 0.0f, 0.0f );
  Vector3 meanValue = parent.GetCurrentPosition() +
                      sibling1.GetCurrentPosition() +
                      sibling2.GetCurrentPosition() +
                      sibling3.GetCurrentPosition();
  meanValue *= (1.0f / 4.0f); // divide by number of positions

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  // Apply constraint with a parent input property

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    Source( sibling1, Actor::POSITION ),
                                                    Source( sibling2, Actor::POSITION ),
                                                    ParentSource( Actor::POSITION ),
                                                    Source( sibling3, Actor::POSITION ),
                                                    MeanPositionConstraint4() );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewInput5(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentStartSize( 100.0f, 100.0f, 0.0f );
  parent.SetSize( parentStartSize );
  parent.SetPosition( 10.0f, 10.0f, 10.0f );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  parent.Add( actor );

  Actor sibling1 = Actor::New();
  sibling1.SetPosition( Vector3(1.0f, 2.0f, 3.0f) );
  parent.Add( sibling1 );

  Actor sibling2 = Actor::New();
  sibling2.SetPosition( Vector3(300.0f, 300.0f, 300.0f) );
  parent.Add( sibling2 );

  Actor sibling3 = Actor::New();
  sibling3.SetPosition( Vector3(-100.0f, -10.0f, -1.0f) );
  parent.Add( sibling3 );

  Actor sibling4 = Actor::New();
  sibling4.SetPosition( Vector3(-1.0f, 1.0f, 2.0f) );
  parent.Add( sibling4 );

  application.SendNotification();
  application.Render(0);

  Vector3 startValue( 0.0f, 0.0f, 0.0f );
  Vector3 meanValue = parent.GetCurrentPosition() +
                      sibling1.GetCurrentPosition() +
                      sibling2.GetCurrentPosition() +
                      sibling3.GetCurrentPosition() +
                      sibling4.GetCurrentPosition();
  meanValue *= (1.0f / 5.0f); // divide by number of positions

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  // Apply constraint with a parent input property

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    Source( sibling1, Actor::POSITION ),
                                                    Source( sibling2, Actor::POSITION ),
                                                    ParentSource( Actor::POSITION ),
                                                    Source( sibling3, Actor::POSITION ),
                                                    Source( sibling4, Actor::POSITION ),
                                                    MeanPositionConstraint5() );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintNewInput6(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentStartSize( 100.0f, 100.0f, 0.0f );
  parent.SetSize( parentStartSize );
  parent.SetPosition( 10.0f, 10.0f, 10.0f );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  parent.Add( actor );

  Actor child = Actor::New();
  child.SetPosition( Vector3(7.0f, 7.0f, 7.0f) );
  actor.Add( child );

  Actor sibling1 = Actor::New();
  sibling1.SetPosition( Vector3(1.0f, 2.0f, 3.0f) );
  parent.Add( sibling1 );

  Actor sibling2 = Actor::New();
  sibling2.SetPosition( Vector3(300.0f, 300.0f, 300.0f) );
  parent.Add( sibling2 );

  Actor sibling3 = Actor::New();
  sibling3.SetPosition( Vector3(-100.0f, -10.0f, -1.0f) );
  parent.Add( sibling3 );

  Actor sibling4 = Actor::New();
  sibling4.SetPosition( Vector3(-1.0f, 1.0f, 2.0f) );
  parent.Add( sibling4 );

  application.SendNotification();
  application.Render(0);

  Vector3 startValue( 0.0f, 0.0f, 0.0f );
  Vector3 meanValue = parent.GetCurrentPosition() +
                      child.GetCurrentPosition() +
                      sibling1.GetCurrentPosition() +
                      sibling2.GetCurrentPosition() +
                      sibling3.GetCurrentPosition() +
                      sibling4.GetCurrentPosition();
  meanValue *= (1.0f / 6.0f); // divide by number of positions

  /**
   * Test that the Constraint is correctly applied on a clean Node
   */
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  // Apply constraint with a parent input property

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    Source( child, Actor::POSITION ),
                                                    Source( sibling1, Actor::POSITION ),
                                                    Source( sibling2, Actor::POSITION ),
                                                    ParentSource( Actor::POSITION ),
                                                    Source( sibling3, Actor::POSITION ),
                                                    Source( sibling4, Actor::POSITION ),
                                                    MeanPositionConstraint6() );

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), meanValue, POSITION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Constraint::DownCast()");

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Constraint constraint = Constraint::New<bool>( index, TestAlwaysTrueConstraint() );

  BaseHandle object(constraint);

  Constraint constraint2 = Constraint::DownCast(object);
  DALI_TEST_CHECK(constraint2);

  Constraint constraint3 = DownCast< Constraint >(object);
  DALI_TEST_CHECK(constraint3);

  BaseHandle unInitializedObject;
  Constraint constraint4 = Constraint::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!constraint4);

  Constraint constraint5 = DownCast< Constraint >(unInitializedObject);
  DALI_TEST_CHECK(!constraint5);
  END_TEST;
}

int UtcDaliConstraintSetApplyTime(void)
{
  TestApplication application;

  // Build constraint

  Vector4 targetColor(Color::BLACK);
  Constraint constraint = Constraint::New<Vector4>( Actor::COLOR, TestColorConstraint(targetColor) );
  DALI_TEST_EQUALS(constraint.GetApplyTime(), TimePeriod(0.0f), TEST_LOCATION);

  float applySeconds(7.0f);
  constraint.SetApplyTime(applySeconds);
  DALI_TEST_EQUALS(constraint.GetApplyTime(), TimePeriod(applySeconds), TEST_LOCATION);

  // Apply to an actor

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(applySeconds*200.0f)/* 20% progress */);

  // Constraint shouldn't be fully applied yet
  Vector4 twentyPercentColor( Color::WHITE.x*0.8f, Color::WHITE.y*0.8f, Color::WHITE.z*0.8f, Color::WHITE.a );
  DALI_TEST_EQUALS( actor.GetCurrentColor(), twentyPercentColor, TEST_LOCATION );

  // Constraint shouldn't be fully applied yet
  application.Render(static_cast<unsigned int>(applySeconds*200.0f)/* 40% progress */);
  Vector4 fourtyPercentColor( Color::WHITE.x*0.6f, Color::WHITE.y*0.6f, Color::WHITE.z*0.6f, Color::WHITE.a );
  DALI_TEST_EQUALS( actor.GetCurrentColor(), fourtyPercentColor, TEST_LOCATION );

  // Constraint shouldn't be fully applied yet
  application.Render(static_cast<unsigned int>(applySeconds*200.0f)/* 60% progress */);
  Vector4 sixtyPercentColor( Color::WHITE.x*0.4f, Color::WHITE.y*0.4f, Color::WHITE.z*0.4f, Color::WHITE.a );
  DALI_TEST_EQUALS( actor.GetCurrentColor(), sixtyPercentColor, TEST_LOCATION );

  // Constraint shouldn't be fully applied yet
  application.Render(static_cast<unsigned int>(applySeconds*200.0f)/* 80% progress */);
  Vector4 eightyPercentColor( Color::WHITE.x*0.2f, Color::WHITE.y*0.2f, Color::WHITE.z*0.2f, Color::WHITE.a );
  DALI_TEST_EQUALS( actor.GetCurrentColor(), eightyPercentColor, TEST_LOCATION );

  // Constraint should be fully applied
  application.Render(static_cast<unsigned int>(applySeconds*200.0f)/* 100% progress */);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), targetColor, TEST_LOCATION );

  // Constraint should still be fully applied
  application.Render(static_cast<unsigned int>(applySeconds*200.0f)/* Still 100% progress */);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), targetColor, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), targetColor, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), targetColor, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintGetApplyTime(void)
{
  TestApplication application;

  Constraint constraint = Constraint::New<Vector4>( Actor::COLOR, TestConstraint() );
  DALI_TEST_EQUALS(constraint.GetApplyTime(), TimePeriod(0.0f), TEST_LOCATION);

  float applySeconds(7.0f);
  constraint.SetApplyTime(applySeconds);
  DALI_TEST_EQUALS(constraint.GetApplyTime(), TimePeriod(applySeconds), TEST_LOCATION);

  constraint.SetApplyTime(applySeconds - 3.0f);
  DALI_TEST_EQUALS(constraint.GetApplyTime(), TimePeriod(applySeconds - 3.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliConstraintSetAlphaFunction(void)
{
  TestApplication application;

  Vector3 startValue( Vector3::ZERO );
  Vector3 targetValue(100.0f, 100.0f, 100.0f);

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    TestConstraintVector3( targetValue ) );

  // Test the alpha-function itself

  AlphaFunction func = constraint.GetAlphaFunction();
  DALI_TEST_EQUALS(func(0.1f), 0.1f, TEST_LOCATION); // Default is Linear

  // Test that the alpha-function is used correctly

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  constraint.SetApplyTime( 10.0f );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue) * 0.1f, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue) * 0.2f, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue) * 0.3f, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue) * 0.4f, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue) * 0.5f, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue) * 0.6f, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue) * 0.7f, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue) * 0.8f, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue) * 0.9f, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue), TEST_LOCATION );

  // Check that the constrained value is stable
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue), TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue), TEST_LOCATION );

  // Remove the constraint

  actor.RemoveConstraints();
  actor.SetPosition( startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), startValue, TEST_LOCATION );

  // Change to non-linear alpha and retest

  constraint.SetAlphaFunction(AlphaFunctions::EaseIn);
  func = constraint.GetAlphaFunction();
  DALI_TEST_CHECK(func(0.1f) < 0.09f);

  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));

  DALI_TEST_CHECK( actor.GetProperty<Vector3>( Actor::POSITION ).x > startValue.x );
  DALI_TEST_CHECK( actor.GetProperty<Vector3>( Actor::POSITION ).y > startValue.y );
  DALI_TEST_CHECK( actor.GetProperty<Vector3>( Actor::POSITION ).z > startValue.z );

  Vector3 lessThanTenPercentProgress( (targetValue - startValue) * 0.09f );
  DALI_TEST_CHECK( actor.GetProperty<Vector3>( Actor::POSITION ).x < lessThanTenPercentProgress.x );
  DALI_TEST_CHECK( actor.GetProperty<Vector3>( Actor::POSITION ).y < lessThanTenPercentProgress.y );
  DALI_TEST_CHECK( actor.GetProperty<Vector3>( Actor::POSITION ).z < lessThanTenPercentProgress.z );

  application.Render(static_cast<unsigned int>(9000.0f/*9 seconds*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue), TEST_LOCATION );

  // Check that the constrained value is stable
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue), TEST_LOCATION );
  application.Render(static_cast<unsigned int>(1000.0f/*1 second*/));
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( Actor::POSITION ), (targetValue - startValue), TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintGetAlphaFunction(void)
{
  TestApplication application;

  Constraint constraint = Constraint::New<Vector4>( Actor::COLOR, TestConstraint() );

  AlphaFunction func = constraint.GetAlphaFunction();
  DALI_TEST_EQUALS(func(0.5f), 0.5f, TEST_LOCATION); // Default is Linear
  END_TEST;
}

int UtcDaliConstraintSetRemoveAction(void)
{
  TestApplication application;

  Vector3 sourcePosition(0.0f, 0.0f, 0.0f);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);

  // Build constraint, with "Discard" remove action

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION, TestPositionConstraint(targetPosition) );
  DALI_TEST_EQUALS((unsigned int)constraint.GetRemoveAction(), (unsigned int)Constraint::Bake, TEST_LOCATION);

  constraint.SetRemoveAction(Constraint::Discard);
  DALI_TEST_EQUALS((unsigned int)constraint.GetRemoveAction(), (unsigned int)Constraint::Discard, TEST_LOCATION);

  // Apply to an actor

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), sourcePosition, TEST_LOCATION );

  application.SendNotification();
  application.Render(100u/*0.1 seconds*/);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Remove from the actor
  actor.RemoveConstraints(); // should go back to source position

  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f));

  // Constraint should be fully removed
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), sourcePosition, TEST_LOCATION );

  // Constraint should still be fully removed
  application.Render(static_cast<unsigned int>(1000.0f)/* Still 100% removal progress */);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), sourcePosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), sourcePosition, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), sourcePosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintGetRemoveAction(void)
{
  TestApplication application;

  Constraint constraint = Constraint::New<Vector4>( Actor::COLOR, TestConstraint() );
  DALI_TEST_EQUALS((unsigned int)constraint.GetRemoveAction(), (unsigned int)Constraint::Bake, TEST_LOCATION);

  constraint.SetRemoveAction(Constraint::Discard);
  DALI_TEST_EQUALS((unsigned int)constraint.GetRemoveAction(), (unsigned int)Constraint::Discard, TEST_LOCATION);

  constraint.SetRemoveAction(Constraint::Bake);
  DALI_TEST_EQUALS((unsigned int)constraint.GetRemoveAction(), (unsigned int)Constraint::Bake, TEST_LOCATION);
  END_TEST;
}

/**
 * Test a constraint with non-zero apply-time & zero (immediate) remove-time, where the constraint is removed during the apply-time
 */
int UtcDaliConstraintImmediateRemoveDuringApply(void)
{
  TestApplication application;

  Vector3 sourcePosition(0.0f, 0.0f, 0.0f);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);

  // Build constraint

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION, TestPositionConstraint(targetPosition) );
  DALI_TEST_EQUALS((unsigned int)constraint.GetRemoveAction(), (unsigned int)Constraint::Bake, TEST_LOCATION);

  float applySeconds(4.0f);
  constraint.SetApplyTime(applySeconds);
  DALI_TEST_EQUALS(constraint.GetApplyTime(), TimePeriod(applySeconds), TEST_LOCATION);

  // Apply to an actor

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  actor.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), sourcePosition, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(applySeconds*250.0f)/* 25% progress */);

  // Constraint shouldn't be fully applied yet
  Vector3 twentyFivePercent( targetPosition * 0.25f );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), twentyFivePercent, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(applySeconds*250.0f)/* 50% progress */);

  // Constraint shouldn't be fully applied yet
  Vector3 fiftyPercent( targetPosition * 0.5f );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), fiftyPercent, TEST_LOCATION );

  // Remove from the actor

  actor.RemoveConstraints(); // should go back to source position
  application.SendNotification();

  // Constraint should be fully removed
  application.Render(static_cast<unsigned int>(200.0f /*0.2 seconds*/));
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), sourcePosition, TEST_LOCATION );

  // Constraint should still be fully applied
  application.Render(static_cast<unsigned int>(200.0f /*0.2 seconds*/));
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), sourcePosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), sourcePosition, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), sourcePosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintActorSize(void)
{
  TestApplication application;

  // Build constraint, to make child 20% of parent size

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE,
                                                    ParentSource( Actor::SIZE ),
                                                    TestRelativeConstraintVector3(0.2f) );
  // Apply to a child actor

  Actor parent = Actor::New();
  Stage::GetCurrent().Add(parent);

  Actor child = Actor::New();
  parent.Add(child);

  child.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( child.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Animate the parent between two sizes

  Vector3 targetParentSize(100.0f, 100.0f, 100.0f);

  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property(parent, Actor::SIZE), targetParentSize );
  animation.Play();

  application.SendNotification();

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.25f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize(),  targetParentSize*0.25f * 0.2f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.5f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize(),  targetParentSize*0.5f * 0.2f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.75f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize(),  targetParentSize*0.75f * 0.2f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 100% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize(),  targetParentSize * 0.2f, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize(),  targetParentSize * 0.2f, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize(),  targetParentSize * 0.2f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintActorSizeWidth(void)
{
  TestApplication application;

  // Build constraint, to make child 20% of parent width

  Constraint constraint = Constraint::New<float>( Actor::SIZE_WIDTH,
                                                  ParentSource( Actor::SIZE_WIDTH ),
                                                  TestRelativeConstraintFloat(0.2f) );
  // Apply to a child actor

  Actor parent = Actor::New();
  Stage::GetCurrent().Add(parent);

  Actor child = Actor::New();
  parent.Add(child);

  child.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( child.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Animate the parent between two sizes

  Vector3 targetParentSize(80.0f, 90.0f, 100.0f);

  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property(parent, Actor::SIZE), targetParentSize );
  animation.Play();

  application.SendNotification();

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.25f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  targetParentSize.width*0.25f * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.5f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  targetParentSize.width*0.5f * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.75f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  targetParentSize.width*0.75f * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 100% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  targetParentSize.width * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  targetParentSize.width * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );

  application.Render(0);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  targetParentSize.width * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintActorSizeHeight(void)
{
  TestApplication application;

  // Build constraint, to make child 20% of parent height

  Constraint constraint = Constraint::New<float>( Actor::SIZE_HEIGHT,
                                                  ParentSource( Actor::SIZE_HEIGHT ),
                                                  TestRelativeConstraintFloat(0.2f) );
  // Apply to a child actor

  Actor parent = Actor::New();
  Stage::GetCurrent().Add(parent);

  Actor child = Actor::New();
  parent.Add(child);

  child.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( child.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Animate the parent between two sizes

  Vector3 targetParentSize(80.0f, 90.0f, 100.0f);

  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property(parent, Actor::SIZE), targetParentSize );
  animation.Play();

  application.SendNotification();

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.25f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, targetParentSize.height*0.25f * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.5f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, targetParentSize.height*0.5f * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.75f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, targetParentSize.height*0.75f * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 100% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, targetParentSize.height * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, targetParentSize.height * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );

  application.Render(0);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, targetParentSize.height * 0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  0.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintActorSizeDepth(void)
{
  TestApplication application;

  // Build constraint, to make child 20% of parent height

  Constraint constraint = Constraint::New<float>( Actor::SIZE_DEPTH,
                                                  ParentSource( Actor::SIZE_DEPTH ),
                                                  TestRelativeConstraintFloat(0.2f) );
  // Apply to a child actor

  Actor parent = Actor::New();
  Stage::GetCurrent().Add(parent);

  Actor child = Actor::New();
  parent.Add(child);

  child.ApplyConstraint( constraint );
  DALI_TEST_EQUALS( child.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Animate the parent between two sizes

  Vector3 targetParentSize(80.0f, 90.0f, 100.0f);

  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property(parent, Actor::SIZE), targetParentSize );
  animation.Play();

  application.SendNotification();

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.25f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  targetParentSize.depth*0.25f * 0.2f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.5f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  targetParentSize.depth*0.5f * 0.2f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize*0.75f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  targetParentSize.depth*0.75f * 0.2f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 100% progress */);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  targetParentSize.depth * 0.2f, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  targetParentSize.depth * 0.2f, TEST_LOCATION );

  application.Render(0);
  DALI_TEST_EQUALS( parent.GetCurrentSize(), targetParentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().width,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().height, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentSize().depth,  targetParentSize.depth * 0.2f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintInputWorldPosition(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentPosition( 10.0f, 10.0f, 10.0f );
  parent.SetPosition( parentPosition );
  parent.SetParentOrigin( ParentOrigin::CENTER );
  parent.SetAnchorPoint( AnchorPoint::CENTER );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  child.SetParentOrigin( ParentOrigin::CENTER );
  child.SetAnchorPoint( AnchorPoint::CENTER );
  Vector3 childPosition( 10.0f, 10.0f, 10.0f );
  child.SetPosition( childPosition );
  parent.Add( child );

  Actor trackingActor = Actor::New();
  trackingActor.SetParentOrigin( ParentOrigin::CENTER );
  trackingActor.SetAnchorPoint( AnchorPoint::CENTER );
  Stage::GetCurrent().Add( trackingActor );

  // The actors should not have a world position yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( trackingActor.GetCurrentWorldPosition(), Vector3::ZERO, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( trackingActor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );
  Vector3 previousPosition( parentPosition + childPosition );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), previousPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( trackingActor.GetCurrentWorldPosition(), Vector3::ZERO, TEST_LOCATION );

  // Build constraint, to make actor track the world-position of another actor
  // Note that the world-position is always from the previous frame, so the tracking actor will lag behind

  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    Source( child, Actor::WORLD_POSITION ),
                                                    EqualToConstraint() );

  trackingActor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( trackingActor.GetCurrentPosition(), previousPosition, TEST_LOCATION );

  // Move the actors and try again
  Vector3 relativePosition( 5, 5, 5 );
  parent.MoveBy( relativePosition );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition + relativePosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childPosition, TEST_LOCATION );

  // The tracking actor lags behind
  DALI_TEST_EQUALS( trackingActor.GetCurrentPosition(), previousPosition, TEST_LOCATION );

  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition + relativePosition, TEST_LOCATION );
  previousPosition = Vector3( parentPosition + childPosition + relativePosition );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), previousPosition, TEST_LOCATION );

  // Allow the tracking actor to catch up
  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition + relativePosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childPosition, TEST_LOCATION );

  // The tracking actor catches up!
  DALI_TEST_EQUALS( trackingActor.GetCurrentPosition(), previousPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition + relativePosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), previousPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintInputWorldRotation(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Radian rotationAngle( Degree(90.0f) );
  Quaternion rotation( rotationAngle, Vector3::YAXIS );
  parent.SetRotation( rotation );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  child.SetRotation( rotation );
  parent.Add( child );

  Actor trackingActor = Actor::New();
  Stage::GetCurrent().Add( trackingActor );

  // The actors should not have a world rotation yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldRotation(), Quaternion(0.0f, Vector3::YAXIS), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldRotation(), Quaternion(0.0f, Vector3::YAXIS), 0.001, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentRotation(), rotation, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentRotation(), rotation, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( trackingActor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), 0.001, TEST_LOCATION );

  DALI_TEST_EQUALS( parent.GetCurrentWorldRotation(), Quaternion( rotationAngle, Vector3::YAXIS ), 0.001, TEST_LOCATION );
  Quaternion previousRotation( rotationAngle * 2.0f, Vector3::YAXIS );
  DALI_TEST_EQUALS( child.GetCurrentWorldRotation(), previousRotation, 0.001, TEST_LOCATION );

  // Build constraint, to make actor track the world-rotation of another actor
  // Note that the world-rotation is always from the previous frame, so the tracking actor will lag behind

  Constraint constraint = Constraint::New<Quaternion>( Actor::ROTATION,
                                                       Source( child, Actor::WORLD_ROTATION ),
                                                       EqualToQuaternion() );

  trackingActor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( trackingActor.GetCurrentRotation(), previousRotation, 0.001, TEST_LOCATION );

  // Rotate the actors and try again
  parent.RotateBy( rotation );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentRotation(), rotation * rotation, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentRotation(), rotation, 0.001, TEST_LOCATION );

  // The tracking actor lags behind
  DALI_TEST_EQUALS( trackingActor.GetCurrentRotation(), previousRotation, 0.001, TEST_LOCATION );

  DALI_TEST_EQUALS( parent.GetCurrentWorldRotation(), Quaternion( rotationAngle * 2.0f, Vector3::YAXIS ), 0.001, TEST_LOCATION );
  previousRotation = Quaternion( rotationAngle * 3.0f, Vector3::YAXIS );
  DALI_TEST_EQUALS( child.GetCurrentWorldRotation(), previousRotation, 0.001, TEST_LOCATION );

  // Allow the tracking actor to catch up
  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentRotation(), rotation * rotation, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentRotation(), rotation, 0.001, TEST_LOCATION );

  // The tracking actor catches up!
  DALI_TEST_EQUALS( trackingActor.GetCurrentRotation(), previousRotation, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldRotation(), Quaternion( rotationAngle * 2.0f, Vector3::YAXIS ), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldRotation(), previousRotation, 0.001, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintInputWorldScale(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentScale( 2.0f, 2.0f, 2.0f );
  parent.SetScale( parentScale );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  Vector3 childScale( 1.0f, 2.0f, 3.0f );
  child.SetScale( childScale );
  parent.Add( child );

  Actor trackingActor = Actor::New();
  Stage::GetCurrent().Add( trackingActor );

  // The actors should not have a world scale yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldScale(), Vector3::ONE, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldScale(), Vector3::ONE, TEST_LOCATION );
  DALI_TEST_EQUALS( trackingActor.GetCurrentWorldScale(), Vector3::ONE, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentScale(), parentScale, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentScale(), childScale, TEST_LOCATION );
  DALI_TEST_EQUALS( trackingActor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  DALI_TEST_EQUALS( parent.GetCurrentWorldScale(), parentScale, TEST_LOCATION );
  Vector3 previousScale( parentScale * childScale );
  DALI_TEST_EQUALS( child.GetCurrentWorldScale(), previousScale, TEST_LOCATION );
  DALI_TEST_EQUALS( trackingActor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  // Build constraint, to make actor track the world-scale of another actor
  // Note that the world-scale is always from the previous frame, so the tracking actor will lag behind

  Constraint constraint = Constraint::New<Vector3>( Actor::SCALE,
                                                    Source( child, Actor::WORLD_SCALE ),
                                                    EqualToConstraint() );

  trackingActor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( trackingActor.GetCurrentScale(), previousScale, TEST_LOCATION );

  // Scale the actors and try again
  Vector3 relativeScale( 3, 3, 3 );
  parent.ScaleBy( relativeScale );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentScale(), parentScale * relativeScale, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentScale(), childScale, TEST_LOCATION );

  // The tracking actor lags behind
  DALI_TEST_EQUALS( trackingActor.GetCurrentScale(), previousScale, TEST_LOCATION );

  DALI_TEST_EQUALS( parent.GetCurrentWorldScale(), parentScale * relativeScale, TEST_LOCATION );
  previousScale = Vector3( parentScale * childScale * relativeScale );
  DALI_TEST_EQUALS( child.GetCurrentWorldScale(), previousScale, TEST_LOCATION );

  // Allow the tracking actor to catch up
  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentScale(), parentScale * relativeScale, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentScale(), childScale, TEST_LOCATION );

  // The tracking actor catches up!
  DALI_TEST_EQUALS( trackingActor.GetCurrentScale(), previousScale, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldScale(), parentScale * relativeScale, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldScale(), previousScale, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintInputWorldColor(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector4 parentColor( 1.0f, 0.5f, 0.0f, 1.0f );
  parent.SetColor( parentColor );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  Vector4 childColor( 0.5f, 0.5f, 0.5f, 1.0f );
  child.SetColor( childColor );
  parent.Add( child );

  Actor trackingActor = Actor::New();
  Stage::GetCurrent().Add( trackingActor );

  // The actors should not have a world color yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldColor(), Color::WHITE, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldColor(), Color::WHITE, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentColor(), parentColor, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentColor(), childColor, TEST_LOCATION );
  DALI_TEST_EQUALS( trackingActor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );

  DALI_TEST_EQUALS( parent.GetCurrentWorldColor(), parentColor, TEST_LOCATION );
  Vector4 previousColor( childColor );
  previousColor.a *= parentColor.a;
  DALI_TEST_EQUALS( child.GetCurrentWorldColor(), previousColor, TEST_LOCATION );

  // Build constraint, to make actor track the world-color of another actor
  // Note that the world-color is always from the previous frame, so the tracking actor will lag behind

  Constraint constraint = Constraint::New<Vector4>( Actor::COLOR,
                                                    Source( child, Actor::WORLD_COLOR ),
                                                    EqualToVector4() );

  trackingActor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( trackingActor.GetCurrentColor(), previousColor, TEST_LOCATION );

  // Set the color and try again
  Vector4 newChildColor( 0.75f, 0.75f, 0.75f, 1.0f );
  child.SetColor( newChildColor );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentColor(), parentColor, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentColor(), newChildColor, TEST_LOCATION );

  // The tracking actor lags behind
  DALI_TEST_EQUALS( trackingActor.GetCurrentColor(), previousColor, TEST_LOCATION );

  DALI_TEST_EQUALS( parent.GetCurrentWorldColor(), parentColor, TEST_LOCATION );
  previousColor = Vector3( newChildColor );
  DALI_TEST_EQUALS( child.GetCurrentWorldColor(), previousColor, TEST_LOCATION );

  // Allow the tracking actor to catch up
  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentColor(), parentColor, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentColor(), newChildColor, TEST_LOCATION );

  // The tracking actor catches up!
  DALI_TEST_EQUALS( trackingActor.GetCurrentColor(), previousColor, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldColor(), parentColor, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldColor(), previousColor, TEST_LOCATION );
  END_TEST;
}

int UtcDaliConstraintInvalidInputProperty(void)
{
  TestApplication application;
  Actor actor = Actor::New();
  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION, LocalSource( PROPERTY_REGISTRATION_START_INDEX ), EqualToConstraint() );

  Stage::GetCurrent().Add( actor );

  // Cannot use type registered properties as input to constraints
  try
  {
    actor.ApplyConstraint( constraint );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "mTargetObject->IsPropertyAConstraintInput( source.propertyIndex )", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliBuiltinConstraintParentSize(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentSize(9,9,9);
  parent.SetSize( parentSize );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  parent.Add( actor );

  Vector3 startValue( Vector3::ZERO );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentSize() == startValue );

  // Apply constraint

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetCurrentSize(), parentSize, TEST_LOCATION );

  // This should be ignored
  actor.SetSize( startValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), parentSize, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), parentSize, TEST_LOCATION );

  // Remove the constraint, then set new value
  actor.RemoveConstraints();
  actor.SetSize( startValue );

  // Constraint should have been removed
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBuiltinConstraintParentSizeRelative(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentSize(9,9,9);
  parent.SetSize( parentSize );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  parent.Add( actor );

  Vector3 startValue( Vector3::ZERO );
  Vector3 scale(2,3,4);
  Vector3 endValue( parentSize * scale );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentSize() == startValue );

  // Apply constraint

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), RelativeToConstraint( scale ) );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetCurrentSize(), endValue, TEST_LOCATION );

  // This should be ignored
  actor.SetSize( startValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), endValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), endValue, TEST_LOCATION );

  // Remove the constraint, then set new value
  actor.RemoveConstraints();
  actor.SetSize( startValue );

  // Constraint should have been removed
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), startValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBuiltinConstraintScaleToFitConstraint(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 startParentSize( 10, 10, 10 );
  parent.SetSize( startParentSize );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  Vector3 startChildSize( 5, 5, 5 );
  actor.SetSize( startChildSize );
  parent.Add( actor );

  Vector3 endChildSize( 8, 8, 8 );
  Vector3 endParentSize( 4, 4, 4 );
  Vector3 startChildScale( 2, 2, 2 ); // startParentSize / startChildSize
  Vector3 intermediateChildScale( 1.25, 1.25, 1.25 ); // startParentSize / endChildSize
  Vector3 endChildScale( 0.5, 0.5, 0.5 ); // endParentSize / endChildSize

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentSize() == startChildSize );

  // Apply constraint

  Constraint constraint = Constraint::New<Vector3>( Actor::SCALE,
                                                    LocalSource( Actor::SIZE ),
                                                    ParentSource( Actor::SIZE ),
                                                    ScaleToFitConstraint() );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied, but parent size is larger than child
  DALI_TEST_EQUALS( actor.GetCurrentSize(), startChildSize, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), startChildScale, TEST_LOCATION );

  // This should be allowed (still less than parent size)
  actor.SetSize( endChildSize );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), endChildSize, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), intermediateChildScale, TEST_LOCATION );

  // Reduce the parent size
  parent.SetSize( endParentSize );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor.GetCurrentSize(), endChildSize, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), endChildScale, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), endChildSize, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), endChildScale, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBuiltinConstraintScaleToFitKeepAspectRatio(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentSize1( 10, 10, 10 );
  parent.SetSize( parentSize1 );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  Vector3 childSize( 4, 5, 5 );
  actor.SetSize( childSize );
  parent.Add( actor );

  application.SendNotification();
  application.Render(0);
  Vector3 childScale1( 1.0f, 1.0f, 1.0f );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), childScale1, TEST_LOCATION );

  // Apply constraint

  Constraint constraint = Constraint::New<Vector3>( Actor::SCALE,
                                                    LocalSource( Actor::SIZE ),
                                                    ParentSource( Actor::SIZE ),
                                                    ScaleToFitKeepAspectRatioConstraint() );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied, but parent size is larger than child
  Vector3 childScale2( 2.0f, 2.0f, 2.0f );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), childScale2, TEST_LOCATION );

  // change parent size
  Vector3 parentSize2( 40, 50, 50 );
  parent.SetSize( parentSize2 );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied, but parent size is larger than child
  Vector3 childScale3( 10.0f, 10.0f, 10.0f );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), childScale3, TEST_LOCATION );
  END_TEST;
}


int UtcDaliBuiltinConstraintScaleToFillXYKeepAspectRatio(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentSize1( 10, 10, 10 );
  parent.SetSize( parentSize1 );
  Stage::GetCurrent().Add( parent );

  Actor actor = Actor::New();
  Vector3 childSize( 4, 5, 5 );
  actor.SetSize( childSize );
  parent.Add( actor );

  application.SendNotification();
  application.Render(0);
  Vector3 childScale1( 1.0f, 1.0f, 1.0f );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), childScale1, TEST_LOCATION );

  // Apply constraint

  Constraint constraint = Constraint::New<Vector3>( Actor::SCALE,
                                                    LocalSource( Actor::SIZE ),
                                                    ParentSource( Actor::SIZE ),
                                                    ScaleToFillXYKeepAspectRatioConstraint() );
  actor.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied, but parent size is larger than child
  float val = 10.f / 4.f;
  Vector3 childScale2( val, val, val );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), childScale2, TEST_LOCATION );

  // change parent size
  Vector3 parentSize2( 40, 50, 50 );
  parent.SetSize( parentSize2 );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied, but parent size is larger than child
  Vector3 childScale3( 10.0f, 10.0f, 10.0f );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), childScale3, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBuiltinConstraintEqualToConstraint(void)
{
  TestApplication application;

  Actor actor1 = Actor::New();
  Vector3 startPosition( 10, 10, 10 );
  actor1.SetPosition( startPosition );
  Stage::GetCurrent().Add( actor1 );

  Actor actor2 = Actor::New();
  Vector3 startSize( 100, 100, 100 );
  actor2.SetSize( startSize );
  Stage::GetCurrent().Add( actor2 );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor1.GetCurrentPosition() == startPosition );
  DALI_TEST_CHECK( actor2.GetCurrentSize()     == startSize );

  // Apply constraint - actor1 size == actor2 position

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE,
                                                    Source( actor1, Actor::POSITION ),
                                                    EqualToConstraint() );
  constraint.SetRemoveAction( Constraint::Discard );
  actor2.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor2.GetCurrentSize(), startPosition, TEST_LOCATION );

  // Change the input
  Vector3 endPosition( 2, 2, 2 );
  actor1.SetPosition( endPosition );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor2.GetCurrentSize(), endPosition, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor2.GetCurrentSize(), endPosition, TEST_LOCATION );

  //
  // Check float variant of constraint
  //
  float startOpacity(1.0f);
  float endOpacity(0.2f);
  actor1.SetOpacity( startOpacity );
  actor2.SetOpacity( startOpacity );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor1.GetCurrentOpacity(), startOpacity, TEST_LOCATION );
  DALI_TEST_EQUALS( actor2.GetCurrentOpacity(), startOpacity, TEST_LOCATION );

  Constraint constraint2 = Constraint::New<float>( Actor::COLOR_ALPHA,
                                                  Source( actor1, Actor::COLOR_ALPHA ),
                                                  EqualToConstraint() );
  constraint2.SetRemoveAction( Constraint::Discard );
  actor2.ApplyConstraint( constraint2 );

  actor1.SetOpacity(endOpacity);

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor2.GetCurrentOpacity(), endOpacity, 0.000001f, TEST_LOCATION );

  //
  // Check Vector4 variant of constraint
  //
  actor1.SetColor( Color::GREEN );
  actor2.SetColor( Color::RED );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor1.GetCurrentColor() == Color::GREEN );
  DALI_TEST_CHECK( actor2.GetCurrentColor() == Color::RED );

  Constraint constraint3 = Constraint::New<Vector4>( Actor::COLOR,
                                                    Source( actor1, Actor::COLOR ),
                                                    EqualToConstraint() );
  constraint3.SetRemoveAction( Constraint::Discard );
  actor2.ApplyConstraint( constraint3 );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor2.GetCurrentColor() == Color::GREEN );

  //
  // Check Quaternion variant of constraint
  //
  Quaternion q1 = Quaternion( Math::PI_2, Vector3::XAXIS );
  Quaternion q2 = Quaternion( Math::PI_4, Vector3::YAXIS );
  actor1.SetRotation( q1 );
  actor2.SetRotation( q2 );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor1.GetCurrentRotation(), q1, 0.01, TEST_LOCATION );
  DALI_TEST_EQUALS( actor2.GetCurrentRotation(), q2, 0.01, TEST_LOCATION );

  Constraint constraint4 = Constraint::New<Quaternion>( Actor::ROTATION,
                                                    Source( actor1, Actor::ROTATION ),
                                                    EqualToConstraint() );
  constraint4.SetRemoveAction( Constraint::Discard );
  actor2.ApplyConstraint( constraint4 );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor2.GetCurrentRotation(), q1, 0.01, TEST_LOCATION );

  //
  // Check Matrix3 variant
  //
  EqualToConstraint equalToConstraint;

  Matrix3 a;
  a.AsFloat()[0] = 1.f;
  Matrix3 b;
  b.AsFloat()[0] = 2.f;
  PropertyInputAbstraction pi(b);

  Matrix3 c = equalToConstraint(a,pi);
  DALI_TEST_EQUALS( c.AsFloat()[0], b.AsFloat()[0], 0.01, TEST_LOCATION);
  END_TEST;
}

int UtcDaliBuiltinConstraintRelativeToConstraint(void)
{
  TestApplication application;

  Actor actor1 = Actor::New();
  Vector3 startPosition( 10, 10, 10 );
  actor1.SetPosition( startPosition );
  Stage::GetCurrent().Add( actor1 );

  Actor actor2 = Actor::New();
  Vector3 startSize( 100, 100, 100 );
  actor2.SetSize( startSize );
  Stage::GetCurrent().Add( actor2 );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor1.GetCurrentPosition() == startPosition );
  DALI_TEST_CHECK( actor2.GetCurrentSize()     == startSize );

  // Apply constraint - actor1 size == actor2 position

  RelativeToConstraint( 0.f );
  Vector3 scale( 0.5, 0.6, 0.7 );
  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE,
                                                    Source( actor1, Actor::POSITION ),
                                                    RelativeToConstraint( scale ) );
  constraint.SetRemoveAction( Constraint::Discard );
  actor2.ApplyConstraint( constraint );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor2.GetCurrentSize(), scale * startPosition, TEST_LOCATION );

  // Change the input
  Vector3 endPosition( 2, 2, 2 );
  actor1.SetPosition( endPosition );

  application.SendNotification();
  application.Render(0);

  // Constraint should be fully applied
  DALI_TEST_EQUALS( actor2.GetCurrentSize(), scale * endPosition, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor2.GetCurrentSize(), scale * endPosition, TEST_LOCATION );

  //
  // Check float variant of constraint
  //
  float scale2( 0.5f );
  float startOpacity(1.0f);
  actor1.SetOpacity( startOpacity );
  actor2.SetOpacity( startOpacity );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor1.GetCurrentOpacity(), startOpacity, TEST_LOCATION );
  DALI_TEST_EQUALS( actor2.GetCurrentOpacity(), startOpacity, TEST_LOCATION );

  Constraint constraint2 = Constraint::New<float>( Actor::COLOR_ALPHA,
                                                  Source( actor1, Actor::COLOR_ALPHA ),
                                                  RelativeToConstraintFloat(scale2) );
  constraint2.SetRemoveAction( Constraint::Discard );
  actor2.ApplyConstraint(constraint2);
  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor2.GetCurrentOpacity(), startOpacity * scale2, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBuiltinConstraintFunctions(void)
{
  TestApplication application;

  {
    SourceWidthFixedHeight sourceWidthFixedHeight( 10.f );
    Vector3 current;
    {
      Vector3 reference(1, 10, 0);
      Vector3 value = sourceWidthFixedHeight( current, PropertyInputAbstraction(Vector3::ONE) );
      DALI_TEST_EQUALS( reference, value, TEST_LOCATION );
    }
    {
      Vector3 reference(10, 10, 0);
      Vector3 value = sourceWidthFixedHeight( current, PropertyInputAbstraction(Vector3::ONE*10.f) );
      DALI_TEST_EQUALS( reference, value, TEST_LOCATION );
    }
    {
      Vector3 reference(10,10,0);
      Vector3 value = sourceWidthFixedHeight( current, PropertyInputAbstraction(Vector3::ONE*10.f) );
      DALI_TEST_EQUALS( reference, value, TEST_LOCATION );
    }
  }

  { // LookAt
    Quaternion current(0, Vector3::YAXIS);
    PropertyInputAbstraction target(Vector3::ZAXIS);
    PropertyInputAbstraction targetRotation(Vector3::YAXIS);
    PropertyInputAbstraction camera(Vector3::ZERO);

    {
      Quaternion reference(1., 0., 0., 0.);
      Quaternion value = LookAt( current, target, camera, targetRotation );
      DALI_TEST_EQUALS( reference, value, 0.001, TEST_LOCATION );
    }

  }

  END_TEST;
}
