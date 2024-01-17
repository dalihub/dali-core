#ifndef DALI_INTERNAL_UPDATE_RESETTER_CONTAINER_H
#define DALI_INTERNAL_UPDATE_RESETTER_CONTAINER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
 */

namespace Dali::Internal::SceneGraph
{
/**
 * Template class to manage node/property resetters
 */
template<class ResetterType>
class ResetterContainer
{
public:
  // std::list offers fast delete and fast iteration.
  using ContainerType = std::list<ResetterType*>;
  using Iterator      = typename ContainerType::iterator;
  using ConstIterator = typename ContainerType::const_iterator;

  ResetterContainer() = default;

  ResetterContainer(const ResetterContainer&) = delete;
  ResetterContainer(ResetterContainer&&)      = delete;

  /**
   * Ensure all resetters are destroyed when the container is destroyed
   */
  ~ResetterContainer()
  {
    Clear();
  }

  /**
   * Add a resetter to the container
   * @param[in] resetterPtr A pointer to the resetter.
   * The container takes ownership.
   */
  void PushBack(ResetterType* resetterPtr)
  {
    mContainer.push_back(resetterPtr);
  }

  /**
   * @return true if the container is empty
   */
  bool Empty()
  {
    return mContainer.empty();
  }

  /**
   * Clear the container, destroying all extant resetters.
   */
  void Clear()
  {
    auto iter = Begin();
    while(iter != End())
    {
      iter = EraseObject(iter);
    }
  }

  /**
   * @return The number of resetter contained.
   */
  size_t Count() const
  {
    return mContainer.size();
  }

  /**
   * @return an iterator to the start of the container
   */
  Iterator Begin()
  {
    return mContainer.begin();
  }
  /**
   * Support for C++11 Range-based for loop
   * @return an iterator to the start of the container.
   */
  Iterator begin()
  {
    return mContainer.begin();
  }
  /**
   * @return an iterator to the start of the container
   */
  ConstIterator Begin() const
  {
    return mContainer.begin();
  }
  /**
   * Support for C++11 Range-based for loop
   * @return an iterator to the start of the container.
   */
  ConstIterator begin() const
  {
    return mContainer.begin();
  }

  /**
   * @return an iterator to the end of the container.
   */
  Iterator End()
  {
    return mContainer.end();
  }
  /**
   * Support for C++11 Range-based for loop
   * @return an iterator to the end of the container.
   */
  Iterator end()
  {
    return mContainer.end();
  }
  /**
   * @return an iterator to the end of the container.
   */
  ConstIterator End() const
  {
    return mContainer.end();
  }
  /**
   * Support for C++11 Range-based for loop
   * @return an iterator to the end of the container.
   */
  ConstIterator end() const
  {
    return mContainer.end();
  }

  /**
   * Erase a resetter from the container
   */
  Iterator EraseObject(Iterator iter)
  {
    delete *iter;
    return mContainer.erase(iter);
  }

  /**
   * Iterate over the container, resetting all the referenced
   * properties. If a resetter has finished (e.g. it's animation /
   * constraint has ended, or it's baked 2 values), then it is removed
   * from the list.
   * @param[in] bufferIndex The buffer index of the property to be reset
   */
  void ResetToBaseValues(BufferIndex bufferIndex)
  {
    if(!mContainer.empty())
    {
      auto end  = mContainer.end();
      auto iter = mContainer.begin();
      while(iter != end)
      {
        (*iter)->ResetToBaseValue(bufferIndex);
        if((*iter)->IsFinished())
        {
          iter = EraseObject(iter);
        }
        else // Skip to next element
        {
          ++iter;
        }
      }
    }
  }

private:
  ContainerType mContainer; ///< The list of resetters
};

} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_UPDATE_RESETTER_CONTAINER_H
