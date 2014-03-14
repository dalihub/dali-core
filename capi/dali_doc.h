#ifndef __DALI_DOC_H__
#define __DALI_DOC_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @ingroup CAPI_UI_FRAMEWORK
 * @defgroup CAPI_DALI_FRAMEWORK Dali
 * @brief The Dali provides a cross-platform C++ 3D UI toolkit based on
 * OpenGL ES 2.0.
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref CAPI_DALI_MODULE</td><td>A library to provide a C++ scene graph based on OpenGL ES 2.0.</td></tr>
 * <tr><td>@ref CAPI_DALI_TOOLKIT_MODULE</td><td>A library to provide a user interface toolkit on top of Dali.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_FRAMEWORK
 * @defgroup CAPI_DALI_MODULE Dali Core
 * @brief The Dali library provides a C++ scene graph based on OpenGL ES 2.0.
 *
 * @section CAPI_DALI_MODULE_HEADER Required Header
 *  \#include <dali/dali.h>
 * @section CAPI_DALI_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref CAPI_DALI_ACTORS_MODULE</td><td>The set of interactive objects that can be placed on the Dali::Stage.</td></tr>
 * <tr><td>@ref CAPI_DALI_ADAPTOR_MODULE</td><td>Interface to the platform.</td></tr>
 * <tr><td>@ref CAPI_DALI_ANIMATION_MODULE</td><td>The property animation and constraints system.</td></tr>
 * <tr><td>@ref CAPI_DALI_COMMON_MODULE</td><td>General classes.</td></tr>
 * <tr><td>@ref CAPI_DALI_EVENTS_MODULE</td><td>Events, gestures and gesture detectors.</td></tr>
 * <tr><td>@ref CAPI_DALI_GEOMETRY_MODULE</td><td></td>Meshes.</tr>
 * <tr><td>@ref CAPI_DALI_IMAGES_MODULE</td><td></td>Image resource types.</tr>
 * <tr><td>@ref CAPI_DALI_MATH_MODULE</td><td></td>Math classes and utilities.</tr>
 * <tr><td>@ref CAPI_DALI_MODELING_MODULE</td><td>Ancilliary classes for meshes and 3D models </td></tr>
 * <tr><td>@ref CAPI_DALI_OBJECT_MODULE</td><td></td>Registered Object and property system.</tr>
 * <tr><td>@ref CAPI_DALI_RENDER_TASKS_MODULE</td><td>Classes to describe how the scene is rendered. </td></tr>
 * <tr><td>@ref CAPI_DALI_SHADER_EFFECTS_MODULE</td><td>Classes to change how actors are visualised.</td></tr>
 * <tr><td>@ref CAPI_DALI_SIGNALS_MODULE</td><td>Signals and Slots - a type-safe, loosely-coupled callback system.</td></tr>
 * <tr><td>@ref CAPI_DALI_TEXT_MODULE</td><td></td>Classes for manipulating text.</tr>
 * </table>
 */


/**
 * @ingroup CAPI_DALI_MODULE
 *
 * @defgroup CAPI_DALI_ACTORS_MODULE Actors
 *
 * @section CAPI_DALI_ACTORS_MODULE_HEADER RequiredHeader
 * \#include <dali/dali.h>
 *
 * @section CAPI_DALI_ACTORS_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Actor </td><td>Primary class for application interaction.</td></tr>
 * <tr><td>@ref Dali::CameraActor </td><td>An actor which controls a camera.</td></tr>
 * <tr><td>@ref Dali::CustomActor </td><td>CustomActor is a base class for custom UI controls.</td></tr>
 * <tr><td>@ref Dali::CustomActorImpl </td><td>CustomActorImpl is an abstract base class for custom control implementations.</td></tr>
 * <tr><td>@ref Dali::RenderableActor </td><td>A base class for renderable actors.</td></tr>
 * <tr><td>@ref Dali::ImageActor </td><td>An actor for displaying images.</td></tr>
 * <tr><td>@ref Dali::MeshActor </td><td>An actor for displaying mesh geometries.</td></tr>
 * <tr><td>@ref Dali::TextActor </td><td>An actor for display simple text.</td></tr>
 * <tr><td>@ref Dali::Layer </td><td></td>An actor for grouping and sorting other actors.</tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_ANIMATION_MODULE Animation
 * @section  CAPI_DALI_ANIMATION_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section  CAPI_DALI_ANIMATION_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::ActiveConstraint </td><td>A constraint which is being applied to an object.</td></tr>
 * <tr><td> Dali::AlphaFunctions </td><td>Various functions that control progress of an animation.</td></tr>
 * <tr><td>@ref Dali::Animation </td><td>Dali::Animation can be used to animate the properties of any number of objects, typically Dali::Actor%s.</td></tr>
 * <tr><td>@ref Dali::Constraint </td><td>An abstract base class for Constraints.</td></tr>
 * <tr><td>Constraints</td><td>Various constraints for properties:
@ref Dali::ScaleToFitConstraint
@ref Dali::ScaleToFitKeepAspectRatioConstraint
@ref Dali::ScaleToFillKeepAspectRatioConstraint
@ref Dali::ScaleToFillXYKeepAspectRatioConstraint
@ref Dali::ShrinkInsideKeepAspectRatioConstraint
@ref Dali::MultiplyConstraint
@ref Dali::DivideConstraint
@ref Dali::EqualToConstraint
@ref Dali::RelativeToConstraint
@ref Dali::RelativeToConstraintFloat
@ref Dali::InverseOfConstraint
@ref Dali::SourceWidthFixedHeight
@ref Dali::SourceHeightFixedWidth
Dali::LookAt()
@ref Dali::OrientedLookAt </td></tr>
 * <tr><td>Interpolator Functions </td><td>Dali::LerpBoolean(), Dali::LerpFloat(), Dali::LerpVector2(), Dali::LerpVector3(), Dali::LerpVector4(), Dali::SlerpQuaternion() </td></tr>
 * <tr><td>@ref Dali::KeyFrames </td><td>A set of key frames for a property that can be animated using @ref Dali::Animation::AnimateBetween()</td></tr>
 * <tr><td>Constraint Sources</td><td>Specifies the location of a property for use in a constraint:
 * @ref Dali::SourceType
@ref Dali::LocalSource
@ref Dali::ParentSource
@ref Dali::Source
@ref Dali::ConstraintSource</td></tr>
 * <tr><td>@ref Dali::TimePeriod </td><td>A value-type representing a period of time within an animation.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_COMMON_MODULE Common
 * @section CAPI_DALI_COMMON_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section CAPI_DALI_COMMON_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::ParentOrigin </td><td>Constants to specify the origin of an actor within it's parent volume, used by Dali::Actor::SetParentOrigin()</td></tr>
 * <tr><td>@ref Dali::AnchorPoint </td><td>Constants to specify the anchor point of an actor, used by Dali::Actor::SetAnchorPoint() </td></tr>
 * <tr><td>@ref Dali::Color </td><td>Constants to specify primary colors</td></tr>
 * <tr><td>@ref Dali::Math </td><td>Epsilon and Pi constants</td></tr>
 * <tr><td>@ref Dali::String </td><td>String constants</td></tr>
 * <tr><td>@ref Dali::LoadingState </td><td>The status during resource loading operations.</td></tr>
 * <tr><td>@ref Dali::Stage </td><td>The Stage is a top-level object used for displaying a tree of Actors.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_EVENTS_MODULE Events
 * @section CAPI_DALI_EVENTS_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section CAPI_DALI_EVENTS_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::GestureDetector </td><td>GestureDetectors analyse a stream of touch events and attempt to determine the intention of the user.</td></tr>
 * <tr><td>@ref Dali::Gesture </td><td>Base structure for different gestures that an application can receive.</td></tr>
 * <tr><td>@ref Dali::KeyEvent </td><td>The key event structure is used to store a key press.</td></tr>
 * <tr><td>@ref Dali::LongPressGestureDetector</td><td>This emits a signal when a long press gesture occurs.</td></tr>
 * <tr><td>@ref Dali::LongPressGesture </td><td>A LongPressGesture is emitted when the user touches and holds the screen with the stated number of fingers.</td></tr>
 * <tr><td>@ref Dali::PanGestureDetector </td><td>This class looks for panning (or dragging) gestures.</td></tr>
 * <tr><td>@ref Dali::PanGesture </td><td>A PanGesture is emitted when the user moves one or more fingers in a particular direction.</td></tr>
 * <tr><td>@ref Dali::PinchGestureDetector </td><td>This class looks for pinching gestures involving two touches.</td></tr>
 * <tr><td>@ref Dali::PinchGesture </td><td>A PinchGesture is emitted when the user moves two fingers towards or away from each other.</td></tr>
 * <tr><td>@ref Dali::TapGestureDetector </td><td>This class emits a signal when a tap gesture occurs</td></tr>
 * <tr><td>@ref Dali::TapGesture </td><td>A TapGesture is emitted when the user taps the screen</td></tr>
 * <tr><td>@ref Dali::TouchEvent </td><td>Touch events are a collection of touch points at a specific moment in time.</td></tr>
 * <tr><td>@ref Dali::TouchPoint </td><td>A TouchPoint represents a point on the screen that is currently being touched or where touch
 * has stopped.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_GEOMETRY_MODULE Geometry
 * @section CAPI_DALI_GEOMETRY_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section CAPI_DALI_GEOMETRY_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::AnimatableMesh </td><td>A mesh geometry that can be animated using the Dali property animation system.</td></tr>
 * <tr><td>@ref Dali::AnimatableVertex </td><td>Represents a vertex in a Dali::AnimatableMesh.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_IMAGES_MODULE Images
 * @section CAPI_DALI_IMAGES_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section CAPI_DALI_IMAGES_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::BitmapImage </td><td>This represents an image resource supplied by Application writer that can be added to Dali::ImageActor.</td></tr>
 * <tr><td>@ref Dali::EncodedBufferImage </td><td>This represents an encoded image supplied by Application writer that can be added to Dali::ImageActor.</td></tr>
 * <tr><td>@ref Dali::FrameBufferImage </td><td>This represents a GLES frame buffer object and contains the result of an 'off-screen' render pass of a Dali::RenderTask</td></tr>
 * <tr><td>@ref Dali::ImageAttributes </td><td>Describes Image properties like width or pixel format.</td></tr>
 * <tr><td>@ref Dali::Image </td><td>An Image object represents an image resource that can be added to ImageActors.</td></tr>
 * <tr><td>@ref Dali::NativeImage </td><td>Abstract interface to provide platform-specific support for handling image data.</td></tr>
 * <tr><td>@ref Dali::Pixel </td><td>Pixel format types and their properties.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_MATH_MODULE Math
 * @section CAPI_DALI_MATH_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section CAPI_DALI_MATH_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::AngleAxis </td><td>An angle & axis pair.</td></tr>
 * <tr><td>@ref Dali::Degree </td><td>An angle in degrees.</td></tr>
 * <tr><td>@ref Dali::Radian </td><td>An angle in radians.</td></tr>
 * <tr><td>@ref Dali::Matrix </td><td>The Matrix class represents transformations and projections.</td></tr>
 * <tr><td>@ref Dali::Matrix3 </td><td>A 3x3 matrix.</td></tr>
 * <tr><td>@ref Dali::Quaternion </td><td>The Quaternion class encapsulates the mathematics of the quaternion.</td></tr>
 * <tr><td>@ref Dali::Rect </td><td>Template class to create and operate on rectangles.</td></tr>
 * <tr><td>@ref Dali::Vector2 </td><td>A two-dimensional vector.</td></tr>
 * <tr><td>@ref Dali::Vector3 </td><td>A three-dimensional vector.</td></tr>
 * <tr><td>@ref Dali::Vector4 </td><td>A four-dimensional vector.</td></tr>
 * <tr><td>@ref Dali::Random </td><td>Provides methods to generate and use random values.</td></tr>
 * <tr><td>@ref Dali::Viewport </td><td>Screen area in pixels</td></tr>
 * <tr><td> Math Utilities </td><td>
Dali::NextPowerOfTwo()
Dali::IsPowerOfTwo()
Dali::Clamp()
Dali::ClampInPlace()
Dali::Lerp()
Dali::GetRangedEpsilon()
Dali::EqualsZero()
Dali::Equals()
Dali::Round()
Dali::WrapInDomain()
Dali::ShortestDistanceInDomain()
 * </td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_MODELING_MODULE Modeling
 * @section CAPI_DALI_MODELING_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section CAPI_DALI_MODELING_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Material </td><td>Encapsulates the data describing a material.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_OBJECT_MODULE Object
 * @section CAPI_DALI_OBJECT_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section CAPI_DALI_OBJECT_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::BaseHandle </td><td>Dali::BaseHandle is a handle to an internal Dali resource.</td></tr>
 * <tr><td>@ref Dali::BaseObject </td><td>A base class for objects.</td></tr>
 * <tr><td>@ref Dali::Constrainable </td><td>A handle to an internal property owning Dali object that
 * can have constraints applied to it.</td></tr>
 * <tr><td>@ref Dali::Handle </td><td>A handle to an internal property owning Dali object.</td></tr>
 * <tr><td>@ref Dali::ObjectRegistry </td><td>The ObjectRegistry notifies it's observers when an object is created.</td></tr>
 * <tr><td>@ref Dali::PropertyCondition </td><td>This represents a condition that can be evaluated on a Property::Value. Factory methods include Dali::LessThanCondition(), Dali::GreaterThanCondition(), Dali::InsideCondition() and Dali::OutsideCondition()</td></tr>
 * <tr><td>@ref Dali::PropertyInput </td><td>An abstract interface for receiving property values.</td></tr>
 * <tr><td>@ref Dali::PropertyNotification </td><td>This is used to issue a notification upon a condition of the property being met.</td></tr>
 * <tr><td>@ref Dali::PropertyTypes </td><td>Template function instances for property getters.</td></tr>
 * <tr><td>@ref Dali::Property </td><td>An object + property pair.</td></tr>
 * <tr><td>@ref Dali::Property::Value </td><td>A value-type representing a property value.</td></tr>a
 * <tr><td>@ref Dali::RefObject </td><td>Base class for reference counted objects.</td></tr>
 * <tr><td>@ref Dali::IntrusivePtr </td><td>Template class used to point at Dali::RefObjects</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_RENDER_TASKS_MODULE Render Tasks
 * @section CAPI_DALI_RENDER_TASKS_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section CAPI_DALI_RENDER_TASKS_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::RenderTask </td><td>A RenderTask describes how all or part of the Dali scene should be rendered.</td></tr>
 * <tr><td>@ref Dali::RenderTaskList </td><td>An ordered list of Dali::RenderTask%s.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_SHADER_EFFECTS_MODULE Shader effects
 * @section CAPI_DALI_SHADER_EFFECTS_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section CAPI_DALI_SHADER_EFFECTS_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::ShaderEffect </td><td>Shader effects provide a visual effect for actors.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_SIGNALS_MODULE Signals
 * @section CAPI_DALI_SIGNALS_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section CAPI_DALI_SIGNALS_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::BaseSignal </td><td>Implementation class for Dali::SignalV2%s.</td></tr>
 * <tr><td>@ref Dali::Callback </td><td>Templates to provide type safety for member function callbacks.</td></tr>
 * <tr><td>@ref Dali::ConnectionTracker </td><td>Connection tracker concrete implementation</td></tr>
 * <tr><td>@ref Dali::ConnectionTrackerInterface </td><td>Interface used to track connections between signals and slots.</td></tr>
 * <tr><td>@ref Dali::SignalV2 </td><td>Template classes that provide signals</td></tr>
 * <tr><td>@ref Dali::FunctorDelegate </td><td>Used to connect a void() functor to a signal via Dali::BaseObject::SignalConnect()</td></tr>
 * <tr><td>@ref Dali::SlotConnection </td><td>SlotConnection is the connection information held by a connection tracker.</td></tr>
 * <tr><td>@ref Dali::SignalConnection </td><td>SignalConnection is the connection information held by the signal.</td></tr>
 * <tr><td>@ref Dali::SignalObserver </td><td>Abstract interface used by a signal to inform a slot it is disconnecting.</td></tr>
 * <tr><td>@ref Dali::SlotObserver </td><td>Abstract Interface used by a slot to inform a signal it is disconnecting.</td></tr>
 * <tr><td>@ref Dali::SlotDelegate </td><td>SlotDelegates can be used to connect member functions to signals, without inheriting from SlotDelegateInterface.</td></tr>
 * </table>
 */

/**
 * @ingroup CAPI_DALI_MODULE
 * @defgroup CAPI_DALI_TEXT_MODULE Text
 * @section CAPI_DALI_TEXT_MODULE_HEADER Required Header
 * \#include <dali/dali.h>
 * @section CAPI_DALI_TEXT_MODULE_OVERVIEW Overview
 * <table> <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref Dali::Character </td><td>Character class encapsulates a character implementation to support multiple languages.</td></tr>
 * <tr><td>@ref Dali::PointSize </td><td>Font size in points.</td></tr>
 * <tr><td>@ref Dali::PixelSize </td><td>Font size in pixels.</td></tr>
 * <tr><td>@ref Dali::CapsHeight </td><td>Font size in caps height.</td></tr>
 * <tr><td>@ref Dali::FontParameters </td><td>Encapsulates all font parameters.</td></tr>
 * <tr><td>@ref Dali::Font </td><td>Encapsulates a font resource</td></tr>
 * <tr><td>@ref Dali::TextStyle </td><td>Encapsulates style properties for text, such as weight, italics, underline, shadow, etc.</td></tr>
 * <tr><td>@ref Dali::Text </td><td>Encapsulates a string of characters implementation to support multiple languages.</td></tr>
 * </table>
 */



#endif //__DALI_DOC_H__
