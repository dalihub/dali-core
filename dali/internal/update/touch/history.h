#ifndef __DALI_INTERNAL_HISTORY_H__
#define __DALI_INTERNAL_HISTORY_H__

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
#include <limits>

// INTERNAL INCLUDES
#include <dali/devel-api/common/set-wrapper.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

namespace Internal
{

/**
 * HistoryPair
 * represents a key-value element in the HistoryContainer
 */
template<class T>
struct HistoryPairType
{
public:

  HistoryPairType(float firstValue)
  : first(firstValue)
  {
  }

  HistoryPairType(float firstValue, T secondValue)
  : first(firstValue),
    second(secondValue)
  {
  }

  bool operator<(const HistoryPairType& rhs) const
  {
    return first < rhs.first;
  }

  float first;
  T second;
};

/**
 * History container.
 * This container is used for keeping a list of element pairs while providing an API that can
 * generate interpolated values of requested elements that lie between two stored elements.
 *
 * e.g. stored values:
 *
 * 1.0 - 10
 * 2.0 - 30
 * 3.0 - 50
 *
 * Requesting value at key 1.5 will use the adjacent stored keys (1.0 and 2.0) to return an
 * interpolated value of 20.0 (i.e. 0.5 of the way between 10 and 30).
 *
 * Requesting value at key 2.9 will use the adjacent stored keys (2.0 and 3.0) to return an
 * interpolated value of 48.0 (i.e. 0.9 of the way between 30 and 50)
 */
class History
{
  typedef HistoryPairType<Vector2> HistoryPair;
  typedef std::set<HistoryPair> HistoryContainer;
  typedef HistoryContainer::iterator HistoryContainerIter;

public:

  /**
   * History constructor
   */
  History()
  : mMaxSize(std::numeric_limits<size_t>::max())
  {
  }

  /**
   * History destructor
   */
  ~History()
  {
  }

  /**
   * Sets the maximum size of the history container in terms of elements stored, default is no limit
   * @param[in] maxSize The maximum number of elements stored in container
   */
  void SetMaxSize(size_t maxSize)
  {
    mMaxSize = maxSize;

    if(mHistory.size() > mMaxSize)
    {
      // determine reduction in history size, and remove these elements
      size_t reduction = mHistory.size() - mMaxSize;

      while(reduction--)
      {
        mHistory.erase(mHistory.begin() );
      }
    }
  }

  void Clear()
  {
    mHistory.clear();
  }

  /**
   * Adds an element (y) to the container at position (x)
   *
   * @param[in] x Key position value to add
   * @param[in] y Value to add at Key.
   */
  void Add(float x, const Vector2& y)
  {
    if(mHistory.size() >= mMaxSize)
    {
      RemoveTail();
    }

    mHistory.insert(HistoryPair(x,y));
  }

  /**
   * Removes first element in the container
   */
  void RemoveTail()
  {
    mHistory.erase(mHistory.begin());
  }

  /**
   * Retrieves value from the history using key (x).
   * If the requested key (x) lies between two points, a linearly interpolated value between the two
   * points is returned.
   *
   * @param[in] x Key position to retrieve
   *
   * @return The interpolated Value is returned for this position.
   */
  Vector2 Get(float x) const
  {
    HistoryContainerIter i = mHistory.lower_bound(x);

    if(i == mHistory.end())
    {
      --i;
    }

    // For samples based on first point, just return position.
    if(i == mHistory.begin())
    {
      return i->second;
    }

    // within begin() ... end() range
    float x2 = i->first;
    Vector2 y2 = i->second;

    --i;
    float x1 = i->first;
    Vector2 y1 = i->second;

    // For samples based on first 2 points, just use linear interpolation
    // TODO: Should really perform quadratic interpolation whenever there are 3+
    // points.
    if(i == mHistory.begin())
    {
      return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
    }

    // For samples based elsewhere, always use quadratic interpolation.
    --i;
    float x0 = i->first;
    Vector2 y0 = i->second;

    if(i != mHistory.begin())
    {
      --i;
      float xn = i->first;
      Vector2 yn = i->second;

      x2 = (x2 + x1) * 0.5f;
      x1 = (x1 + x0) * 0.5f;
      x0 = (xn + x0) * 0.5f;

      y2 = (y2 + y1) * 0.5f;
      y1 = (y1 + y0) * 0.5f;
      y0 = (yn + y0) * 0.5f;
    }
    // Quadratic equation:

    // y = ax^2 + bx + c
    // by making touches relative to x0, y0 (i.e. x0 = 0, y0 = 0)
    // we get c = 0, and equation becomes:
    // y = ax^2 + bx
    // 1) Y1 = a . X1^2 + b X1
    // 2) Y2 = a . X2^2 + b X2
    // solving simulatenous equations gets:

    // make time (x) & position (y) relative to x0, y0
    y1 -= y0;
    y2 -= y0;
    x1 -= x0;
    x2 -= x0;

    x -= x0;

    Vector2 a = ( y1 - (y2 * x1) / x2 ) / (x1 * (x1 - x2) );
    Vector2 b = ( y1 / x1 ) - (a * x1);

    return a * x * x + b * x + y0;
  }

  /**
   * Retrieves a value from the history relative to the head.
   *
   * @note If the Keys (x) in the history decrease in value the further back you go. Then a
   * negative deltaX value should be supplied to refer to these keys relative to the head key.
   *
   * @param[in] deltaX Key position to retrieve relative to head key
   *
   * @return The interpolated Value is returned for this relative position.
   */
  Vector2 GetRelativeToHead(float deltaX) const
  {
    HistoryContainerIter i = mHistory.end();
    --i;
    return Get(i->first + deltaX);
  }

  /**
   * Retrieves the head time value.
   *
   * @retrun The head time value.
   */
  float GetHeadTime() const
  {
    HistoryContainerIter i = mHistory.end();
    if(i==mHistory.begin())
    {
      return 0.0f;
    }
    --i;
    return i->first;
  }

  /**
   * Retrieves the head value.
   *
   * @return The head value.
   */
  Vector2 GetHead() const
  {
    HistoryContainerIter i = mHistory.end();
    if(i==mHistory.begin())
    {
      return Vector2();
    }
    --i;
    return i->second;
  }

private:

  HistoryContainer mHistory;                ///< History container
  size_t mMaxSize;                          ///< Current maximum size of container

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_HISTORY_H__
