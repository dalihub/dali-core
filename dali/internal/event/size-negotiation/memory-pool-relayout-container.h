#ifndef DALI_INTERNAL_MEMORY_POOL_RELAYOUT_CONTAINER_H
#define DALI_INTERNAL_MEMORY_POOL_RELAYOUT_CONTAINER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <memory> // for std::unique_ptr

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/size-negotiation/relayout-container.h>

#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/common/ordered-set.h>

namespace Dali
{
namespace Internal
{
/**
 * @brief Container to encapsulate information required for relayout.
 *
 * Uses a memory pool to manage data allocations.
 */
class MemoryPoolRelayoutContainer : public RelayoutContainer
{
public:
  /**
   * Struct to store the relayout information
   */
  struct RelayoutInfo
  {
    Dali::Actor actor; ///< The actor to relayout
    Vector2     size;  ///< The desired size of the actor
    struct RelayoutInfoHash
    {
      std::size_t operator()(const RelayoutInfo* x) const noexcept
      {
        return reinterpret_cast<std::size_t>(x->actor.GetObjectPtr());
      }
    };
    struct RelayoutInfoCompare
    {
      bool operator()(const RelayoutInfo* lhs, const RelayoutInfo* rhs) const noexcept
      {
        return lhs->actor == rhs->actor;
      }
    };
  };

  /**
   * @brief Default constructor
   *
   * @param objectAllocator A memory pool that can allocate memory for RelayoutInfos
   */
  MemoryPoolRelayoutContainer(MemoryPoolObjectAllocator<RelayoutInfo>& objectAllocator);

  /**
   * Virtual destructor
   */
  ~MemoryPoolRelayoutContainer() override;

  /**
   * @brief Add relayout information to the container if it does'nt already exist
   *
   * @param actor The actor to relayout
   * @param size The size to relayout
   */
  void Add(const Dali::Actor& actor, const Vector2& size) override;

  /**
   * @brief Remove information from the container
   */
  void PopBack();

  /**
   * @brief Retrieve relayout information for the latest added
   *
   * @param[out] actorOut Latest added actor
   * @param[out] sizeOt Latest added size
   */
  void GetBack(Dali::Actor& actorOut, Vector2& sizeOut) const;

  /**
   * @brief The count of information in the container
   */
  size_t Size() const;

  /**
   * @brief Set the capacity of the container
   *
   * @param capacity The new capacity for the container
   */
  void Reserve(size_t capacity);

  /**
   * @brief Reset the container, freeing all memory
   */
  void Clear();

private:
  /**
   * @brief Returns if the container contains the actor or not
   *
   * @param actor The actor to search for
   * @return Return if the actor was found or not
   */
  bool Contains(const Dali::Actor& actor);

private:
  using RelayoutInfoContainer = Dali::Internal::OrderedSet<RelayoutInfo, false, RelayoutInfo::RelayoutInfoHash, RelayoutInfo::RelayoutInfoCompare>;

  RelayoutInfoContainer mRelayoutInfos; ///< The list of relayout infos

  MemoryPoolObjectAllocator<RelayoutInfo>& mAllocator; ///< The memory pool from which the infos are allocated

  std::unique_ptr<RelayoutInfo> mDummyRelayoutInfo; ///< Dummy pointer that will be used to compare relayout infors what we already holded.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_MEMORY_POOL_RELAYOUT_CONTAINER_H
