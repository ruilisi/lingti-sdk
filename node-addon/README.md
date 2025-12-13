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

SDK Version: 1.5.5
