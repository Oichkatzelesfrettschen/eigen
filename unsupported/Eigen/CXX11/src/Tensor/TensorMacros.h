// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2015 Benoit Steiner <benoit.steiner.goog@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef EIGEN_CXX11_TENSOR_TENSOR_META_MACROS_H
#define EIGEN_CXX11_TENSOR_TENSOR_META_MACROS_H

#include <type_traits>


/** use this macro in sfinae selection in templated functions
 *
 *   template<typename T,
 *            typename std::enable_if< isBanana<T>::value , int >::type = 0
 *   >
 *   void foo(){}
 *
 *   becomes =>
 *
 *   template<typename TopoType,
 *           SFINAE_ENABLE_IF( isBanana<T>::value )
 *   >
 *   void foo(){}
 */

// SFINAE requires variadic templates
#ifndef EIGEN_CUDACC
#if EIGEN_HAS_VARIADIC_TEMPLATES
  // SFINAE doesn't work for gcc <= 4.7
  #ifdef EIGEN_COMP_GNUC
    #if EIGEN_GNUC_AT_LEAST(4,8)
      #define EIGEN_HAS_SFINAE
    #endif
  #else
    #define EIGEN_HAS_SFINAE
  #endif
#endif
#endif
#define EIGEN_SFINAE_ENABLE_IF( __condition__ ) \
    std::enable_if_t<( __condition__ ), int> = 0

#if EIGEN_OS_WIN || EIGEN_OS_WIN64
#define EIGEN_SLEEP(n) Sleep(n)
#elif EIGEN_OS_GNULINUX
#define EIGEN_SLEEP(n) usleep(n * 1000);
#else
#define EIGEN_SLEEP(n) sleep(std::max<unsigned>(1, n/1000))
#endif

#endif
