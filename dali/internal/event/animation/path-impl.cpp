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
 *
 */

// CLASS HEADER
#include <dali/internal/event/animation/path-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/property-helper.h>

namespace Dali
{

namespace Internal
{

namespace
{

// Properties

//              Name             Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "points",         ARRAY, true, false, false,   Dali::Path::Property::POINTS         )
DALI_PROPERTY( "controlPoints",  ARRAY, true, false, false,   Dali::Path::Property::CONTROL_POINTS )
DALI_PROPERTY_TABLE_END( DEFAULT_OBJECT_PROPERTY_START_INDEX, PathDefaultProperties )

/**
 * These coefficient arise from the cubic polynomial equations for
 * a bezier curve.
 *
 * A bezier curve is defined by a cubic polynomial. Given two end points p0 and p1
 * and two control points cp0 and cp1, the bezier curve will be defined by a polynomial in the form
 * f(x) = a3*x^3 + a2*x^2 + a1*x + a0 with this restrictions:
 * f(0) = p0
 * f(1) = p1
 * f'(0) = 3*(cp0 - p0)
 * f'(1) = 3*(p1-cp1)
 */
const float BezierBasisCoeff[] = {  -1.0f,  3.0f, -3.0f, 1.0f,
                                     3.0f, -6.0f,  3.0f, 0.0f,
                                    -3.0f,  3.0f,  0.0f, 0.0f,
                                     1.0f,  0.0f,  0.0f, 0.0f  };

const Dali::Matrix BezierBasis = Dali::Matrix( BezierBasisCoeff );

Dali::BaseHandle Create()
{
  return Dali::Path::New();
}

TypeRegistration mType( typeid(Dali::Path), typeid(Dali::Handle), Create, PathDefaultProperties );

inline bool PathIsComplete(const Dali::Vector<Vector3>& point, const Dali::Vector<Vector3>& controlPoint)
{
  return ( point.Size() > 1 && controlPoint.Size() == (point.Size()-1)*2 );
}

} //Unnamed namespace

Path* Path::New()
{
  return new Path();
}

Path::Path()
: Object( nullptr ) // we don't have our own scene object
{
}

Path::~Path() = default;

Path* Path::Clone(const Path& path)
{
  Path* clone = new Path();
  clone->SetPoints( path.GetPoints() );
  clone->SetControlPoints( path.GetControlPoints() );

  return clone;
}

Property::Value Path::GetDefaultProperty( Property::Index index ) const
{
  if( index == Dali::Path::Property::POINTS )
  {
    Property::Value value( Property::ARRAY );
    Property::Array* array = value.GetArray();
    Property::Array::SizeType pointCount = mPoint.Count();

    if( array )
    {
      array->Reserve( pointCount );
      for( Property::Array::SizeType i = 0; i < pointCount; ++i )
      {
        array->PushBack( mPoint[i] );
      }
    }
    return value;
  }
  else if( index == Dali::Path::Property::CONTROL_POINTS )
  {
    Property::Value value( Property::ARRAY );
    Property::Array* array = value.GetArray();
    Property::Array::SizeType  controlpointCount = mControlPoint.Count();

    if( array )
    {
      array->Reserve( controlpointCount );
      for( Property::Array::SizeType i = 0; i < controlpointCount; ++i )
      {
        array->PushBack( mControlPoint[i] );
      }
    }
    return value;
  }

  return Property::Value();
}

void Path::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  const Property::Array* array = propertyValue.GetArray();
  if( array )
  {
    Property::Array::SizeType propertyArrayCount = array->Count();
    if( index == Dali::Path::Property::POINTS )
    {
      mPoint.Reserve( propertyArrayCount );
      for( Property::Array::SizeType i = 0; i < propertyArrayCount; ++i )
      {
        Vector3 point;
        array->GetElementAt( i ).Get( point );
        mPoint.PushBack( point );
      }
    }
    else if( index == Dali::Path::Property::CONTROL_POINTS )
    {
      mControlPoint.Reserve( propertyArrayCount );
      for( Property::Array::SizeType i = 0; i < propertyArrayCount; ++i )
      {
        Vector3 point;
        array->GetElementAt( i ).Get( point );
        mControlPoint.PushBack( point );
      }
    }
  }
}

void Path::AddPoint(const Vector3& point )
{
  mPoint.PushBack( point );
}

void Path::AddControlPoint(const Vector3& point )
{
  mControlPoint.PushBack( point );
}

uint32_t Path::GetNumberOfSegments() const
{
  return static_cast<uint32_t>( (mPoint.Size()>1) ? mPoint.Size()-1 : 0 );
}

void Path::GenerateControlPoints( float curvature )
{
  uint32_t numSegments = GetNumberOfSegments();
  DALI_ASSERT_ALWAYS( numSegments > 0 && "Need at least 1 segment to generate control points" ); // need at least 1 segment

  mControlPoint.Resize( numSegments * 2);

  //Generate two control points for each segment
  for( uint32_t i(0); i<numSegments; ++i )
  {
    //Segment end-points
    Vector3 p1 = mPoint[i];
    Vector3 p2 = mPoint[i+1];

    Vector3 p0;
    if( i == 0 )
    {
      //There's no previous point. We chose a point in the line defined by the two end points  at
      //a 1/8th of the distance between them.
      p0 = p1 - (p2 - p1)/8.0f;
    }
    else
    {
      //Previous point
      p0 = mPoint[i-1];
    }

    Vector3 p3;
    if( i == numSegments - 1)
    {
      //There's no next point. We chose a point in the line defined by the two end points  at
      //a 1/8th of the distance between them.
      p3 = p2 - (p1 - p2)/8.0f;
    }
    else
    {
      //Next point
      p3 = mPoint[i+2];
    }

    Vector3 p0p1 = p1 - p0;
    Vector3 p1p2 = p2 - p1;
    Vector3 p2p3 = p3 - p2;

    float length = p1p2.Length();

    Vector3 tangentOut = ( p0p1*length + p1p2*p0p1.Length() ) * 0.5f;
    tangentOut.Normalize();

    Vector3 tangentIn = ( p1p2*p2p3.Length() + p2p3*length ) * 0.5f;
    tangentIn.Normalize();

    //Use curvature to scale the tangents
    length *= curvature;
    mControlPoint[2*i] =   p1 + tangentOut*length;
    mControlPoint[2*i+1] = p2 - tangentIn*length;
  }
}

void Path::FindSegmentAndProgress( float t, uint32_t& segment, float& tLocal ) const
{
  //Find segment and local progress
  uint32_t numSegs = GetNumberOfSegments();

  if( t <= 0.0f || numSegs == 0 )
  {
    segment = 0;
    tLocal = 0.0f;
  }
  else if( t >= 1.0f )
  {
    segment = numSegs-1;
    tLocal = 1.0f;
  }
  else
  {
    segment = static_cast<uint32_t>( t * static_cast<float>( numSegs ) );
    float segLength = 1.0f / static_cast<float>( numSegs );
    float segStart  = static_cast<float>( segment ) * segLength;
    tLocal = (t - segStart) * static_cast<float>( numSegs );
  }
}

void Path::Sample( float t, Vector3& position, Vector3& tangent ) const
{
  if( !SampleAt(t, position, tangent) )
  {
    DALI_ASSERT_ALWAYS(!"Spline not fully initialized" );
  }
}

bool Path::SampleAt( float t, Vector3& position, Vector3& tangent ) const
{
  bool done = false;

  if( PathIsComplete(mPoint, mControlPoint) )
  {
    uint32_t segment;
    float tLocal;
    FindSegmentAndProgress( t, segment, tLocal );

    //Get points and control points in the segment
    const Vector3& controlPoint0 = mControlPoint[2*segment];
    const Vector3& controlPoint1 = mControlPoint[2*segment+1];
    const Vector3& point0 = mPoint[segment];
    const Vector3& point1 = mPoint[segment+1];

    if(tLocal < Math::MACHINE_EPSILON_1)
    {
      position = point0;
      tangent = ( controlPoint0 - point0 ) * 3.0f;
      tangent.Normalize();
    }
    else if( (1.0 - tLocal) < Math::MACHINE_EPSILON_1)
    {
      position = point1;
      tangent = ( point1 - controlPoint1 ) * 3.0f;
      tangent.Normalize();
    }
    else
    {
      const Vector4 sVect(tLocal*tLocal*tLocal, tLocal*tLocal, tLocal, 1.0f );
      const Vector3 sVectDerivative(3.0f*tLocal*tLocal, 2.0f*tLocal, 1.0f );

      //X
      Vector4  cVect( point0.x, controlPoint0.x, controlPoint1.x,  point1.x);

      Vector4 A = BezierBasis * cVect;
      position.x = sVect.Dot4(A);
      tangent.x  = sVectDerivative.Dot(Vector3(A));

      //Y
      cVect.x  = point0.y;
      cVect.y  = controlPoint0.y;
      cVect.z  = controlPoint1.y;
      cVect.w  = point1.y;

      A = BezierBasis * cVect;
      position.y = sVect.Dot4(A);
      tangent.y  = sVectDerivative.Dot(Vector3(A));

      //Z
      cVect.x  = point0.z;
      cVect.y  = controlPoint0.z;
      cVect.z  = controlPoint1.z;
      cVect.w  = point1.z;

      A = BezierBasis * cVect;
      position.z = sVect.Dot4(A);
      tangent.z  = sVectDerivative.Dot(Vector3(A));

      tangent.Normalize();
    }

    done = true;
  }

  return done;
}

bool Path::SamplePosition( float t, Vector3& position ) const
{
  bool done = false;

  if( PathIsComplete(mPoint, mControlPoint) )
  {
    uint32_t segment;
    float tLocal;
    FindSegmentAndProgress( t, segment, tLocal );

    const Vector3& controlPoint0 = mControlPoint[2*segment];
    const Vector3& controlPoint1 = mControlPoint[2*segment+1];
    const Vector3& point0 = mPoint[segment];
    const Vector3& point1 = mPoint[segment+1];

    if(tLocal < Math::MACHINE_EPSILON_1)
    {
      position = point0;
    }
    else if( (1.0 - tLocal) < Math::MACHINE_EPSILON_1)
    {
      position = point1;
    }
    else
    {
      const Vector4 sVect(tLocal*tLocal*tLocal, tLocal*tLocal, tLocal, 1.0f );

      //X
      Vector4  cVect( point0.x, controlPoint0.x, controlPoint1.x,  point1.x);
      position.x = sVect.Dot4(BezierBasis * cVect);

      //Y
      cVect.x  = point0.y;
      cVect.y  = controlPoint0.y;
      cVect.z  = controlPoint1.y;
      cVect.w  = point1.y;
      position.y = sVect.Dot4(BezierBasis * cVect);

      //Z
      cVect.x  = point0.z;
      cVect.y  = controlPoint0.z;
      cVect.z  = controlPoint1.z;
      cVect.w  = point1.z;
      position.z = sVect.Dot4(BezierBasis * cVect);
    }

    done = true;
  }

  return done;
}

bool Path::SampleTangent( float t, Vector3& tangent ) const
{
  bool done = false;

  if( PathIsComplete(mPoint, mControlPoint) )
  {
    uint32_t segment;
    float tLocal;
    FindSegmentAndProgress( t, segment, tLocal );

    const Vector3& controlPoint0 = mControlPoint[2*segment];
    const Vector3& controlPoint1 = mControlPoint[2*segment+1];
    const Vector3& point0 = mPoint[segment];
    const Vector3& point1 = mPoint[segment+1];

    if(tLocal < Math::MACHINE_EPSILON_1)
    {
      tangent = ( controlPoint0 - point0 ) * 3.0f;
    }
    else if( (1.0f - tLocal) < Math::MACHINE_EPSILON_1)
    {
      tangent = ( point1 - controlPoint1 ) * 3.0f;
    }
    else
    {
      const Vector3 sVectDerivative(3.0f*tLocal*tLocal, 2.0f*tLocal, 1.0f );

      //X
      Vector4  cVect( point0.x, controlPoint0.x, controlPoint1.x,  point1.x);
      tangent.x  = sVectDerivative.Dot(Vector3(BezierBasis * cVect));

      //Y
      cVect.x  = point0.y;
      cVect.y  = controlPoint0.y;
      cVect.z  = controlPoint1.y;
      cVect.w  = point1.y;
      tangent.y  = sVectDerivative.Dot(Vector3(BezierBasis * cVect));

      //Z
      cVect.x  = point0.z;
      cVect.y  = controlPoint0.z;
      cVect.z  = controlPoint1.z;
      cVect.w  = point1.z;
      tangent.z  = sVectDerivative.Dot(Vector3(BezierBasis * cVect));
    }

    tangent.Normalize();
    done = true;
  }

  return done;
}

Vector3& Path::GetPoint( uint32_t index )
{
  DALI_ASSERT_ALWAYS( index < mPoint.Size() && "Path: Point index out of bounds" );

  return mPoint[index];
}

Vector3& Path::GetControlPoint( uint32_t index )
{
  DALI_ASSERT_ALWAYS( index < mControlPoint.Size() && "Path: Control Point index out of bounds" );

  return mControlPoint[index];
}

uint32_t Path::GetPointCount() const
{
  return static_cast<uint32_t>( mPoint.Size() );
}

void Path::ClearPoints()
{
  mPoint.Clear();
}

void Path::ClearControlPoints()
{
  mControlPoint.Clear();
}

} // Internal
} // Dali
