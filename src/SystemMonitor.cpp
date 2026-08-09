#include "SystemMonitor.h"
#include "Utils.h"
#include "Color.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

// Internal state
struct SystemMonitor::Impl {
	std::unique_ptr<ISystemInfo> sysInfo;
	std::atomic<bool> running{true};
	bool firstFrame = true;

	// Cached metrics
	uint64_t totalMemory = 0;
	uint64_t availMemory = 0;
	float cpuUsage = 0.0f;
	float diskUsage = 0.0f;
	std::string timestamp;
};

// Constructor / Destructor
SystemMonitor::SystemMonitor()
	: pImpl(std::make_unique<Impl>()) {}

SystemMonitor::~SystemMonitor() = default;

// Public interface
bool SystemMonitor::initialize() {
	pImpl->sysInfo = createSystemInfo();

	if (!pImpl->sysInfo) {
		std::cerr << Color::RED
				<< "Error: Could not create system info implementation."
				<< Color::RESET << std::endl;
		return false;
	}

	// Register signal handlers
	std::signal(SIGINT, [](int) {});
	std::signal(SIGTERM, [](int) {});

	return true;
}

void SystemMonitor::refresh() {
	// Query OS metrics
	pImpl->totalMemory = pImpl->sysInfo->getTotalMemory();
	pImpl->availMemory = pImpl->sysInfo->getAvailableMemory();
	pImpl->cpuUsage = pImpl->sysInfo->getCpuUsage();
	pImpl->diskUsage = pImpl->sysInfo->getDiskUsage();

	// Capture timestamp
	auto now = std::chrono::system_clock::now();
	std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);

	std::ostringstream oss;
	oss << std::put_time(std::localtime(&time_t_now), "%H:%M:%S");
	pImpl->timestamp = oss.str();
}

// Box Drawing
void SystemMonitor::printBox(const std::vector<std::string>& lines) const {
	// Find the longest line (ignoring ANSI codes for width calculation)
	int maxWidth = 0;

	for (const auto& line : lines) 
	{
		int visibleLen = 0;
		bool inEscape = false;
		for (char c : line) 
		{
			if (c == '\033') {
				inEscape = true;
				continue;
			}
			if (inEscape) {
				if (c == 'm') 
					inEscape = false;
				continue;
			}
			visibleLen++;
		}
		if (visibleLen > maxWidth)
			maxWidth = visibleLen;
	}

	int padding = 8; // 4 spaces on each side

	// Top border
	std::cout << Color::WHITE << "\xe2\x94\x8c";
	for (int i = 0; i < maxWidth + padding; i++)
		std::cout << "\xe2\x94\x80";
	std::cout << "\xe2\x94\x90" << Color::RESET << std::endl;

	// Content lines
	for (const auto& line : lines) 
	{
		int visibleLen = 0;
		bool inEscape = false;
		for (char c : line) 
		{
			if (c == '\033') {
				inEscape = true;
				continue;
			}
			if (inEscape) {
				if (c == 'm')
					inEscape = false;
				continue;
			}
			visibleLen++;
		}

		std::cout << Color::WHITE << "\xe2\x94\x82" << Color::RESET 
					<< " " << line;
		
		// Pad remaining space
		for (int i = visibleLen; i < maxWidth; i++) 
			std::cout << " ";
		
		std::cout << " " << Color::WHITE << "\xe2\x94\x82" << Color::RESET << std::endl;
	}
	
	// Bottom border
	std::cout << Color::WHITE << "\xe2\x94\x94";
	for (int i = 0; i < maxWidth + padding; i++)
		std::cout << "\xe2\x94\x80";
	std::cout << "\xe2\x94\x98" << Color::RESET << std::endl;
}

void SystemMonitor::display() const {
	// Clear screen (except first frame)
	if (!pImpl->firstFrame) {
		utils::clearScreen();
	}
	pImpl->firstFrame = false;

	// Build all lines for the box
	std::vector<std::string> lines;

	// Title
	lines.push_back(
		Color::CYAN + Color::BOLD + "System Resource Monitor" + Color::RESET
	);

	// Subtitle
	lines.push_back(
		Color::DIM + "Press Ctrl+C to exit" + Color::RESET
	);

	// Empty line
	lines.push_back("");
	
	// Time stamp
	lines.push_back(
		Color::DIM + "[" + pImpl->timestamp + "]" + Color::RESET
	);
	
	// Empty line
	lines.push_back("");

	// CPU line: Label + Progress Bar + Percentage
	{
		std::string color = utils::usageColor(pImpl->cpuUsage);
		std::ostringstream oss;
		oss << Color::BOLD << "CPU	" << Color::RESET << " "
			<< color << utils::progressBar(pImpl->cpuUsage) << " "
			<< std::fixed << std::setprecision(2) << pImpl->cpuUsage << "%"
			<< Color::RESET;
		lines.push_back(oss.str());
	}

	// Memory line: Label + Progress Bar + Percentage + GB info
	{
		float memPercent = 0.0f;
		if (pImpl->totalMemory > 0) {
			memPercent = (1.0f - static_cast<float>(pImpl->availMemory) / pImpl->totalMemory) * 100.0f;
		}

		std::string color = utils::usageColor(memPercent);
		std::ostringstream oss;
		oss << Color::BOLD << "Memory	" << Color::RESET << " "
			<< color << utils::progressBar(memPercent) << " "
			<< std::fixed << std::setprecision(2) << memPercent << "%"
			<< Color::RESET
			<< " (" << std::fixed << std::setprecision(2)
			<< utils::bytesToGB(pImpl->availMemory) << "GB free)";	
		lines.push_back(oss.str());
	}

	// Disk line: Label + Progress Bar + Percentage
	{
		std::string color = utils::usageColor(pImpl->diskUsage);
		std::ostringstream oss;
		oss << Color::BOLD << "Disk		" << Color::RESET << " "
			<< color << utils::progressBar(pImpl->diskUsage) << " "
			<< std::fixed << std::setprecision(2) << pImpl->diskUsage << "%"
			<< Color::RESET;
		lines.push_back(oss.str());
	}

	// Print the box
	printBox(lines);
}

bool SystemMonitor::isRunning() const {
	return pImpl->running.load();
}

void SystemMonitor::stop() {
	pImpl->running.store(false);
}