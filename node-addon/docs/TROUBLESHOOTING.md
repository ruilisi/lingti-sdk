# Troubleshooting Guide

[English](TROUBLESHOOTING.md) | [简体中文](TROUBLESHOOTING.zh-CN.md)

Common issues and solutions for the Lingti SDK Node.js addon.

## Table of Contents

- [Platform Issues](#platform-issues)
- [Installation Problems](#installation-problems)
- [Configuration Errors](#configuration-errors)
- [Connection Issues](#connection-issues)
- [Performance Problems](#performance-problems)
- [Error Codes](#error-codes)
- [Debug Mode](#debug-mode)
- [Getting Help](#getting-help)

## Platform Issues

### Error: Platform not supported

**Problem:**
```javascript
if (!lingti.isAddonAvailable()) {
    // This returns false
}
```

**Cause:** The native addon only works on Windows. macOS and Linux are not supported.

**Solution:**
```javascript
const lingti = require('lingti-sdk');

if (!lingti.isAddonAvailable()) {
    console.error(`Platform ${lingti.getPlatform()} is not supported`);
    console.error('This addon requires Windows to run');
    process.exit(1);
}
```

**Workaround for Development:**
- Use a Windows virtual machine
- Test on a Windows machine
- Use Windows Subsystem for Linux (WSL) with Windows binaries

---

### Module did not self-register

**Problem:**
```
Error: Module did not self-register
```

**Causes:**
1. Node.js version mismatch
2. Corrupted node_modules
3. Wrong architecture (x86 vs x64)

**Solutions:**

1. **Rebuild the addon:**
```bash
cd node_modules/lingti-sdk
npm run rebuild
```

2. **Reinstall the package:**
```bash
npm uninstall lingti-sdk
npm cache clean --force
npm install lingti-sdk
```

3. **Check Node.js version:**
```bash
node --version  # Should be >= 16.0.0
```

4. **Ensure correct architecture:**
```bash
node -p "process.arch"  # Should match your OS (x64 for 64-bit Windows)
```

---

## Installation Problems

### npm install fails on Windows

**Problem:**
```
npm ERR! gyp ERR! build error
npm ERR! gyp ERR! stack Error: `msbuild.exe` failed with exit code: 1
```

**Causes:**
- Missing Visual Studio Build Tools
- Missing Python
- Wrong Python version

**Solutions:**

1. **Install Visual Studio Build Tools:**
```bash
# Download from: https://visualstudio.microsoft.com/downloads/
# Install "Desktop development with C++"
```

2. **Install Python 3.x:**
```bash
# Download from: https://www.python.org/downloads/
# Make sure to check "Add Python to PATH"
```

3. **Configure npm to use correct tools:**
```bash
npm config set msvs_version 2019
npm config set python "C:\Python39\python.exe"
```

4. **Try installation again:**
```bash
npm install lingti-sdk
```

---

### Package installs but addon doesn't work

**Problem:**
Package installs successfully but functions throw errors.

**Cause:** The addon binary wasn't built (common on non-Windows platforms).

**Solution:**

Check if the addon is available:
```javascript
const lingti = require('lingti-sdk');

console.log('Addon available:', lingti.isAddonAvailable());
console.log('Platform:', lingti.getPlatform());

if (lingti.isAddonAvailable()) {
    // Safe to use SDK functions
} else {
    // Addon not available on this platform
}
```

---

## Configuration Errors

### ERR_LOAD_CONFIG (-4): Failed to load config file

**Problem:**
```javascript
const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
// Returns -4
```

**Causes:**
1. File doesn't exist
2. Wrong file path
3. No read permissions

**Solutions:**

1. **Check if file exists:**
```javascript
const fs = require('fs');
const configPath = 'encrypted_config.txt';

if (!fs.existsSync(configPath)) {
    console.error('Config file not found:', configPath);
    console.log('Current directory:', process.cwd());
}
```

2. **Use absolute path:**
```javascript
const path = require('path');
const configPath = path.join(__dirname, 'encrypted_config.txt');
lingti.startTun2RWithConfigFile(configPath);
```

3. **Check file permissions:**
```javascript
try {
    fs.accessSync(configPath, fs.constants.R_OK);
    console.log('File is readable');
} catch (err) {
    console.error('Cannot read file:', err);
}
```

---

### ERR_NULL_CONFIG (-1): Invalid configuration

**Problem:**
```javascript
const result = lingti.startTun2R(config);
// Returns -1
```

**Causes:**
1. Empty configuration
2. Invalid Base64 encoding
3. Corrupted configuration

**Solutions:**

1. **Validate configuration format:**
```javascript
function validateConfig(config) {
    if (!config || typeof config !== 'string') {
        return false;
    }

    // Check if it's valid Base64
    const base64Regex = /^[A-Za-z0-9+/=]+$/;
    if (!base64Regex.test(config.trim())) {
        console.error('Invalid Base64 format');
        return false;
    }

    // Check minimum length
    if (config.length < 100) {
        console.error('Configuration too short');
        return false;
    }

    return true;
}

if (validateConfig(encryptedConfig)) {
    lingti.startTun2R(encryptedConfig);
} else {
    console.error('Invalid configuration');
}
```

2. **Re-fetch configuration from backend:**
```javascript
const newConfig = await fetchConfigFromBackend();
fs.writeFileSync('encrypted_config.txt', newConfig);
```

---

### ERR_JSON_PARSE (-2): JSON parsing error

**Problem:**
Configuration file contains invalid JSON or corrupted data.

**Solution:**

1. **Check file encoding (must be UTF-8):**
```javascript
const fs = require('fs');
const config = fs.readFileSync('encrypted_config.txt', 'utf8');
console.log('Config length:', config.length);
console.log('First 50 chars:', config.substring(0, 50));
```

2. **Ensure no BOM or extra whitespace:**
```javascript
const cleanConfig = config.trim().replace(/^\uFEFF/, ''); // Remove BOM
lingti.startTun2R(cleanConfig);
```

3. **Request new configuration:**
Contact backend service to generate a fresh configuration.

---

## Connection Issues

### ERR_ALREADY_RUN (-3): Service already running

**Problem:**
```javascript
lingti.startTun2RWithConfigFile();
// Returns -3: Service already running
```

**Solutions:**

1. **Check if service is running:**
```javascript
if (lingti.isServiceRunning()) {
    console.log('Service is already running');
    // Either use existing service or stop it first
    lingti.stopTun2R();
}
```

2. **Implement singleton pattern:**
```javascript
class TunnelService {
    static start(configPath) {
        if (lingti.isServiceRunning()) {
            console.log('Using existing tunnel');
            return 0;
        }

        return lingti.startTun2RWithConfigFile(configPath);
    }
}
```

---

### Tunnel starts but no traffic flows

**Problem:**
Service starts successfully but no data is transmitted.

**Diagnostics:**

1. **Check service status:**
```javascript
console.log('Service running:', lingti.isServiceRunning());

const stats = lingti.getTrafficStats();
console.log('TX bytes:', stats.txBytes);
console.log('RX bytes:', stats.rxBytes);
```

2. **Check network configuration:**
```javascript
const config = lingti.getConsoleConfig();
console.log('IP State:', config.stateStr);
console.log('IP Address:', config.ip);
console.log('Gateway:', config.gateway);

if (config.state !== lingti.ConsoleIPState.COMPLETED) {
    console.error('IP assignment not complete');
}
```

3. **Test connectivity:**
```javascript
lingti.runPing();

setTimeout(() => {
    const ping = lingti.getLastPingStats();
    console.log('Router ping:', ping.router, 'ms');
    console.log('Takeoff ping:', ping.takeoff, 'ms');

    if (ping.router < 0) {
        console.error('Cannot reach router');
    }
}, 2000);
```

**Solutions:**

1. Verify firewall settings allow the application
2. Check if game executable is properly configured
3. Ensure Windows driver (lingtiwfp64.sys) is in the correct location
4. Try restarting the service
5. Check DNS settings with `lingti.flushDNSCache()`

---

### High latency or packet loss

**Problem:**
Ping values are high or unstable.

**Diagnostics:**
```javascript
lingti.runPing();

const monitor = setInterval(() => {
    const ping = lingti.getLastPingStats();
    const stats = lingti.getTrafficStats();

    console.log({
        router: ping.router,
        takeoff: ping.takeoff,
        landing: ping.landing,
        txPkts: stats.txPkts,
        rxPkts: stats.rxPkts
    });

    // Check for packet loss
    const ratio = stats.rxPkts / stats.txPkts;
    if (ratio < 0.9) {
        console.warn('Possible packet loss detected');
    }

    // Check for high latency
    if (ping.router > 100) {
        console.warn('High router latency');
    }
}, 5000);
```

**Solutions:**

1. **Switch to different routing line:**
```javascript
// Get new configuration with different line
const newConfig = await getConfigWithDifferentLine(gameId, differentLineId);
lingti.stopTun2R();
lingti.startTun2R(newConfig);
```

2. **Check local network:**
- Test local network speed
- Check for other bandwidth-intensive applications
- Try wired connection instead of WiFi

3. **Flush DNS cache:**
```javascript
lingti.flushDNSCache();
```

---

## Performance Problems

### High CPU usage

**Problem:**
Node.js process consuming excessive CPU.

**Causes:**
1. Polling too frequently
2. Too many concurrent monitoring intervals
3. Memory leaks

**Solutions:**

1. **Reduce polling frequency:**
```javascript
// Bad: Polling every 100ms
setInterval(getStats, 100);

// Good: Polling every 1-2 seconds
setInterval(getStats, 1000);
```

2. **Clear intervals properly:**
```javascript
let monitorInterval;

function startMonitoring() {
    // Stop existing interval first
    if (monitorInterval) {
        clearInterval(monitorInterval);
    }

    monitorInterval = setInterval(() => {
        const stats = lingti.getTrafficStats();
        updateUI(stats);
    }, 1000);
}

function stopMonitoring() {
    if (monitorInterval) {
        clearInterval(monitorInterval);
        monitorInterval = null;
    }
}

process.on('exit', stopMonitoring);
```

---

### Memory leaks

**Problem:**
Memory usage grows over time.

**Solutions:**

1. **Proper cleanup:**
```javascript
class TunnelMonitor {
    constructor() {
        this.intervals = [];
    }

    startMonitoring() {
        const interval = setInterval(() => {
            this.update();
        }, 1000);

        this.intervals.push(interval);
    }

    cleanup() {
        // Clear all intervals
        this.intervals.forEach(interval => clearInterval(interval));
        this.intervals = [];

        // Stop services
        lingti.stopPing();
        lingti.stopTun2R();
    }
}

// Use cleanup on exit
const monitor = new TunnelMonitor();
process.on('exit', () => monitor.cleanup());
process.on('SIGINT', () => {
    monitor.cleanup();
    process.exit(0);
});
```

2. **Avoid creating multiple service instances:**
```javascript
// Singleton pattern
class TunnelService {
    static instance = null;

    static getInstance() {
        if (!TunnelService.instance) {
            TunnelService.instance = new TunnelService();
        }
        return TunnelService.instance;
    }
}
```

---

## Error Codes

### Complete Error Code Reference

| Code | Constant | Description | Solution |
|------|----------|-------------|----------|
| 0 | SUCCESS | Operation successful | - |
| -1 | ERR_NULL_CONFIG | Invalid/null configuration | Check configuration format |
| -2 | ERR_JSON_PARSE | JSON parsing error | Verify configuration encoding |
| -3 | ERR_ALREADY_RUN | Service already running | Stop existing service first |
| -4 | ERR_LOAD_CONFIG | Failed to load config file | Check file path and permissions |
| -1 | ERR_NOT_RUNNING | Service not running | Start service before operation |

### Handling All Errors

```javascript
function handleTunnelError(errorCode) {
    const errorMsg = lingti.getLastErrorMessage();

    switch (errorCode) {
        case lingti.ErrorCodes.SUCCESS:
            return 'Success';

        case lingti.ErrorCodes.ERR_NULL_CONFIG:
            return `Invalid configuration: ${errorMsg}`;

        case lingti.ErrorCodes.ERR_JSON_PARSE:
            return `Configuration parsing failed: ${errorMsg}`;

        case lingti.ErrorCodes.ERR_ALREADY_RUN:
            return 'Tunnel is already running';

        case lingti.ErrorCodes.ERR_LOAD_CONFIG:
            return `Cannot load configuration file: ${errorMsg}`;

        case lingti.ErrorCodes.ERR_NOT_RUNNING:
            return 'Tunnel is not running';

        default:
            return `Unknown error (${errorCode}): ${errorMsg}`;
    }
}

// Usage
const result = lingti.startTun2RWithConfigFile();
if (result !== 0) {
    console.error(handleTunnelError(result));
}
```

---

## Debug Mode

### Enable Verbose Logging

```javascript
const lingti = require('lingti-sdk');

// Log all operations
function debugLog(operation, ...args) {
    console.log(`[DEBUG] ${new Date().toISOString()} - ${operation}`, ...args);
}

debugLog('Platform check', {
    available: lingti.isAddonAvailable(),
    platform: lingti.getPlatform(),
    version: lingti.getSDKVersion()
});

const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
debugLog('Start result', result);

if (result !== 0) {
    debugLog('Error', lingti.getLastErrorMessage());
}

// Monitor all stats
setInterval(() => {
    const stats = lingti.getTrafficStats();
    const ping = lingti.getLastPingStats();
    const config = lingti.getConsoleConfig();

    debugLog('Stats', {
        running: lingti.isServiceRunning(),
        traffic: stats,
        ping: ping,
        network: config
    });
}, 5000);
```

### Log to File

```javascript
const fs = require('fs');
const path = require('path');

class Logger {
    constructor(logFile = 'lingti-debug.log') {
        this.logFile = logFile;
        this.stream = fs.createWriteStream(logFile, { flags: 'a' });
    }

    log(level, message, data = {}) {
        const entry = {
            timestamp: new Date().toISOString(),
            level,
            message,
            data
        };

        this.stream.write(JSON.stringify(entry) + '\n');
        console.log(`[${level}] ${message}`, data);
    }

    close() {
        this.stream.end();
    }
}

const logger = new Logger();

try {
    const result = lingti.startTun2RWithConfigFile();
    logger.log('INFO', 'Tunnel start result', { code: result });

    if (result !== 0) {
        logger.log('ERROR', 'Failed to start', {
            code: result,
            message: lingti.getLastErrorMessage()
        });
    }
} catch (error) {
    logger.log('ERROR', 'Exception occurred', {
        error: error.message,
        stack: error.stack
    });
}

process.on('exit', () => logger.close());
```

---

## Getting Help

### Before Asking for Help

Collect the following information:

```javascript
const diagnostics = {
    // System info
    platform: process.platform,
    arch: process.arch,
    nodeVersion: process.version,

    // Addon info
    addonAvailable: lingti.isAddonAvailable(),
    sdkVersion: lingti.getSDKVersion(),

    // Service status
    serviceRunning: lingti.isServiceRunning(),

    // If service is running
    stats: lingti.isServiceRunning() ? lingti.getTrafficStats() : null,
    ping: lingti.isServiceRunning() ? lingti.getLastPingStats() : null,
    config: lingti.isServiceRunning() ? lingti.getConsoleConfig() : null,

    // Last error
    lastError: lingti.getLastErrorMessage()
};

console.log(JSON.stringify(diagnostics, null, 2));
```

### Support Channels

1. **GitHub Issues**: https://github.com/ruilisi/lingti-sdk/issues
2. **Community Forum**: https://xiemala.com/f/rY1aZz
3. **Email Support**: Contact your Lingti representative

### Include in Bug Reports

1. Full error message and stack trace
2. System diagnostics (from above)
3. Steps to reproduce the issue
4. Expected vs actual behavior
5. Configuration (without sensitive data)
6. SDK version and Node.js version

---

## See Also

- [API Reference](API.md) - Complete API documentation
- [Configuration Guide](CONFIGURATION.md) - Configuration management
- [Examples](EXAMPLES.md) - Code examples and use cases
