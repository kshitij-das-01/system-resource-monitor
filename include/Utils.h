#ifndef UTILS_H
#define UTILS_H

namespace utils {
	template <typename T>
	constexpr T clamp(T value, T min, T max) {
		return (value < min) ? min : (value > max) ? max : value;
	}
}
#endif // UTILS_H