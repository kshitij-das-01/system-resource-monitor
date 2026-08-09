#ifndef UTILS_H
#define UTILS_H

#include "Color.h"
#include <cstdint>
#include <string>
#include <iostream>

namespace utils {

	// Clamp template
	template <typename T>
	constexpr T clamp(T value, T min, T max) {
		return (value < min) ? min : (value > max) ? max : value;
	}

	// Convert bytes to GB for display
	inline double bytesToGB(uint64_t bytes) {
		return static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0);
	}

	// Cross platform screen clear
	inline void clearScreen() {
	#ifdef _WIN32
		std::system("cls");
	#else 
		std::system("clear");
	#endif
	}

	// Generate a progress bar string
	inline std::string progressBar(float percentage, int width = 20) {
		int filled = static_cast<int>((percentage / 100.0f) * width);
		int empty = width - filled;

		std::string bar = "[";
		for (int i = 0; i < filled; i++)
			bar += "\xe2\x96\x88"; // filled boxes

		for (int i = 0; i < empty; i++)
			bar += "\xe2\x96\x91"; // empty boxes
		
		bar += "]";
		return bar;
	}

	// Get color code based on usage severity
	// Returns ANSI escape string
	inline std::string usageColor(float usage) {
		if (usage >= 90.0f)
			return Color::BRIGHT_RED;
		else if (usage >= 70.0f)
			return Color::BRIGHT_YELLOW;

		return Color::BRIGHT_GREEN;
	}

	inline void enableAnsiColors() {
	#ifdef _WIN32
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE) 
			return;

		DWORD dwMode = 0;
		if (!GetConsoleMode(hOut,&dwMode))
			return;
		
		// Enable ANSI escape code processing
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	#endif
		// macOS/Linux: ANSI codes work out of the box
	}
} // namespace utils
#endif // UTILS_H