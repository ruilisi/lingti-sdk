# Lingti SDK - Node.js Native Addon

English | [简体中文](https://github.com/ruilisi/lingti-sdk/blob/main/node-addon/README.zh-CN.md)

Node.js native addon (N-API) for the Lingti SDK, providing network tunneling capabilities for game traffic routing.

This addon is located in the `node-addon` directory of the lingti-core project and links to the SDK DLL in `../dist/lingti_sdk/`.

## Prerequisites

- **Node.js** >= 16.0.0
- **For runtime**: Windows OS (the SDK DLL is Windows-only)
- **For building** (Windows only):
  - Visual Studio 2019 or later with C++ build tools
  - Python 3.x
  - node-gyp (installed automatically as dependency)

### Installing Build Tools

```bash
# Install Visual Studio Build Tools
# Download from: https://visualstudio.microsoft.com/downloads/

# Install node-gyp globally
npm install -g node-gyp
```

## Installation

```bash
# Navigate to the node-addon directory
cd node-addon

# Install dependencies
npm install

# Build the native addon
npm run build
```

## Usage

### Basic Example

```javascript
const lingti = require('lingti-sdk');

// Check platform compatibility first
if (!lingti.isAddonAvailable()) {
    console.log('Platform:', lingti.getPlatform());
    console.log('This addon requires Windows to run.');
    process.exit(1);
}

// Start the service with encrypted config file (base64 encoded text)
// To obtain encrypted config: visit https://game.lingti.com/sdk
// Select your game (需要加速的游戏) and tunnel line (线路)
const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
if (result === 0) {
    console.log('Service started successfully!');
    console.log('SDK Version:', lingti.getSDKVersion());
} else {
    console.error('Failed to start:', lingti.getLastErrorMessage());
}

// Check if service is running
if (lingti.isServiceRunning()) {
    console.log('Service is active');
}

// Get traffic statistics
const stats = lingti.getTrafficStats();
console.log('TX:', stats.txBytes, 'RX:', stats.rxBytes);

// Stop the service when done
lingti.stopTun2R();
```

### TypeScript Support

The addon includes full TypeScript definitions:

```typescript
import * as lingti from 'lingti-sdk';

// Start service with encrypted config file
lingti.startTun2RWithConfigFile('encrypted_config.txt');
```

## Documentation

### Complete Guides

- **[API Reference](docs/API.md)** - Complete API documentation with detailed examples
- **[Configuration Guide](docs/CONFIGURATION.md)** - How to obtain and use encrypted configurations
- **[Examples](docs/EXAMPLES.md)** - Practical code examples and use cases
- **[Troubleshooting](docs/TROUBLESHOOTING.md)** - Common issues and solutions

### Quick Links

- [Installation](#installation)
- [Basic Usage](#usage)
- [Testing](#testing)
- [Platform Support](#platform-support)

## API Reference

### Core Functions

#### `startTun2R(encryptedConfig)`

Start the TUN2R service with encrypted configuration (base64 encoded text).

- **Parameters:**
  - `encryptedConfig` (string): Base64 encoded encrypted configuration text
- **Returns:** `number` - 0 on success, negative error code on failure

```javascript
// Use encrypted config text (base64 encoded)
const encryptedConfig = "...base64 encrypted config...";
const result = lingti.startTun2R(encryptedConfig);
```

#### `startTun2RWithConfigFile([configPath])`

Start the service using an encrypted configuration file (base64 encoded text).

- **Parameters:**
  - `configPath` (string, optional): Path to encrypted config file (defaults to 'encrypted_config.txt')
- **Returns:** `number` - 0 on success, negative error code on failure

```javascript
const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
```

#### `stopTun2R()`

Stop the service gracefully.

- **Returns:** `number` - 0 on success, negative error code on failure

```javascript
lingti.stopTun2R();
```

#### `isServiceRunning()`

Check if the service is currently running.

- **Returns:** `boolean` - true if running, false otherwise

```javascript
const running = lingti.isServiceRunning();
```

### Information Functions

#### `getSDKVersion()`

Get the SDK version string.

- **Returns:** `string` - Version (e.g., "1.4.3")

```javascript
const version = lingti.getSDKVersion();
```

#### `getLastErrorMessage()`

Get the last error message.

- **Returns:** `string` - Error message

```javascript
const error = lingti.getLastErrorMessage();
```

### Statistics Functions

#### `getTrafficStats()`

Get current traffic statistics.

- **Returns:** `object`
  - `txBytes` (number): Transmitted bytes
  - `rxBytes` (number): Received bytes
  - `txPkts` (number): Transmitted packets
  - `rxPkts` (number): Received packets

```javascript
const stats = lingti.getTrafficStats();
console.log(`TX: ${stats.txBytes} bytes, RX: ${stats.rxBytes} bytes`);
```

#### `getLastPingStats()`

Get the latest ping statistics.

- **Returns:** `object`
  - `router` (number): Ping to router in ms
  - `takeoff` (number): Ping to takeoff server in ms
  - `landing` (number): Ping to landing server in ms

```javascript
const ping = lingti.getLastPingStats();
console.log(`Router: ${ping.router}ms, Takeoff: ${ping.takeoff}ms`);
```

### Ping Monitoring

#### `runPing()`

Start periodic ping monitoring.

- **Returns:** `number` - 0 on success, negative error code on failure

```javascript
lingti.runPing();
```

#### `stopPing()`

Stop periodic ping monitoring.

- **Returns:** `number` - 0 on success, negative error code on failure

```javascript
lingti.stopPing();
```

### Utility Functions

#### `flushDNSCache()`

Flush the DNS cache.

- **Returns:** `number` - 0 on success

```javascript
lingti.flushDNSCache();
```

#### `getConsoleConfig()`

Get console configuration parameters.

- **Returns:** `object`
  - `state` (number): IP state code (0-3)
  - `stateStr` (string): State description
  - `gateway` (string): Gateway address
  - `mask` (string): Subnet mask
  - `ip` (string): Console IP
  - `dns` (string): DNS server

```javascript
const config = lingti.getConsoleConfig();
console.log(`State: ${config.stateStr}, IP: ${config.ip}`);
```

### Constants

#### `ErrorCodes`

Error code constants:

- `SUCCESS` (0): Operation successful
- `ERR_NULL_CONFIG` (-1): Invalid/null configuration
- `ERR_JSON_PARSE` (-2): JSON parsing error
- `ERR_ALREADY_RUN` (-3): Service already running
- `ERR_LOAD_CONFIG` (-4): Failed to load config file
- `ERR_NOT_RUNNING` (-1): Service not running

```javascript
if (result === lingti.ErrorCodes.ERR_ALREADY_RUN) {
    console.log('Service is already running');
}
```

#### `ConsoleIPState`

Console IP state constants:

- `COMPLETED` (0): IP assignment successful
- `FAILED` (1): IP assignment failed
- `IDLE` (2): Not started
- `IN_PROGRESS` (3): IP assignment in progress

```javascript
const config = lingti.getConsoleConfig();
if (config.state === lingti.ConsoleIPState.COMPLETED) {
    console.log('IP assignment completed');
}
```

## Testing

Run the test suite:

```bash
npm test
```

**On Windows**: Runs full test suite with all SDK functions
**On macOS/Linux**: Shows platform info and available constants

See `example.js` for a simple usage example with platform detection.

## Building

### Development Build

```bash
npm run build
```

### Clean Build

```bash
npm run clean
npm run build
```

## Architecture

The addon consists of:

1. **Native Layer** (`src/addon.cc`): C++ N-API bindings to the DLL
2. **JavaScript Layer** (`index.js`): High-level JavaScript wrapper
3. **TypeScript Definitions** (`index.d.ts`): Type definitions for TypeScript

## Platform Support

- **Windows**: Full support (native DLL)
- **macOS/Linux**: Package installs but native addon is not built (Windows-only DLL)

**Note:** The package can be installed on any platform, but the native addon will only build and work on Windows. On macOS/Linux, the installation will complete successfully but skip the native build step.

## License

Proprietary - Copyright (c) 2025 Ruilisi

## Version

SDK Version: 1.5.6

---

# Underlying C SDK

This Node.js addon wraps the Lingti C SDK. Below is the complete documentation for the underlying C SDK.

## C SDK Overview

Lingti SDK is a high-performance network tunneling library designed for game traffic optimization. It provides a simple C API for integrating network acceleration capabilities into games and applications, featuring real-time traffic monitoring, intelligent routing, and cross-platform support.

## C SDK Installation

### Download Required Files

The SDK is open source and available at: **https://github.com/ruilisi/lingti-sdk**

**Pre-compiled DLL/lib files for each SDK version are available in the [GitHub Releases](https://github.com/ruilisi/lingti-sdk/releases) section.**

Each release includes:
- `lingti_sdk.dll` - Main SDK library (13MB)
- `lingti_sdk.lib` - Import library for linking (8.6KB)
- `lingti_sdk.h` - C header file with API declarations
- `lingti_sdk.def` - Module definition file

**Note:** The `lingtiwfp64.sys` Windows driver file is included in the repository (not in releases, as it rarely changes). This file **must be placed in the same directory as your compiled executable** for the SDK to function properly on Windows.

### Understanding DLL vs LIB

#### What is the DLL?

The **DLL (Dynamic Link Library)** file (`lingti_sdk.dll`, 13MB) contains all the actual compiled code:
- Complete Go runtime and garbage collector
- All SDK functionality and business logic
- Network tunneling implementation
- Required at **runtime** when your application executes

**Runtime requirement:** `lingti_sdk.dll` must be present when your application runs. Place it:
- In the same directory as your `.exe` file (recommended)
- In a system directory (e.g., `C:\Windows\System32`)
- In any directory listed in your system's PATH environment variable

#### What is the LIB?

The **LIB (Import Library)** file (`lingti_sdk.lib`, 8.6KB) is much smaller because it contains only:
- Stub code with function name references
- Metadata telling the linker where to find functions in the DLL
- Import table information

**The small size (8.6KB vs 13MB) is normal and correct!** The import library only contains references to the 9 exported functions, not the actual implementation code.

**Compile-time requirement:** `lingti_sdk.lib` is only needed when compiling/linking your application with MSVC. It's not needed at runtime.

#### When to Use Each File

| File | Used When | Purpose |
|------|-----------|---------|
| `lingti_sdk.dll` | Runtime (always) | Contains all actual code, must be distributed with your app |
| `lingti_sdk.lib` | Compile-time (MSVC only) | Tells linker how to find DLL functions |
| `lingti_sdk.h` | Compile-time (always) | Provides function declarations for your C code |

#### Compiler-Specific Usage

**MSVC (Visual Studio):**
```bash
# Compilation requires .lib file
cl your_app.c lingti_sdk.lib

# Runtime requires .dll file in same directory as .exe
your_app.exe    # needs lingti_sdk.dll present
```

**MinGW/GCC:**
```bash
# Can link directly against .dll (no .lib needed)
gcc your_app.c -L. -llingti_sdk -o your_app.exe

# Runtime requires .dll file
./your_app.exe  # needs lingti_sdk.dll present
```

#### Distribution Checklist

When distributing your application, include:
- ✅ Your compiled `.exe` file
- ✅ `lingti_sdk.dll` (13MB - **required at runtime**)
- ✅ `lingtiwfp64.sys` (Windows driver - **required at runtime**)
- ❌ `lingti_sdk.lib` (NOT needed by end users)
- ❌ `lingti_sdk.h` (NOT needed by end users)

## C SDK Features

- **Simple C API** - Clean interface with start/stop service management
- **Asynchronous Operation** - Non-blocking service execution in background threads
- **Real-time Monitoring** - Track transmitted/received bytes and packets
- **DNS Management** - Built-in DNS cache control
- **Cross-platform** - Windows (DLL), Linux, and macOS support
- **Encrypted Configuration** - Secure encrypted config via string or file
- **Traffic Statistics** - Byte and packet-level monitoring
- **Error Handling** - Comprehensive error codes and messages

## C SDK Quick Start

### Minimum Code (5 lines) to work

```c
#include "../lingti_sdk.h"
int main() {
    StartTun2RWithConfigFile("encrypted_config.txt");
    return 0;
}
```

### C SDK Basic Usage

```c
#include <stdio.h>
#include "../lingti_sdk.h"

#ifdef _WIN32
#include <windows.h>
#define SLEEP(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP(ms) usleep((ms) * 1000)
#endif

int main() {
    printf("Lingti SDK Example\n");
    printf("==================\n\n");

    // Check SDK version
    char* version = GetSDKVersion();
    printf("SDK Version: %s\n\n", version);
    free(version);

    // Path to encrypted config file
    // For encryption details, see API.md
    const char* configFile = "encrypted_config.txt";

    printf("Starting service from config file...\n");
    int result = StartTun2RWithConfigFile(configFile);

    if (result != 0) {
        char* error = GetLastErrorMessage();
        printf("Failed to start service (code %d): %s\n", result, error);
        free(error);
        return 1;
    }

    printf("Service started successfully!\n\n");

    // Check service status
    if (IsServiceRunning()) {
        printf("Service status: RUNNING\n\n");
    }

    // Monitor traffic for 30 seconds
    printf("Monitoring traffic for 30 seconds...\n");
    printf("Press Ctrl+C to stop early\n\n");

    for (int i = 0; i < 30; i++) {
        unsigned long long txBytes, rxBytes;
        GetTrafficStats(&txBytes, &rxBytes, NULL, NULL);

        printf("\r[%02d/%02d] TX: %llu bytes | RX: %llu bytes",
               i + 1, 30, txBytes, rxBytes);
        fflush(stdout);

        SLEEP(1000);
    }

    printf("\n\n");

    // Stop the service
    printf("Stopping service...\n");
    result = StopTun2R();

    if (result == 0) {
        printf("Service stopped successfully!\n");
    } else {
        char* error = GetLastErrorMessage();
        printf("Failed to stop service (code %d): %s\n", result, error);
        free(error);
    }

    printf("\nExample completed. See API.md for detailed documentation.\n");

    return 0;
}
```

### Encrypted Config

The SDK **only** supports encrypted configuration for enhanced security.

To obtain an encrypted configuration:
1. Visit https://game.lingti.com/sdk
2. Select your game (需要加速的游戏)
3. Select your tunnel line (线路)
4. Copy the provided encrypted_config string

The encrypted_config is a Base64-encoded string that contains all necessary tunnel settings.

## C SDK API Reference

### Core Functions

- `StartTun2RWithConfigFile(const char* configPath)` - Start service from encrypted config file (base64 encoded text)
- `StopTun2R(void)` - Stop the service gracefully
- `IsServiceRunning(void)` - Check if service is running

### Monitoring Functions

- `GetTrafficStats(...)` - Get current traffic statistics
- `GetSDKVersion(void)` - Get SDK version string
- `GetLastErrorMessage(void)` - Get last error message
- `FlushDNSCache(void)` - Flush local DNS cache

### Memory Management

- Use standard C `free()` to release strings returned by SDK functions (`GetSDKVersion()`, `GetLastErrorMessage()`, `GetDeviceID()`, and string parameters from `GetConsoleConfig()`)

## C SDK Error Codes

- `LINGTI_SUCCESS (0)` - Operation successful
- `LINGTI_ERR_NULL_CONFIG (-1)` - Invalid/null configuration
- `LINGTI_ERR_JSON_PARSE (-2)` - JSON parsing error
- `LINGTI_ERR_ALREADY_RUN (-3)` - Service already running
- `LINGTI_ERR_LOAD_CONFIG (-4)` - Failed to load config file
- `LINGTI_ERR_NOT_RUNNING (-1)` - Service not running

## C SDK Building

### Using the Makefile (Recommended)

Build the example with all required files:

```bash
make example
```

This will create an `example/` directory containing:

- `example.exe` - Compiled executable
- `lingtiwfp64.sys` - Windows driver file
- `lingti_sdk.dll` - SDK library

Clean the build:

```bash
make clean
```

The Makefile automatically detects your platform:

- **Windows**: Uses native gcc or MinGW
- **Linux/macOS**: Uses MinGW cross-compiler (install with `brew install mingw-w64`)

### Manual Compilation

#### Windows (MinGW)

```bash
gcc your_app.c -L. -llingti_sdk -o your_app.exe
```

#### Windows (MSVC)

```bash
cl your_app.c lingti_sdk.lib
```

#### Linux/macOS (Cross-compile)

```bash
x86_64-w64-mingw32-gcc your_app.c lingti_sdk.lib -o your_app.exe
```

## C SDK Examples

See the `examples/` directory for complete working examples:

- `sdk_example.c` - Basic SDK usage demonstration
- `sdk_example_min.c` - Minimal 5-line example
