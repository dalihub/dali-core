#ifndef __DALI_CORE_H__
#define __DALI_CORE_H__

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/actors/custom-actor.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/actors/sampling.h>

#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/constraint-source.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/animation/linear-constrainer.h>
#include <dali/public-api/animation/path.h>
#include <dali/public-api/animation/time-period.h>

#include <dali/public-api/common/compile-time-assert.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/loading-state.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/view-mode.h>

#include <dali/public-api/events/gesture-detector.h>
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/events/hover-event.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/long-press-gesture-detector.h>
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/events/pinch-gesture-detector.h>
#include <dali/public-api/events/pinch-gesture.h>
#include <dali/public-api/events/point-state.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/events/touch-point.h>

#include <dali/public-api/images/buffer-image.h>
#include <dali/public-api/images/encoded-buffer-image.h>
#include <dali/public-api/images/frame-buffer-image.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/native-image.h>
#include <dali/public-api/images/native-image-interface.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/images/pixel-data.h>

#include <dali/public-api/math/angle-axis.h>
#include <dali/public-api/math/compile-time-math.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/random.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/uint-16-pair.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

#include <dali/public-api/object/any.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/object-registry.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-conditions.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-notification-declarations.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/object/property-types.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/type-info.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/weak-handle.h>

#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/render-tasks/render-task.h>

#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/property-buffer.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/rendering/texture-set.h>

#include <dali/public-api/signals/base-signal.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/connection-tracker-interface.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/public-api/signals/dali-signal.h>
#include <dali/public-api/signals/functor-delegate.h>
#include <dali/public-api/signals/signal-slot-connections.h>
#include <dali/public-api/signals/signal-slot-observers.h>
#include <dali/public-api/signals/slot-delegate.h>

#include <dali/public-api/size-negotiation/relayout-container.h>

#include <dali/public-api/dali-core-version.h>

#endif // __DALI_CORE_H__
