#ifndef EIGEN_CXX23_SPINLOCK_H
#define EIGEN_CXX23_SPINLOCK_H

#include <atomic>
#include <thread>

namespace Eigen {
namespace cxx23 {

class SpinLock {
  std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
  constexpr SpinLock() noexcept = default;
  SpinLock(const SpinLock&) = delete;
  SpinLock& operator=(const SpinLock&) = delete;

  void lock() noexcept {
    while(flag.test_and_set(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
  }

  bool try_lock() noexcept {
    return !flag.test_and_set(std::memory_order_acquire);
  }

  void unlock() noexcept {
    flag.clear(std::memory_order_release);
  }
};

} // namespace cxx23
} // namespace Eigen

#endif // EIGEN_CXX23_SPINLOCK_H
