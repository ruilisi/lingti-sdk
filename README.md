# Lingti SDK
<div>
<img width="128" height="128" alt="image" src="https://github.com/user-attachments/assets/a83686c0-96f8-4b38-91e9-669f9f826461" style="text-align: center;" />
</div>

**灵缇游戏(game.lingti.com) / 灵缇路由(router.lingti.com)**

Lingti SDK is a lightweight C SDK for game traffic tunneling and network acceleration with real-time traffic monitoring.

## Overview

Lingti SDK is a high-performance network tunneling library designed for game traffic optimization. It provides a simple C API for integrating network acceleration capabilities into games and applications, featuring real-time traffic monitoring, intelligent routing, and cross-platform support.

## Features

- **Simple C API** - Clean interface with start/stop service management
- **Asynchronous Operation** - Non-blocking service execution in background threads
- **Real-time Monitoring** - Track transmitted/received bytes and packets
- **DNS Management** - Built-in DNS cache control
- **Cross-platform** - Windows (DLL), Linux, and macOS support
- **Flexible Configuration** - JSON-based config via string or file
- **Traffic Statistics** - Byte and packet-level monitoring
- **Error Handling** - Comprehensive error codes and messages

## Quick Start

### Minimum Code(5 lines) to work
```c
#include "../lingti_sdk.h"
int main() {
    StartTun2RWithConfigFile("encrypted_config.txt");
    return 0;
}
```
### Basic Usage

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
    FreeString(version);

    // Path to encrypted config file
    // For encryption details, see API.md
    const char* configFile = "encrypted_config.txt";

    printf("Starting service from config file...\n");
    int result = StartTun2RWithConfigFile(configFile);

    if (result != 0) {
        char* error = GetLastErrorMessage();
        printf("Failed to start service (code %d): %s\n", result, error);
        FreeString(error);
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
        FreeString(error);
    }

    printf("\nExample completed. See API.md for detailed documentation.\n");

    return 0;
}
```

### Encrypted Config
The SDK supports encrypted configuration files for enhanced security. 
To create an encrypted config file, one needs to select `game` and `line`, then handle them to lingti backend service to apply the encrypted text.
Related APIs will be provided once requested [here](https://xiemala.com/f/rY1aZz).


## API Reference

### Core Functions

- `StartTun2R(const char* configJSON)` - Start service with JSON config string
- `StartTun2RWithConfigFile(const char* configPath)` - Start service from config file
- `StopTun2R(void)` - Stop the service gracefully
- `IsServiceRunning(void)` - Check if service is running

### Monitoring Functions

- `GetTrafficStats(...)` - Get current traffic statistics
- `GetSDKVersion(void)` - Get SDK version string
- `GetLastErrorMessage(void)` - Get last error message
- `FlushDNSCache(void)` - Flush local DNS cache

### Memory Management

- `FreeString(char* s)` - Free strings returned by SDK functions

## Error Codes

- `LINGTI_SUCCESS (0)` - Operation successful
- `LINGTI_ERR_NULL_CONFIG (-1)` - Invalid/null configuration
- `LINGTI_ERR_JSON_PARSE (-2)` - JSON parsing error
- `LINGTI_ERR_ALREADY_RUN (-3)` - Service already running
- `LINGTI_ERR_LOAD_CONFIG (-4)` - Failed to load config file
- `LINGTI_ERR_NOT_RUNNING (-1)` - Service not running

## Building

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

## Examples

See the `examples/` directory for complete working examples:
- `sdk_example.c` - Basic SDK usage demonstration
- `sdk_example_min.c` - Minimal 5-line example

## License

Copyright (c) 2025 Ruilisi

## Version

Current version: 1.0.0
