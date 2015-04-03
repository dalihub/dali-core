#ifndef __DALI_INTERNAL_MEMORY_POOL_RELAYOUT_CONTAINER_H__
#define __DALI_INTERNAL_MEMORY_POOL_RELAYOUT_CONTAINER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/actors/actor.h>

#include <dali/internal/common/memory-pool-object-allocator.h>

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
    Dali::Actor actor;            ///< The actor to relayout
    Vector2 size;           ///< The desired size of the actor
  };

  /**
   * @brief Default constructor
   *
   * @param objectAllocator A memory pool that can allocate memory for RelayoutInfos
   */
  MemoryPoolRelayoutContainer( MemoryPoolObjectAllocator< RelayoutInfo >& objectAllocator );

  /**
   * Virtual destructor
   */
  virtual ~MemoryPoolRelayoutContainer();

  /**
   * @brief Add relayout information to the container if it does'nt already exist
   *
   * @param actor The actor to relayout
   * @param size The size to relayout
   */
  virtual void Add( const Dali::Actor& actor, const Vector2& size );

  /**
   * @brief Remove information from the container
   */
  void PopBack();

  /**
   * @brief Retrieve relayout information for the given index
   *
   * @param index The index of the information to retrieve
   */
  void Get( size_t index, Dali::Actor& actorOut, Vector2& sizeOut  ) const;

  /**
   * @brief The count of information in the container
   */
  size_t Size() const;

  /**
   * @brief Set the capacity of the container
   *
   * @param capacity The new capacity for the container
   */
  void Reserve( size_t capacity );

  /**
   * @brief Reset the container, freeing all memory
   */
  void Clear();

  /**
   * @brief Returns if the container contains the actor or not
   *
   * @param actor The actor to search for
   * @return Return if the actor was found or not
   */
  bool Contains( const Dali::Actor& actor );

private:

  typedef Vector< RelayoutInfo* > RelayoutInfoContainer;

  RelayoutInfoContainer mRelayoutInfos;     ///< The list of relayout infos

  MemoryPoolObjectAllocator< RelayoutInfo >& mAllocator;         ///< The memory pool from which the infos are allocated
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_MEMORY_POOL_RELAYOUT_CONTAINER_H__
