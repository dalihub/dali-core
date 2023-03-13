#ifndef DALI_INTERNAL_SCENEGRAPH_NODE_HELPER_H
#define DALI_INTERNAL_SCENEGRAPH_NODE_HELPER_H

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
 */

/*
 * Defines the given value, and adds an enumeration that contains it's
 * size.  The enumeration can be used by the wrapper templated types
 * below during structure definition. ("offsetof" requires the
 * structure to be completely defined before use, so cannot be
 * utilized here).
 */
#define BASE(_TYPE, _X)         \
  _TYPE _X;                     \
  enum                          \
  {                             \
    OFFSET_##_X = sizeof(_TYPE) \
  };

/*
 * Create a property template instance, passing in the previously
 * stored element, which utilizes that element's OFFSET enumeration to
 * compute the offset of the BASE element.
 *
 * It is expected that _TEMPLATE is a TransformManagerPropertyVector3
 * which takes a TransformManagerProperty enumeration and an offset
 * to the BASE TransformManagerData.
 *
 * It is assumed that successive elements are aligned, and that no padding
 * is inserted by the compiler.
 */
#define PROPERTY_WRAPPER(_BASE_ELEMENT, _TEMPLATE, _PROPERTY, _ELEMENT)      \
  using _TEMPLATE##_ELEMENT = _TEMPLATE<_PROPERTY, OFFSET_##_BASE_ELEMENT>;  \
  _TEMPLATE##_ELEMENT _ELEMENT;                                              \
  enum                                                                       \
  {                                                                          \
    OFFSET_##_ELEMENT = sizeof(_TEMPLATE##_ELEMENT) + OFFSET_##_BASE_ELEMENT \
  };

/*
 * Create a template instance, passing in the previously stored
 * element, utilizing that element's OFFSET enumeration to compute the offset
 * of the BASE element.
 *
 * It is expected that _TEMPLATE is a TransformManagerPropertyNNNN template
 * which takes an offset to the BASE TransformManagerData.
 *
 * It is assumed that successive elements are aligned, and that no padding
 * is inserted by the compiler.
 */
#define TEMPLATE_WRAPPER(_BASE_ELEMENT, _TEMPLATE, _ELEMENT)                 \
  using _TEMPLATE##_ELEMENT = _TEMPLATE<OFFSET_##_BASE_ELEMENT>;             \
  _TEMPLATE##_ELEMENT _ELEMENT;                                              \
  enum                                                                       \
  {                                                                          \
    OFFSET_##_ELEMENT = sizeof(_TEMPLATE##_ELEMENT) + OFFSET_##_BASE_ELEMENT \
  };

#endif // DALI_INTERNAL_SCENEGRAPH_NODE_HELPER_H
