# System Resource Monitor

## Overview
A CLI tool that monitors and displays real-time system resource usage (CPU, memory, disk). This project teaches project structure, file organization, and integration with system-specific APIs.

Cross-platform (Linux/maxOS/Windows) via `src/utils/` implementations (`SystemInfo_Linux.cpp`, `SystemInfo_MacOS.cpp`, `SystemInfo_Windows.cpp`).

## Features
- **Real-time metrics**: `SystemMonitor::refresh()` queries OS APIs; `SystemMonitor::display()` renders output.
- **Cross-platform backend**: `include/SystemInfo.h` (abstract interface) + platform implementation in `src/utils/`.
- **Modern CLI UI**: `include/Color.h` (ANSI colors), `include/Utils.h` (progress bars, dynamic usage colors, box drawing, `clamp()` helper).
- **Dynamic color by usage**: `utils::usageColor()` applies red (>=90%), yellow (>=70%), or green (<70%) thresholds.
- **Graceful exit**: Signal handling (`SIGINT`/`SIGTERM`) registered in `SystemMonitor::initialize()`; `main.cpp` exits cleanly on `Ctrl+C`.

## Project Structure
```
system-resource-monitor/
├── src/
│   ├── main.cpp                    # Thin entry point (loop + signal handling)
│   ├── SystemMonitor.cpp           # Orchestrator: display + refresh logic
│   ├── SystemInfo_Utils.cpp        # Platform factory (`createSystemInfo()`)
│   └── utils/
│       ├── SystemInfo_Linux.cpp   # Linux: `<sys/sysinfo.h>`
│       ├── SystemInfo_MacOS.cpp    # macOS: `sysctlbyname()` / Mach APIs
│       └── SystemInfo_Windows.cpp  # Windows: `GlobalMemoryStatusEx()` / `GetSystemTimes()`
├── include/
│   ├── SystemInfo.h                # Abstract `ISystemInfo` interface
│   ├── SystemMonitor.h             # `SystemMonitor` class (Pimpl idiom)
│   ├── Utils.h                     # `clamp()`, `bytesToGB()`, `clearScreen()`, `progressBar()`, `usageColor()`, `enableAnsiColors()`
│   └── Color.h                     # ANSI escape constants (`BRIGHT_RED`, `BOLD`, `RESET`, etc.)
├── tests/
│   └── test_system_info.cpp        # Minimal harness: factory validity, memory/CPU/disk ranges
├── .github/workflows/
│   ├── test.yml                    # Tests + 2-second binary smoke test on Linux/macOS/Windows VMs
│   ├── build.yml                   # Build artifacts on push/PR
│   └── release.yml                 # Publish binaries on `v*` tags
├── Project1_ResourceMonitor/
│   ├── REFACTORING_GUIDE.md        # `main.cpp` cleanup + `SystemMonitor` architecture
│   ├── CLI_UI_IMPROVEMENTS.md      # Colors, progress bars, box drawing, Windows VT processing
│   ├── CICD_BUILD_GUIDE.md         # GitHub Actions setup (build/test/release)
│   └── Resource-Monitor-PRD.md     # Project requirements and implementation guide
├── CMakeLists.txt                  # C++17, executable, tests (`system_monitor_tests`), install target
└── docs/README.md                  # Original overview document
```

## Build Instructions
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
./system_monitor
```

Optional (local build without manual `mkdir`):
```bash
make build     # Uses Makefile (optional)
```

## Testing
```bash
# Build the test executable
cmake --build . --config Release --target system_monitor_tests

# Run via CTest
cd build && ctest -C Release --output-on-failure
```

What is tested (`tests/test_system_info.cpp`):
- `createSystemInfo()` returns non-null.
- `getTotalMemory()` > 0.
- `getAvailableMemory()` in range [0, total].
- `getCpuUsage()` samples in [0, 100].
- `getDiskUsage()` in [0, 100].

## Architecture / How It Works
1. `main.cpp` creates a `SystemMonitor` and enters the loop.
2. `SystemMonitor::initialize()` calls `createSystemInfo()` (factory picks OS-specific `ISystemInfo` implementation from `SystemInfo_Utils.cpp`).
3. Each loop iteration:
   - `SystemMonitor::refresh()` queries OS metrics (`totalMemory`, `availMemory`, `cpuUsage`, `diskUsage`) and captures timestamp.
   - `SystemMonitor::display()` clears screen, builds formatted lines, renders a Unicode box (`printBox()`), and applies dynamic colors (`usageColor()` + `Color.h`).
4. `main.cpp` sleeps for 2 seconds (`std::this_thread::sleep_for`) before repeating.
5. Signal handlers (`SIGINT`/`SIGTERM`) set a global atomic flag (`g_signalRunning`) that stops the loop gracefully.

The `SystemMonitor` uses the **Pimpl idiom** (`struct Impl` inside the `.cpp` file) to hide implementation details from the header.

## CI/CD Workflow Chain
1. `test.yml` — Runs on `ubuntu-latest` / `macos-latest` / `windows-latest` VMs on every push/PR. Builds `system_monitor_tests`, runs `ctest`, and performs a 2-second binary smoke test (`timeout` kills the loop gracefully).
2. `build.yml` — Builds artifacts (`system_monitor-linux-amd64`, `system_monitor-macos-arm64`, `system_monitor-windows-amd64.exe`) on push/PR.
3. `release.yml` — Creates a GitHub Release with binary assets when a tag (`v*`) is pushed.

Reference: `Project1_ResourceMonitor/CICD_BUILD_GUIDE.md` (Sections 3-5).

## Author
Kshitij Das