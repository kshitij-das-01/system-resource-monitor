#include "SystemInfo.h"
#include "utils/SystemInfo_Linux.cpp"
#include "utils/SystemInfo_MacOS.cpp"
#include "utils/SystemInfo_Windows.cpp"

std::unique_ptr<ISystemInfo> createSystemInfo() {
	#if defined(__linux__)
		return std::make_unique<LinuxSystemInfo>();
	#elif defined(_WIN32)
		return std::make_unique<WindowsSystemInfo>();
	#elif defined(__APPLE__)
		return std::make_unique<MacOSSystemInfo>();
	#else
		std::cerr << "\n[ERROR] Unsupported or complex OS compilation target detected.\n";
		return nullptr;
	#endif
}