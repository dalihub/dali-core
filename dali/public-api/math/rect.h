#ifndef __DALI_RECT_H__
#define __DALI_RECT_H__

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

// EXTERNAL INCLUDES
#include <math.h>
#include <ostream>

// INTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

/**
 * @brief Template class to create and operate on rectangles.
 */

template< typename T = float >
struct Rect
{
// Methods

  /**
   * @brief Constructor.
   */
  Rect()
  : x(0),
    y(0),
    width(0),
    height(0)
  {
  }

  /**
   * @brief Constructor.
   *
   * @param [in] x       x coordinate (or left)
   * @param [in] y       y coordinate (or right)
   * @param [in] width   width (or bottom)
   * @param [in] height  height (or top)
   */
  Rect(T x, T y, T width, T height)
  : x(x),
    y(y),
    width(width),
    height(height)
  {
  }

  /**
   * @brief Copy constructor.
   *
   * @param [in] rhs  The original object
   */
  Rect(const Rect<T>& rhs)
  {
    x = rhs.x;
    y = rhs.y;
    width = rhs.width;
    height = rhs.height;
  }

  /**
   * @brief Assignment operator.
   *
   * @param [in] rhs  The original object
   * @return reference to this
   */
  Rect<T>& operator= (const Rect<T>& rhs)
  {
    if (this != &rhs)
    {
      x = rhs.x;
      y = rhs.y;
      width = rhs.width;
      height = rhs.height;
    }

    return *this;
  }

  /**
   * @brief Assignment from individual values.
   *
   * @param[in] newX      x coordinate
   * @param[in] newY      y coordinate
   * @param[in] newWidth  width
   * @param[in] newHeight height
   */
  void Set(T newX, T newY, T newWidth, T newHeight)
  {
    x = newX;
    y = newY;
    width = newWidth;
    height = newHeight;
  }

  /**
   * @brief Determines whether or not this Rectangle is empty.
   *
   * @return true if width or height are zero
   */
  bool IsEmpty() const
  {
    return width  == 0 ||
      height == 0;
  }

  /**
   * @brief Get the left of the rectangle.
   *
   * @return The left edge of the rectangle
   */
  T Left() const
  {
    return x;
  }
  /**
   * @brief Get the right of the rectangle.
   *
   * @return The right edge of the rectangle
   */
  T Right() const
  {
    return x + width;
  }

  /**
   * @brief Get the top of the rectangle.
   *
   * @return The top of the rectangle
   */
  T Top() const
  {
    return y;
  }

  /**
   * @brief Get the bottom of the rectangle.
   *
   * @return The bottom of the rectangle
   */
  T Bottom() const
  {
    return y + height;
  }

  /**
   * @brief Get the area of the rectangle.
   *
   * @return The area of the rectangle
   */
  T Area() const
  {
    return width * height;
  }

  /**
   * @brief Determines whether or not this rectangle and the specified rectangle intersect.
   *
   * @param[in] other  The other rectangle to test against this rectangle
   * @return           true if the rectangles intersect
   */
  bool Intersects(const Rect<T>& other) const
  {
    return (other.x + other.width)  > x           &&
      other.x                 < (x + width) &&
                                (other.y + other.height) > y           &&
      other.y                 < (y + height);
  }

  /**
   * @brief Determines whether or not this Rectangle contains the specified rectangle.
   *
   * @param[in] other  The other rectangle to test against this rectangle
   * @return           true if the specified rectangle is contained
   */
  bool Contains(const Rect<T>& other) const
  {
    return other.x                  >= x           &&
      (other.x + other.width)  <= (x + width) &&
      other.y                  >= y           &&
      (other.y + other.height) <= (y + height);
  }

public:   // Data

  union
  {
    T x;      ///< X position of the rectangle
    T left;   ///< The left value
  };

  union
  {
    T y;      ///< Y position of the rectangle
    T right;  ///< The right value
  };

  union
  {
    T width;  ///< width of the rectangle
    T bottom; ///< The bottom value
  };

  union
  {
    T height; ///< height of the rectangle
    T top;    ///< The top value
  };
};

/**
 * @brief Equality operator.
 *
 * @param[in] lhs First operand
 * @param[in] rhs Second operand
 * @return true if boxes are exactly same
 */
template< typename T >
inline bool operator==( const Rect<T>& lhs, const Rect<T>& rhs )
{
  return ( lhs.x == rhs.x )&&
    ( lhs.y == rhs.y )&&
    ( lhs.width == rhs.width )&&
    ( lhs.height == rhs.height );
}

/**
 * @brief Inequality operator.
 *
 * @param[in] lhs The first rectangle
 * @param[in] rhs The second rectangle
 * @return true if rectangles are not identical
 */
template< typename T >
inline bool operator!=( const Rect<T>& lhs, const Rect<T>& rhs )
{
  return !(lhs == rhs);
}

/**
 * @brief Equality operator specialization for float.
 *
 * @param[in] lhs The first rectangle
 * @param[in] rhs The second rectangle
 * @return true if rectangles are exactly same
 */
template<>
inline bool operator==( const Rect<float>& lhs, const Rect<float>& rhs )
{
  return ( fabsf( lhs.x - rhs.x ) < GetRangedEpsilon(lhs.x, rhs.x) )&&
    ( fabsf( lhs.y - rhs.y ) < GetRangedEpsilon(lhs.y, rhs.y) )&&
    ( fabsf( lhs.width - rhs.width ) < GetRangedEpsilon(lhs.width, rhs.width) )&&
    ( fabsf( lhs.height - rhs.height ) < GetRangedEpsilon(lhs.height, rhs.height) );
}

/**
 * @brief IsEmpty specialization for float.
 *
 * @return true if the rectangle has zero size.
 */
template<>
inline bool Rect<float>::IsEmpty() const
{
  return (fabsf(width)  <= GetRangedEpsilon(width, width)
          ||
          fabsf(height) <= GetRangedEpsilon(height, height));
}

/**
 * @brief Convert the value of the rectangle into a string and insert in to an output stream.
 *
 * @param [in] stream The output stream operator.
 * @param [in] rectangle the rectangle to output
 * @return The output stream operator.
 */
template< typename T >
inline std::ostream& operator<< (std::ostream& stream, const Rect<T>& rectangle)
{
  return stream << "[" << rectangle.x << ", " << rectangle.y << ", " << rectangle.width << ", " << rectangle.height << "]";
}

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_RECT_H__
