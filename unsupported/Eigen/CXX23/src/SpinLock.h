#ifndef EIGEN_CXX23_SPINLOCK_H
#define EIGEN_CXX23_SPINLOCK_H

#include <atomic>
#include <thread>
#include <new>
#if defined(_POSIX_VERSION)
#  include <sched.h>
#endif

namespace Eigen {
namespace cxx23 {

class SpinLock {
  alignas(std::hardware_destructive_interference_size)
  std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
  constexpr SpinLock() noexcept = default;
  SpinLock(const SpinLock&) = delete;
  SpinLock& operator=(const SpinLock&) = delete;

  static inline void pause() noexcept {
#if defined(_POSIX_VERSION)
    sched_yield();
#else
    std::this_thread::yield();
#endif
  }

  void lock() noexcept {
    while(flag.test_and_set(std::memory_order_acquire)) {
      pause();
    }
  }

  bool try_lock() noexcept {
    return !flag.test_and_set(std::memory_order_acquire);
  }

  void unlock() noexcept {
    flag.clear(std::memory_order_release);
  }
};

class SpinLockGuard {
  SpinLock& lock_;
public:
  explicit SpinLockGuard(SpinLock& l) noexcept : lock_(l) {
    lock_.lock();
  }
  ~SpinLockGuard() noexcept { lock_.unlock(); }
  SpinLockGuard(const SpinLockGuard&) = delete;
  SpinLockGuard& operator=(const SpinLockGuard&) = delete;
};

} // namespace cxx23
} // namespace Eigen

#endif // EIGEN_CXX23_SPINLOCK_H
