// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2025
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Portions of this file are based on uv's spinlock implementation
// by Ben Noordhuis which is released under the X11/MIT license.

#ifndef EIGEN_CXX23_SPINLOCK_H
#define EIGEN_CXX23_SPINLOCK_H

#include <atomic>
#include <thread>
#include <new>
#if defined(__unix__) || defined(__APPLE__)
#  include <sched.h>
#endif

namespace Eigen {
namespace cxx23 {

namespace detail {
inline void spin_pause() noexcept {
#if defined(__unix__) || defined(__APPLE__)
  sched_yield();
#else
  std::this_thread::yield();
#endif
}
} // namespace detail

/**\class SpinLock
 * \ingroup CXX23_SpinLock_Module
 * \brief Minimal spin lock with guard utility.
 *
 * This spin lock is implemented using \c std::atomic_flag and is
 * suitable for POSIX environments as well as other platforms
 * supported by C++23.  It employs \ref EIGEN_THREAD_YIELD to avoid
 * hogging the CPU while waiting.
 */
class alignas(std::hardware_destructive_interference_size) SpinLock {
  std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
  constexpr SpinLock() noexcept = default;
  SpinLock(const SpinLock&) = delete;
  SpinLock& operator=(const SpinLock&) = delete;

  /** Acquire the spin lock.  Blocks until ownership is obtained. */
  inline void lock() noexcept {
    while (flag.test_and_set(std::memory_order_acquire)) {
      detail::spin_pause();
    }
  }

  /** Attempt to acquire the spin lock without blocking. */
  inline bool try_lock() noexcept {
    return !flag.test_and_set(std::memory_order_acquire);
  }

  /** Release the spin lock. */
  inline void unlock() noexcept {
    flag.clear(std::memory_order_release);
  }
};

/** RAII helper that acquires a SpinLock for the lifetime of the guard. */
class SpinLockGuard {
  SpinLock& lock_;

public:
  explicit SpinLockGuard(SpinLock& l) noexcept : lock_(l) { lock_.lock(); }
  SpinLockGuard(const SpinLockGuard&) = delete;
  SpinLockGuard& operator=(const SpinLockGuard&) = delete;
  ~SpinLockGuard() noexcept { lock_.unlock(); }
};

} // namespace cxx23
} // namespace Eigen

#endif // EIGEN_CXX23_SPINLOCK_H
