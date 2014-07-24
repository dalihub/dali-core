#ifndef __DALI_CONSTRAINT_FUNCTIONS_H__
#define __DALI_CONSTRAINT_FUNCTIONS_H__

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

// EXTERNAL INCLUDES
#include <boost/function.hpp>

namespace Dali
{

struct Vector3;
struct Vector4;
class Quaternion;

/**
 * @brief Constraints can be used to adjust an Actor property, after animations have been applied.
 *
 * A local-constraint function is based on local properties of the Actor.
 */
namespace LocalConstraint DALI_IMPORT_API
{

/**
 * @brief A function which constrains a Vector3 property, based on the local properties of an Actor.
 *
 * @param[in] size     The actor's current size.
 * @param[in] position The actor's current position.
 * @param[in] scale    The actor's current rotation.
 * @param[in] rotation The actor's current scale.
 * @param[in] color    The actor's current color.
 * @return The constrained property value.
 */
typedef boost::function<Vector3 (const Vector3&    size,
                                 const Vector3&    position,
                                 const Quaternion& rotation,
                                 const Vector3&    scale,
                                 const Vector4&    color)> Vector3Function;

/**
 * @brief A function which constrains a Vector4 property, based on the local properties of an Actor.
 *
 * @param[in] size     The actor's current size.
 * @param[in] position The actor's current position.
 * @param[in] scale    The actor's current rotation.
 * @param[in] rotation The actor's current scale.
 * @param[in] color    The actor's current color.
 * @return The constrained property value.
 */
typedef boost::function<Vector4 (const Vector3&    size,
                                 const Vector3&    position,
                                 const Quaternion& rotation,
                                 const Vector3&    scale,
                                 const Vector4&    color)> Vector4Function;

/**
 * @brief A function which constrains a Quaternion property, based on the local properties of an Actor.
 *
 * @param[in] size     The actor's current size.
 * @param[in] position The actor's current position.
 * @param[in] scale    The actor's current rotation.
 * @param[in] rotation The actor's current scale.
 * @param[in] color    The actor's current color.
 * @return The constrained property value.
 */
typedef boost::function<Quaternion (const Vector3&    size,
                                    const Vector3&    position,
                                    const Quaternion& rotation,
                                    const Vector3&    scale,
                                    const Vector4&    color)> QuaternionFunction;

/**
 * @brief A function which constrains a boolean property, based on the local properties of an Actor.
 *
 * @param[in] size     The actor's current size.
 * @param[in] position The actor's current position.
 * @param[in] scale    The actor's current rotation.
 * @param[in] rotation The actor's current scale.
 * @param[in] color    The actor's current color.
 * @return The constrained property value.
 */
typedef boost::function<bool (const Vector3&    size,
                              const Vector3&    position,
                              const Quaternion& rotation,
                              const Vector3&    scale,
                              const Vector4&    color)> BoolFunction;

} // LocalConstraint

/**
 * @brief A parent-constraint function is based on properties related to the Actor's parent.
 */
namespace ParentConstraint DALI_IMPORT_API
{

/**
 * @brief A function which constrains a Vector3 property, based on properties related to the Actor's parent.
 *
 * @param[in] current The current value of the property to be constrained.
 * @param[in] parentOrigin The actor's current parent-origin.
 * @param[in] anchorPoint The actor's current anchor-point.
 * @param[in] parentSize The parent's current size.
 * @param[in] parentPosition The parent's current position.
 * @param[in] parentRotation The parent's current rotation.
 * @param[in] parentScale The parent's current scale.
 * @return The constrained property value.
 */
typedef boost::function<Vector3 (const Vector3&    current,
                                 const Vector3&    parentOrigin,
                                 const Vector3&    anchorPoint,
                                 const Vector3&    parentSize,
                                 const Vector3&    parentPosition,
                                 const Quaternion& parentRotation,
                                 const Vector3&    parentScale)> Vector3Function;

/**
 * @brief A function which constrains a Vector4 property, based on properties related to the Actor's parent.
 *
 * @param[in] current The current value of the property to be constrained.
 * @param[in] parentOrigin The actor's current parent-origin.
 * @param[in] anchorPoint The actor's current anchor-point.
 * @param[in] parentSize The parent's current size.
 * @param[in] parentPosition The parent's current position.
 * @param[in] parentRotation The parent's current rotation.
 * @param[in] parentScale The parent's current scale.
 * @return The constrained property value.
 */
typedef boost::function<Vector4 (const Vector4&    current,
                                 const Vector3&    parentOrigin,
                                 const Vector3&    anchorPoint,
                                 const Vector3&    parentSize,
                                 const Vector3&    parentPosition,
                                 const Quaternion& parentRotation,
                                 const Vector3&    parentScale)> Vector4Function;

/**
 * @brief A function which constrains a Quaternion property, based on properties related to the Actor's parent.
 *
 * @param[in] parentOrigin The actor's current parent-origin.
 * @param[in] anchorPoint The actor's current anchor-point.
 * @param[in] parentSize The parent's current size.
 * @param[in] parentPosition The parent's current position.
 * @param[in] parentRotation The parent's current rotation.
 * @param[in] parentScale The parent's current scale.
 * @return The constrained property value.
 */
typedef boost::function<Quaternion (const Quaternion& current,
                                    const Vector3&    parentOrigin,
                                    const Vector3&    anchorPoint,
                                    const Vector3&    parentSize,
                                    const Vector3&    parentPosition,
                                    const Quaternion& parentRotation,
                                    const Vector3&    parentScale)> QuaternionFunction;

/**
 * @brief A function which constrains a boolean property, based on properties related to the Actor's parent.
 *
 * @param[in] progress A progress value in the range 0->1, where 1 means that the constraint is fully applied.
 * @param[in] parentOrigin The actor's current parent-origin.
 * @param[in] anchorPoint The actor's current anchor-point.
 * @param[in] parentSize The parent's current size.
 * @param[in] parentPosition The parent's current position.
 * @param[in] parentRotation The parent's current rotation.
 * @param[in] parentScale The parent's current scale.
 * @return The constrained property value.
 */
typedef boost::function<bool (const bool&       current,
                              const Vector3&    parentOrigin,
                              const Vector3&    anchorPoint,
                              const Vector3&    parentSize,
                              const Vector3&    parentPosition,
                              const Quaternion& parentRotation,
                              const Vector3&    parentScale)> BoolFunction;

} // ParentConstraint

} // namespace Dali

#endif // __DALI_CONSTRAINT_FUNCTIONS_H__
