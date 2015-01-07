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
#include <dali/public-api/geometry/spline.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/math-utils.h>

namespace
{
const int MAXIMUM_ITERATIONS=1000;
}

namespace Dali
{

/**
 * These basis matrices arise from the cubic polynomial equations for each
 * spline type. See Collada 1.4.1 specification for more information
 */
const float Spline::BezierBasisA[] = {-1.0f,  3.0f, -3.0f, 1.0f,
                                       3.0f, -6.0f,  3.0f, 0.0f,
                                      -3.0f,  3.0f,  0.0f, 0.0f,
                                       1.0f,  0.0f,  0.0f, 0.0f  };


Spline::Spline()
  : mBasis(BezierBasisA)
{
}

Spline::~Spline()
{
}

void Spline::AddKnot(Vector3 knot)
{
  mKnots.push_back(knot);
  mInTangents.push_back(Vector3());
  mOutTangents.push_back(Vector3());
}


void Spline::SetInTangent(size_t knotIndex, Vector3 inTangent)
{
  DALI_ASSERT_ALWAYS( knotIndex < mInTangents.size() && "knotIndex out of bounds" );
  mInTangents[knotIndex] = inTangent;
}

void Spline::SetOutTangent(size_t knotIndex, Vector3 outTangent)
{
  DALI_ASSERT_ALWAYS( knotIndex < mOutTangents.size() && "knotIndex out of bounds" );
  mOutTangents[knotIndex] = outTangent;
}

Vector3 Spline::GetKnot(size_t knotIndex)
{
  DALI_ASSERT_ALWAYS( knotIndex < mKnots.size() && "knotIndex out of bounds" );
  return mKnots[knotIndex];
}

Vector3 Spline::GetInTangent(size_t knotIndex)
{
  DALI_ASSERT_ALWAYS( knotIndex < mInTangents.size() && "knotIndex out of bounds" );
  return mInTangents[knotIndex];
}

Vector3 Spline::GetOutTangent(size_t knotIndex)
{
  DALI_ASSERT_ALWAYS( knotIndex < mOutTangents.size() && "knotIndex out of bounds" );
  return mOutTangents[knotIndex];
}

void Spline::GenerateControlPoints(float curvature)
{
  size_t numKnots = mKnots.size();
  DALI_ASSERT_ALWAYS( numKnots > 1 && "Need at least 1 segment to generate control points" ); // need at least 1 segment

  for(size_t knotIndex = 0; knotIndex < numKnots; knotIndex++)
  {
    Vector3 curPoint = mKnots[knotIndex];
    Vector3 prevPoint, nextPoint;
    if(knotIndex == 0)
    {
      // create a dummy point
      Vector3 nextPoint = mKnots[1];
      prevPoint = curPoint - (nextPoint - curPoint)/8.0f;
    }
    else
    {
      prevPoint = mKnots[knotIndex-1];
    }

    if(knotIndex == numKnots-1)
    {
      // create a dummy point
      nextPoint = curPoint - (prevPoint - curPoint)/8.0f;
    }
    else
    {
      nextPoint = mKnots[knotIndex+1];
    }

    Vector3 a = curPoint - prevPoint;
    Vector3 b = nextPoint - curPoint;
    float aLength = a.Length();
    float bLength = b.Length();

    Vector3 tangent = (a*bLength + b*aLength)/2.0f;
    tangent.Normalize();

    aLength *= curvature;
    bLength *= curvature;
    mInTangents[knotIndex]  = curPoint - tangent*aLength;
    mOutTangents[knotIndex] = curPoint + tangent*bLength;
  }
}

unsigned int Spline::GetNumberOfSegments() const
{
  return (mKnots.size()>1)?mKnots.size()-1:0;
}


const Vector3 Spline::GetPoint(float alpha) const
{
  Vector3 current;
  unsigned int numSegs = GetNumberOfSegments();
  if(numSegs > 0)
  {
    unsigned int segment = alpha * numSegs;
    float segLength = 1.0f / numSegs;
    float segStart  = (float)segment * segLength;
    float progress = (alpha - segStart) * numSegs;
    if(segment >= numSegs)
    {
      segment = numSegs-1.0f;
      progress = 1.0f;
    }
    current = GetPoint(segment, progress);
  }
  return current;
}


// Given a parameter s, return the point on the given spline segment.
// Checks that the segment index is valid (e.g. for 10 points, there
// are only 9 segments)
const Vector3 Spline::GetPoint(unsigned int segmentIndex, float s) const
{
  DALI_ASSERT_ALWAYS( segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size() && "segmentIndex out of bounds" );
  DALI_ASSERT_ALWAYS( mOutTangents.size() == mKnots.size() && "Spline not fully initialized" );
  DALI_ASSERT_ALWAYS( mInTangents.size()  == mKnots.size() && "Spline not fully initialized" );

  Vector3  bPoint;

  if(s < 0.0f || s > 1.0f)
  {
    bPoint.x = 0.0f;
    bPoint.y = 0.0f;
    bPoint.z = 0.0f;
  }
  else if(s < Math::MACHINE_EPSILON_1)
  {
    bPoint = mKnots[segmentIndex];
  }
  else if( (1.0 - s) < Math::MACHINE_EPSILON_1)
  {
    bPoint = mKnots[segmentIndex+1];
  }
  else
  {
    Vector4  sVect(s*s*s, s*s, s, 1);
    Vector4  cVect;

    cVect.x  = mKnots[segmentIndex].x;
    cVect.y  = mOutTangents[segmentIndex].x;
    cVect.z  = mInTangents[segmentIndex+1].x;
    cVect.w  = mKnots[segmentIndex+1].x;
    bPoint.x = sVect.Dot4(mBasis * cVect);

    cVect.x  = mKnots[segmentIndex].y;
    cVect.y  = mOutTangents[segmentIndex].y;
    cVect.z  = mInTangents[segmentIndex+1].y;
    cVect.w  = mKnots[segmentIndex+1].y;
    bPoint.y = sVect.Dot4(mBasis * cVect);

    cVect.x  = mKnots[segmentIndex].z;
    cVect.y  = mOutTangents[segmentIndex].z;
    cVect.z  = mInTangents[segmentIndex+1].z;
    cVect.w  = mKnots[segmentIndex+1].z;
    bPoint.z = sVect.Dot4(mBasis * cVect);
  }
  return bPoint;
}


// Given a parameter s (NOT x), return the Y value. Checks that the
// segment index is valid. For bezier splines, the last segment is
// only used to specify the end point, so is not valid.
float  Spline::GetY(unsigned int segmentIndex, float s) const
{
  DALI_ASSERT_ALWAYS( segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size() && "segmentIndex out of bounds");
  DALI_ASSERT_ALWAYS( mOutTangents.size() == mKnots.size() && "Spline not fully initialized" );
  DALI_ASSERT_ALWAYS( mInTangents.size()  == mKnots.size() && "Spline not fully initialized" );

  float    yValue=0.0f;

  if(s < 0.0f || s > 1.0f)
  {
    yValue = 0.0f;
  }
  else if(s < Math::MACHINE_EPSILON_1)
  {
    yValue = mKnots[segmentIndex].y;
  }
  else if( (1.0 - s) < Math::MACHINE_EPSILON_1)
  {
    yValue = mKnots[segmentIndex+1].y;
  }
  else
  {
    Vector4  sVect(s*s*s, s*s, s, 1);
    Vector4  cVect;

    cVect.x  = mKnots[segmentIndex].y;
    cVect.y  = mOutTangents[segmentIndex].y;
    cVect.z  = mInTangents[segmentIndex+1].y;
    cVect.w  = mKnots[segmentIndex+1].y;
    yValue   = sVect.Dot4(mBasis * cVect);
  }
  return yValue;
}

float ClampToZeroOne(float v)
{
  return (v<0.0f)?(0.0f):((v>1.0f)?1.0f:v);
}

// Use de Casteljau subdivision to approximate the parameter required to find x
// on a Bezier spline
// note, atX is already determined to be >= P0_X, <P1_X
float Spline::ApproximateCubicBezierParameter (
  float atX, float P0_X, float C0_X, float C1_X, float P1_X ) const
{
  if (fabs(atX - P0_X) < GetRangedEpsilon(atX, P0_X) )
  {
    return 0.0;
  }

  if (fabs(P1_X - atX) < GetRangedEpsilon(atX, P1_X) )
  {
    return 1.0;
  }

  int iterationStep = 0;

  float u = 0.0f; float v = 1.0f;

  // iteratively apply subdivision to approach value atX
  while (iterationStep < MAXIMUM_ITERATIONS)
  {
    float a = (P0_X + C0_X)*0.5f;
    float b = (C0_X + C1_X)*0.5f;
    float c = (C1_X + P1_X)*0.5f;
    float d = (a + b)*0.5f;
    float e = (b + c)*0.5f;
    float f = (d + e)*0.5f; // must be on curve - a Bezier spline is 2nd order diff continuous

    // The curve point is close enough to required value.
    if (fabsf(f - atX) < GetRangedEpsilon(f, atX) )
    {
      break;
    }

    if (f < atX)
    {
      P0_X = f;
      C0_X = e;
      C1_X = c;
      u = (u + v)*0.5f;
    }
    else
    {
      C0_X = a;
      C1_X = d;
      P1_X = f;
      v = (u + v)*0.5f;
    }

    iterationStep++;
  }

  return ClampToZeroOne((u + v)*0.5f);
}


bool Spline::FindSegment(float x, /*out*/ int& segmentIndex) const
{
  bool found = false;
  unsigned int index=0;
  unsigned int prevIndex=0;

  while(index < mKnots.size() && mKnots[index].x <= x)
  {
    prevIndex = index;
    ++index;
  }

  if(index == mKnots.size())
  {
    --prevIndex;
    --index;
  }

  if( (mKnots[prevIndex].x <= x) && ( x < mKnots[index].x ) )
  {
    found = true;
    segmentIndex = prevIndex;
  }
  return found;
}

float Spline::GetYFromMonotonicX(float x) const
{
  int segmentIndex=0;
  float yValue = 0.0f;

  if(FindSegment(x, segmentIndex))
  {
    float s = ApproximateCubicBezierParameter (x, mKnots[segmentIndex].x,
                                               mOutTangents[segmentIndex].x,
                                               mInTangents[segmentIndex+1].x,
                                               mKnots[segmentIndex+1].x);
    yValue  = GetY(segmentIndex, s);
  }
  else
  {
    if(mKnots.size() > 0)
    {
      Vector3 lastPoint = mKnots[mKnots.size()-1];
      if(fabsf(lastPoint.x - x) < GetRangedEpsilon(lastPoint.x, x))
      {
        yValue = lastPoint.y;
      }
    }
  }
  return yValue;
}

} // Dali
