#ifndef EIGEN_CXX23_SPINLOCK_H
#define EIGEN_CXX23_SPINLOCK_H

#include <atomic>
#include <new>
#include <thread>
#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#endif

namespace Eigen {
namespace cxx23 {

namespace detail {
inline void cpu_relax() noexcept {
#if defined(__x86_64__) || defined(__i386__)
    _mm_pause();
#elif defined(__aarch64__) || defined(__arm__)
    __asm__ __volatile__("yield");
#elif defined(__riscv)
    __asm__ __volatile__("pause");
#else
    std::this_thread::yield();
#endif
}
} // namespace detail

// Constant representing the cache line size used for alignment. When available
// `std::hardware_destructive_interference_size` yields the system's cache line
// size. Some toolchains emit warnings when this constant is used directly in an
// `alignas` expression. Therefore we wrap it in a constexpr definition and
// provide a fallback value of 64 bytes if the standard feature macro is missing.
#if defined(__cpp_lib_hardware_interference_size)
inline constexpr std::size_t kInterferenceSize =
    std::hardware_destructive_interference_size;
#else
inline constexpr std::size_t kInterferenceSize = 64u;
#endif

class SpinLock {
    // Align the flag to a cache line boundary using the fixed interference size.
    alignas(kInterferenceSize) std::atomic_flag flag = ATOMIC_FLAG_INIT;

  public:
    constexpr SpinLock() noexcept = default;
    SpinLock(const SpinLock &) = delete;
    SpinLock &operator=(const SpinLock &) = delete;

    void lock() noexcept {
        std::size_t spin = 0;
        while (flag.test_and_set(std::memory_order_acquire)) {
            if (spin < 16) {
                detail::cpu_relax();
                ++spin;
            } else {
                std::this_thread::yield();
            }
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
    SpinLock &lock_;

  public:
    explicit SpinLockGuard(SpinLock &l) noexcept : lock_(l) {
        lock_.lock();
    }
    ~SpinLockGuard() {
        lock_.unlock();
    }
    SpinLockGuard(const SpinLockGuard &) = delete;
    SpinLockGuard &operator=(const SpinLockGuard &) = delete;
};

} // namespace cxx23
} // namespace Eigen

#endif // EIGEN_CXX23_SPINLOCK_H
