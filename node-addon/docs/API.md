# Lingti SDK Node.js Addon - API Reference

[English](API.md) | [简体中文](API.zh-CN.md)

Complete API reference for the Lingti SDK Node.js native addon.

## Table of Contents

- [Platform Detection](#platform-detection)
- [Service Management](#service-management)
- [Information & Monitoring](#information--monitoring)
- [Network Statistics](#network-statistics)
- [Ping Operations](#ping-operations)
- [Utility Functions](#utility-functions)
- [Constants](#constants)
- [Error Handling](#error-handling)

## Platform Detection

### `isAddonAvailable()`

Check if the native addon is available on the current platform.

**Returns:** `boolean`
- `true` - Native addon is available (Windows)
- `false` - Native addon is not available (macOS/Linux)

**Example:**
```javascript
const lingti = require('lingti-sdk');

if (!lingti.isAddonAvailable()) {
    console.error('This platform is not supported');
    process.exit(1);
}
```

**Use Case:** Always call this before using any other SDK functions to ensure platform compatibility.

---

### `getPlatform()`

Get the current platform name.

**Returns:** `string`
- `"win32"` - Windows
- `"darwin"` - macOS
- `"linux"` - Linux
- Other platform names as returned by Node.js `process.platform`

**Example:**
```javascript
console.log('Running on:', lingti.getPlatform());
```

---

## Service Management

### `startTun2R(encryptedConfig)`

Start the TUN2R tunneling service with encrypted configuration.

**Parameters:**
- `encryptedConfig` (string) - Base64 encoded encrypted configuration text

**Returns:** `number`
- `0` - Success
- `-1` - Invalid/null configuration (ERR_NULL_CONFIG)
- `-2` - JSON parsing error (ERR_JSON_PARSE)
- `-3` - Service already running (ERR_ALREADY_RUN)

**Example:**
```javascript
// Encrypted config text obtained from backend service
const encryptedConfig = "SGVsbG8gV29ybGQhIFRoaXMgaXMgYmFzZTY0...";

const result = lingti.startTun2R(encryptedConfig);
if (result === 0) {
    console.log('Service started successfully');
} else {
    console.error('Failed to start:', lingti.getLastErrorMessage());
}
```

**Notes:**
- Configuration must be base64 encoded encrypted text
- Service runs asynchronously in the background
- Call `isServiceRunning()` to verify service status
- Only one service instance can run at a time

---

### `startTun2RWithConfigFile([configPath])`

Start the TUN2R service using an encrypted configuration file.

**Parameters:**
- `configPath` (string, optional) - Path to encrypted config file
  - Default: `"encrypted_config.txt"` in current directory
  - Supports both relative and absolute paths

**Returns:** `number`
- `0` - Success
- `-1` - Invalid/null configuration (ERR_NULL_CONFIG)
- `-4` - Failed to load config file (ERR_LOAD_CONFIG)
- `-3` - Service already running (ERR_ALREADY_RUN)

**Example:**
```javascript
// Use default config file
let result = lingti.startTun2RWithConfigFile();

// Use custom config file path
result = lingti.startTun2RWithConfigFile('./config/encrypted_config.txt');

// Use absolute path
result = lingti.startTun2RWithConfigFile('C:\\Users\\Game\\config\\encrypted_config.txt');
```

**Notes:**
- Config file must contain base64 encoded encrypted configuration
- File is read synchronously at startup
- Returns ERR_LOAD_CONFIG if file doesn't exist or can't be read

---

### `stopTun2R()`

Stop the TUN2R service gracefully.

**Returns:** `number`
- `0` - Success
- `-1` - Service not running (ERR_NOT_RUNNING)

**Example:**
```javascript
const result = lingti.stopTun2R();
if (result === 0) {
    console.log('Service stopped successfully');
} else {
    console.log('Service was not running');
}
```

**Notes:**
- Graceful shutdown - waits for ongoing operations to complete
- Safe to call even if service is not running
- Automatically stops ping monitoring if active

---

### `isServiceRunning()`

Check if the TUN2R service is currently running.

**Returns:** `boolean`
- `true` - Service is running
- `false` - Service is not running

**Example:**
```javascript
if (lingti.isServiceRunning()) {
    console.log('Service is active');

    // Get statistics
    const stats = lingti.getTrafficStats();
    console.log('Traffic:', stats);
} else {
    console.log('Service is stopped');
}
```

**Use Case:**
- Check status before starting/stopping service
- Periodic health checks
- Prevent duplicate service instances

---

## Information & Monitoring

### `getSDKVersion()`

Get the SDK version string.

**Returns:** `string` - Version in semantic versioning format (e.g., "1.4.3")

**Example:**
```javascript
const version = lingti.getSDKVersion();
console.log('SDK Version:', version);

// Version comparison
const [major, minor, patch] = version.split('.').map(Number);
if (major >= 1 && minor >= 4) {
    console.log('Using supported SDK version');
}
```

---

### `getLastErrorMessage()`

Get the last error message from the SDK.

**Returns:** `string` - Human-readable error message

**Example:**
```javascript
const result = lingti.startTun2R(invalidConfig);
if (result !== 0) {
    const error = lingti.getLastErrorMessage();
    console.error('Error:', error);

    // Log to file or send to monitoring service
    logError({ code: result, message: error });
}
```

**Notes:**
- Message is in English
- Provides detailed context about the error
- Updated after each SDK operation

---

## Network Statistics

### `getTrafficStats()`

Get current traffic statistics for the tunnel.

**Returns:** `object`
```javascript
{
    txBytes: number,  // Transmitted bytes
    rxBytes: number,  // Received bytes
    txPkts: number,   // Transmitted packets
    rxPkts: number    // Received packets
}
```

**Example:**
```javascript
const stats = lingti.getTrafficStats();

console.log(`Sent: ${formatBytes(stats.txBytes)} (${stats.txPkts} packets)`);
console.log(`Received: ${formatBytes(stats.rxBytes)} (${stats.rxPkts} packets)`);

// Calculate throughput
function formatBytes(bytes) {
    if (bytes < 1024) return bytes + ' B';
    if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(2) + ' KB';
    if (bytes < 1024 * 1024 * 1024) return (bytes / (1024 * 1024)).toFixed(2) + ' MB';
    return (bytes / (1024 * 1024 * 1024)).toFixed(2) + ' GB';
}
```

**Use Case:**
- Real-time bandwidth monitoring
- Data usage tracking
- Performance metrics
- Billing calculations

**Notes:**
- Counters are cumulative since service start
- Reset to zero when service stops
- Updated in real-time

---

### `getLastPingStats()`

Get the most recent ping statistics to various network nodes.

**Returns:** `object`
```javascript
{
    router: number,   // Ping to router (ms)
    takeoff: number,  // Ping to takeoff server (ms)
    landing: number   // Ping to landing server (ms)
}
```

**Example:**
```javascript
// Must call runPing() first
lingti.runPing();

// Wait a moment for first ping
setTimeout(() => {
    const ping = lingti.getLastPingStats();

    console.log('Network Latency:');
    console.log(`  Router:  ${ping.router}ms`);
    console.log(`  Takeoff: ${ping.takeoff}ms`);
    console.log(`  Landing: ${ping.landing}ms`);

    // Check connection quality
    if (ping.router > 100) {
        console.warn('High latency to router');
    }
}, 1000);
```

**Notes:**
- Requires `runPing()` to be called first
- Returns last recorded values (not live)
- Values are `-1` if ping failed or not yet available

---

## Ping Operations

### `runPing()`

Start periodic ping monitoring to network nodes.

**Returns:** `number`
- `0` - Success
- Negative value - Error code

**Example:**
```javascript
// Start ping monitoring
const result = lingti.runPing();
if (result === 0) {
    // Monitor ping every 5 seconds
    const interval = setInterval(() => {
        const ping = lingti.getLastPingStats();
        updateUI(ping);
    }, 5000);

    // Stop monitoring after 60 seconds
    setTimeout(() => {
        clearInterval(interval);
        lingti.stopPing();
    }, 60000);
}
```

**Notes:**
- Pings are sent periodically in the background
- Does not block execution
- Multiple calls to `runPing()` are safe (no duplicate monitoring)

---

### `stopPing()`

Stop periodic ping monitoring.

**Returns:** `number`
- `0` - Success
- Negative value - Error code

**Example:**
```javascript
lingti.stopPing();
console.log('Ping monitoring stopped');
```

**Notes:**
- Safe to call even if ping monitoring is not active
- Last ping values remain available via `getLastPingStats()`

---

## Utility Functions

### `flushDNSCache()`

Flush the DNS cache.

**Returns:** `number`
- `0` - Success
- Negative value - Error code

**Example:**
```javascript
// Flush DNS after changing network
lingti.flushDNSCache();
console.log('DNS cache flushed');
```

**Use Case:**
- After network changes
- When DNS resolution issues occur
- Before starting tunnel service

---

### `getConsoleConfig()`

Get console network configuration parameters.

**Returns:** `object`
```javascript
{
    state: number,      // IP state code (0-3)
    stateStr: string,   // State description
    gateway: string,    // Gateway IP address
    mask: string,       // Subnet mask
    ip: string,         // Console IP address
    dns: string         // DNS server address
}
```

**Example:**
```javascript
const config = lingti.getConsoleConfig();

console.log('Network Configuration:');
console.log(`  Status: ${config.stateStr}`);
console.log(`  IP: ${config.ip}`);
console.log(`  Gateway: ${config.gateway}`);
console.log(`  DNS: ${config.dns}`);
console.log(`  Mask: ${config.mask}`);

// Check if IP assignment is complete
if (config.state === lingti.ConsoleIPState.COMPLETED) {
    console.log('Network configuration is ready');
}
```

**State Values:**
- `0` (COMPLETED) - IP assignment successful
- `1` (FAILED) - IP assignment failed
- `2` (IDLE) - Not started
- `3` (IN_PROGRESS) - IP assignment in progress

---

## Constants

### `ErrorCodes`

Error code constants for SDK operations.

**Properties:**
```javascript
{
    SUCCESS: 0,           // Operation successful
    ERR_NULL_CONFIG: -1,  // Invalid or null configuration
    ERR_JSON_PARSE: -2,   // JSON parsing error
    ERR_ALREADY_RUN: -3,  // Service already running
    ERR_LOAD_CONFIG: -4,  // Failed to load config file
    ERR_NOT_RUNNING: -1   // Service not running
}
```

**Example:**
```javascript
const result = lingti.startTun2R(config);

switch(result) {
    case lingti.ErrorCodes.SUCCESS:
        console.log('Started successfully');
        break;
    case lingti.ErrorCodes.ERR_ALREADY_RUN:
        console.log('Service is already running');
        break;
    case lingti.ErrorCodes.ERR_NULL_CONFIG:
        console.error('Invalid configuration');
        break;
    default:
        console.error('Unknown error:', result);
}
```

---

### `ConsoleIPState`

Console IP assignment state constants.

**Properties:**
```javascript
{
    COMPLETED: 0,     // IP assignment successful
    FAILED: 1,        // IP assignment failed
    IDLE: 2,          // Not started
    IN_PROGRESS: 3    // IP assignment in progress
}
```

**Example:**
```javascript
const config = lingti.getConsoleConfig();

switch(config.state) {
    case lingti.ConsoleIPState.COMPLETED:
        console.log('Ready to use');
        break;
    case lingti.ConsoleIPState.IN_PROGRESS:
        console.log('Configuring network...');
        break;
    case lingti.ConsoleIPState.FAILED:
        console.error('Network configuration failed');
        break;
    case lingti.ConsoleIPState.IDLE:
        console.log('Not configured yet');
        break;
}
```

---

## Error Handling

### Best Practices

**Always check return codes:**
```javascript
const result = lingti.startTun2R(config);
if (result !== 0) {
    const error = lingti.getLastErrorMessage();
    throw new Error(`SDK Error ${result}: ${error}`);
}
```

**Graceful degradation:**
```javascript
try {
    if (!lingti.isAddonAvailable()) {
        // Fall back to alternative implementation
        useAlternativeSDK();
    } else {
        lingti.startTun2RWithConfigFile();
    }
} catch (error) {
    console.error('Failed to start tunnel:', error);
    process.exit(1);
}
```

**Proper cleanup:**
```javascript
process.on('SIGINT', () => {
    console.log('Shutting down...');
    lingti.stopPing();
    lingti.stopTun2R();
    process.exit(0);
});
```

---

## Complete Example

```javascript
const lingti = require('lingti-sdk');

// Check platform
if (!lingti.isAddonAvailable()) {
    console.error('Platform not supported:', lingti.getPlatform());
    process.exit(1);
}

// Display version
console.log('SDK Version:', lingti.getSDKVersion());

// Start service
const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
if (result !== 0) {
    console.error('Failed to start:', lingti.getLastErrorMessage());
    process.exit(1);
}

// Start ping monitoring
lingti.runPing();

// Monitor traffic
const monitor = setInterval(() => {
    if (!lingti.isServiceRunning()) {
        clearInterval(monitor);
        return;
    }

    const stats = lingti.getTrafficStats();
    const ping = lingti.getLastPingStats();
    const config = lingti.getConsoleConfig();

    console.log('='.repeat(50));
    console.log('Traffic - TX:', stats.txBytes, 'RX:', stats.rxBytes);
    console.log('Ping - Router:', ping.router, 'ms');
    console.log('Network Status:', config.stateStr);
}, 5000);

// Cleanup on exit
process.on('SIGINT', () => {
    clearInterval(monitor);
    lingti.stopPing();
    lingti.stopTun2R();
    console.log('Service stopped');
    process.exit(0);
});
```

---

## See Also

- [Configuration Guide](CONFIGURATION.md) - How to configure encrypted config files
- [Examples](EXAMPLES.md) - More usage examples
- [Troubleshooting](TROUBLESHOOTING.md) - Common issues and solutions
