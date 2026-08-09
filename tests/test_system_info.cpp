#include "SystemInfo.h"
#include <cstdlib>
#include <iostream>
#include <memory>

// Minimal test harness
namespace {
	int g_failure = 0;

	void check(bool condition, const char* name) {
		if (condition) {
			std::cout << " [PASS] " << name << std::endl;
		}
		else {
			std::cerr << "  [FAIL] " << name << std::endl;
			++g_failure;
		}
	}

	bool inRange(float value, float lo, float hi) {
		return value >= lo && value <= hi;
	}
} // anonymous namespace

// Tests
int main()
{
	std::cout << "Running system info tests..." << std::endl;

	// 1. Factory returns a valid object
	auto info = createSystemInfo();
	check(info != nullptr, "createSystemInfo() returns non-null");

	if (!info) {
		std::cerr << "\nAborting: could not create platform implementation." << std::endl;
		return EXIT_FAILURE;
	}

	// 2. Memory: total > 0
	uint64_t totalMemory = info->getTotalMemory();	
	check(totalMemory > 0, "total memory > 0");

	// 3. Memory: available in [0, total]
	uint64_t availMemory = info->getAvailableMemory();
	check(availMemory <= totalMemory, "available memory <= total memory");

	// 4. CPU: two-sample call (first returns 0, second returns delta)
	float cpu1 = info->getCpuUsage();
	float cpu2 = info->getCpuUsage();
	check(inRange(cpu1, 0.0f, 100.0f), "first CPU sample in [0, 100]");
	check(inRange(cpu2, 0.0f, 100.0f), "second CPU sample in [0, 100]");
	
	// 5. Disk: in [0, 100]
	float disk = info->getDiskUsage();
	check(inRange(disk, 0.0f, 100.0f), "disk usage in [0, 100]");

	// 6. Summary
	std::cout << std::endl;
	if (g_failure == 0) {
		std::cout << "All tests passed." << std::endl;
		return EXIT_SUCCESS;
	}
	else {
		std::cerr << g_failure << " test(s) failed." << std::endl;
		return EXIT_FAILURE;
	}
}	