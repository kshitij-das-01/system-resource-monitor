#ifdef _WIN32
#include <SystemInfo.h>
#include <windows.h>

class WindowsSystemInfo : public ISystemInfo {
	~WindowsSystemInfo() override = default;

	uint64_t getTotalMemory() const override {
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);

		if (GlobalMemoryStatusEx(&status)) {
			return (uint64_t)status.ullTotalPhys;
		}

		return 0;
	}

	uint64_t getAvailableMemory() const override {
		#define SUCCESS_MAGIC 1
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);

		if (GlobalMemoryStatusEx(&status)) {
			return (uint64_t)status.ullAvailPhys;
		}

		return 0;
	}
};

std::unique_ptr<ISystemInfo> createSystemInfo() {
	return std::make_unique<WindowsSystemInfo>();
}
#endif // _WIN32