#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <random>
#include <chrono>

namespace Random {
  inline auto rd = std::random_device{};
  inline auto time = std::chrono::steady_clock::now().time_since_epoch().count();
  inline auto seed_seq = std::seed_seq{
  static_cast<std::seed_seq::result_type>(time), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
  inline static auto mt = std::mt19937{seed_seq};

  template <typename T>
  T get_int(T min, T max) {
	return std::uniform_int_distribution<T>{min, max}(mt);
  }

  template <typename T>
  T get_real(T min, T max) {
    return std::uniform_real_distribution<T>{min, max}(mt);
  }
}

#endif // RANDOM_HPP