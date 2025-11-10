# Lingti SDK

A lightweight C SDK for game traffic tunneling and network acceleration with real-time traffic monitoring.

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

### Minimum Code(6 lines) to work
```c
#include <stdio.h>
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

### Windows (MinGW)
```bash
gcc your_app.c -L. -llingti_sdk -o your_app.exe
```

### Windows (MSVC)
```bash
cl your_app.c lingti_sdk.lib
```

### Linux/macOS
```bash
gcc your_app.c -L. -llingti_sdk -o your_app
```

## Examples

See the `examples/` directory for complete working examples:
- `sdk_example.c` - Basic SDK usage demonstration
- `build_example.sh` - Linux/macOS build script
- `build_example.bat` - Windows build script

## License

Copyright (c) 2025 Ruilisi

## Version

Current version: 1.0.0
