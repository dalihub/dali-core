#ifndef __DYNAMICS_DECLARATIONS_H__
#define __DYNAMICS_DECLARATIONS_H__

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

// INTERNAL HEADERS
#include <dali/public-api/object/ref-object.h>

namespace Dali
{

namespace Integration
{

struct DynamicsBodySettings;
struct DynamicsWorldSettings;

} // namespace Integration
namespace Internal
{

class DynamicsNotifier;

class DynamicsBody;
typedef IntrusivePtr<DynamicsBody> DynamicsBodyPtr;

class DynamicsBodyConfig;
typedef IntrusivePtr<DynamicsBodyConfig> DynamicsBodyConfigPtr;

class DynamicsCollision;
typedef IntrusivePtr<DynamicsCollision> DynamicsCollisionPtr;

class DynamicsJoint;
typedef IntrusivePtr<DynamicsJoint> DynamicsJointPtr;

class DynamicsSliderJoint;
typedef IntrusivePtr<DynamicsSliderJoint> DynamicsSliderJointPtr;

class DynamicsUniversalJoint;
typedef IntrusivePtr<DynamicsUniversalJoint> DynamicsUniversalJointPtr;

class DynamicsShape;
typedef IntrusivePtr<DynamicsShape> DynamicsShapePtr;

class DynamicsCapsuleShape;
typedef IntrusivePtr<DynamicsCapsuleShape> DynamicsCapsuleShapePtr;

class DynamicsConeShape;
typedef IntrusivePtr<DynamicsConeShape> DynamicsConeShapePtr;

class DynamicsCubeShape;
typedef IntrusivePtr<DynamicsCubeShape> DynamicsCubeShapePtr;

class DynamicsCylinderShape;
typedef IntrusivePtr<DynamicsCylinderShape> DynamicsCylinderShapePtr;

class DynamicsSphereShape;
typedef IntrusivePtr<DynamicsSphereShape> DynamicsSphereShapePtr;

class DynamicsWorld;
typedef IntrusivePtr<DynamicsWorld> DynamicsWorldPtr;

class DynamicsWorldConfig;
typedef IntrusivePtr<DynamicsWorldConfig> DynamicsWorldConfigPtr;

struct DynamicsWorldSettings;

} // namespace Internal

} // namespace Dali

#endif // __DYNAMICS_DECLARATIONS_H__
