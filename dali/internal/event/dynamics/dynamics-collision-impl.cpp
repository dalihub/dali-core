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

// EXTERNAL HEADERS

// CLASS HEADER
#include <dali/internal/event/dynamics/dynamics-collision-impl.h>

// INTERNAL HEADERS
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/dynamics/dynamics-declarations.h>

namespace Dali
{

namespace Internal
{

DynamicsCollision::DynamicsCollision()
: mImpact(0.0f)
{
}

DynamicsCollision::~DynamicsCollision()
{
}

ActorPtr DynamicsCollision::GetActorA()
{
  return mActorA;
}

void DynamicsCollision::SetActorA(ActorPtr actor)
{
  mActorA = actor;
}

ActorPtr DynamicsCollision::GetActorB()
{
  return mActorB;
}

void DynamicsCollision::SetActorB(ActorPtr actor)
{
  mActorB = actor;
}

float DynamicsCollision::GetImpactForce() const
{
  return mImpact;
}

void DynamicsCollision::SetImpactForce(const float impact)
{
  mImpact = impact;
}

Vector3 DynamicsCollision::GetPointOnA() const
{
  return mPointA;
}

void DynamicsCollision::SetPointOnA(const Vector3 point)
{
  mPointA = point;
}

Vector3 DynamicsCollision::GetPointOnB() const
{
  return mPointB;
}

void DynamicsCollision::SetPointOnB(const Vector3 point)
{
  mPointB = point;
}

Vector3 DynamicsCollision::GetNormal() const
{
  return mNormal;
}

void DynamicsCollision::SetNormal(const Vector3 normal)
{
  mNormal = normal;
}

} // namespace Internal

} // namespace Dali
