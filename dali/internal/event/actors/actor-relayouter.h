#ifndef DALI_INTERNAL_ACTOR_RELAYOUTER_H
#define DALI_INTERNAL_ACTOR_RELAYOUTER_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{

namespace Internal
{

/**
 * Struct to do some actor specific relayouting and store related variables
 */
struct Actor::Relayouter
{
  // Defaults
  static constexpr Vector3 DEFAULT_SIZE_MODE_FACTOR{1.0f, 1.0f, 1.0f};
  static constexpr Vector2 DEFAULT_PREFERRED_SIZE{0.0f, 0.0f};
  static constexpr Vector2 DEFAULT_DIMENSION_PADDING{0.0f, 0.0f};
  static constexpr SizeScalePolicy::Type DEFAULT_SIZE_SCALE_POLICY = SizeScalePolicy::USE_SIZE_SET;

  /// Constructor
  Relayouter();

  /// Default Destructor
  ~Relayouter() = default;

  /// @copydoc Actor::GetResizePolicy
  ResizePolicy::Type GetResizePolicy( Dimension::Type dimension ) const;

  /// @copydoc Actor::SetPadding
  void SetPadding( const Vector2& padding, Dimension::Type dimension );

  /// @copydoc Actor::SetLayoutNegotiated
  void SetLayoutNegotiated( bool negotiated, Dimension::Type dimension );

  /// @copydoc Actor::IsLayoutNegotiated
  bool IsLayoutNegotiated( Dimension::Type dimension ) const;

  /// @copydoc Actor::ApplySizeSetPolicy
  Vector2 ApplySizeSetPolicy( Internal::Actor& actor, const Vector2& size );

  /// @copydoc Actor::SetUseAssignedSize
  void SetUseAssignedSize( bool use, Dimension::Type dimension );

  /// @copydoc Actor::GetUseAssignedSize
  bool GetUseAssignedSize( Dimension::Type dimension ) const;

  /// @copydoc Actor::SetMinimumSize
  void SetMinimumSize( float size, Dimension::Type dimension );

  /// @copydoc Actor::GetMinimumSize
  float GetMinimumSize( Dimension::Type dimension ) const;

  /// @copydoc Actor::SetMaximumSize
  void SetMaximumSize( float size, Dimension::Type dimension );

  /// @copydoc Actor::GetMaximumSize
  float GetMaximumSize( Dimension::Type dimension ) const;

  /// @copydoc Actor::SetResizePolicy
  void SetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension, Vector3& targetSize );

  /// @copydoc Actor::SetDimensionDependency
  void SetDimensionDependency( Dimension::Type dimension, Dimension::Type dependency );

  /// @copydoc Actor::GetDimensionDependency
  Dimension::Type GetDimensionDependency( Dimension::Type dimension ) const;

  /// @copydoc Actor::SetLayoutDirty
  void SetLayoutDirty( bool dirty, Dimension::Type dimension );

  /// @copydoc Actor::IsLayoutDirty
  bool IsLayoutDirty( Dimension::Type dimension ) const;

  /**
   * @brief Clamp a dimension given the relayout constraints on given actor
   *
   * @param[in] actor The actor to clamp
   * @param[in] size The size to constrain
   * @param[in] dimension The dimension the size exists in
   * @return Return the clamped size
   */
  static float ClampDimension( const Internal::Actor& actor, float size, Dimension::Type dimension );

public:

  ResizePolicy::Type resizePolicies[ Dimension::DIMENSION_COUNT ];      ///< Resize policies
  bool useAssignedSize[ Dimension::DIMENSION_COUNT ];                   ///< The flag to specify whether the size should be assigned to the actor

  Dimension::Type dimensionDependencies[ Dimension::DIMENSION_COUNT ];  ///< A list of dimension dependencies

  Vector2 dimensionPadding[ Dimension::DIMENSION_COUNT ];         ///< Padding for each dimension. X = start (e.g. left, bottom), y = end (e.g. right, top)

  float negotiatedDimensions[ Dimension::DIMENSION_COUNT ];       ///< Storage for when a dimension is negotiated but before set on actor

  float minimumSize[ Dimension::DIMENSION_COUNT ];                ///< The minimum size an actor can be
  float maximumSize[ Dimension::DIMENSION_COUNT ];                ///< The maximum size an actor can be

  bool dimensionNegotiated[ Dimension::DIMENSION_COUNT ];         ///< Has the dimension been negotiated
  bool dimensionDirty[ Dimension::DIMENSION_COUNT ];              ///< Flags indicating whether the layout dimension is dirty or not

  Vector3 sizeModeFactor;                              ///< Factor of size used for certain SizeModes

  Vector2 preferredSize;                               ///< The preferred size of the actor

  SizeScalePolicy::Type sizeSetPolicy :3;            ///< Policy to apply when setting size. Enough room for the enum

  bool relayoutEnabled :1;                   ///< Flag to specify if this actor should be included in size negotiation or not (defaults to true)
  bool insideRelayout :1;                    ///< Locking flag to prevent recursive relayouts on size set
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_ACTOR_RELAYOUTER_H
