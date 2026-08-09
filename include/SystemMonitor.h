#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include "SystemInfo.h"
#include <memory>
#include <string>
#include <vector>

class SystemMonitor {
private:
	struct Impl;
	std::unique_ptr<Impl> pImpl;

	// Box drawing helper
	void printBox(const std::vector<std::string>& lines) const;

public:
	SystemMonitor();
	~SystemMonitor();

	// Non copyable (owns a system resource handle)
	SystemMonitor(const SystemMonitor&) = delete;
	SystemMonitor& operator=(const SystemMonitor&) = delete;

	// Initialize the monitor. Returns false if platform is unsupported.
	bool initialize();

	// Refresh all metrics from the OS and update internal state
	void refresh();

	// Display the current state to stdout with formatting
	void display() const;

	// Check if the monitor is running (for main loop control)
	bool isRunning() const;

	// Signal the monitor to stop.
	void stop();
};

#endif // SYSTEM_MONITOR_H