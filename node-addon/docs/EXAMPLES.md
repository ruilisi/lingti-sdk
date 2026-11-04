# Examples

[English](EXAMPLES.md) | [简体中文](EXAMPLES.zh-CN.md)

Practical examples and use cases for the Lingti SDK Node.js addon.

## Table of Contents

- [Basic Usage](#basic-usage)
- [Electron Application](#electron-application)
- [Express Server Integration](#express-server-integration)
- [Game Launcher](#game-launcher)
- [Traffic Monitoring Dashboard](#traffic-monitoring-dashboard)
- [Multi-User Management](#multi-user-management)
- [Error Handling](#error-handling)
- [TypeScript Examples](#typescript-examples)

## Basic Usage

### Simple Start and Stop

```javascript
const lingti = require('lingti-sdk');

// Check platform support
if (!lingti.isAddonAvailable()) {
    console.error('This platform is not supported');
    process.exit(1);
}

console.log('SDK Version:', lingti.getSDKVersion());

// Start tunnel
// To obtain encrypted_config: visit https://game.lingti.com/sdk
// Select your game (需要加速的游戏) and tunnel line (线路)
const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
if (result === 0) {
    console.log('Tunnel started successfully');
} else {
    console.error('Failed to start:', lingti.getLastErrorMessage());
    process.exit(1);
}

// Monitor for 60 seconds
setTimeout(() => {
    // Get statistics
    const stats = lingti.getTrafficStats();
    console.log('Traffic Statistics:');
    console.log('  TX:', stats.txBytes, 'bytes');
    console.log('  RX:', stats.rxBytes, 'bytes');

    // Stop tunnel
    lingti.stopTun2R();
    console.log('Tunnel stopped');
}, 60000);
```

### With Ping Monitoring

```javascript
const lingti = require('lingti-sdk');

// Start service
lingti.startTun2RWithConfigFile();

// Start ping monitoring
lingti.runPing();

// Display stats every 5 seconds
const interval = setInterval(() => {
    const ping = lingti.getLastPingStats();
    const stats = lingti.getTrafficStats();

    console.clear();
    console.log('='.repeat(50));
    console.log('LINGTI TUNNEL STATUS');
    console.log('='.repeat(50));
    console.log(`Router Ping:  ${ping.router}ms`);
    console.log(`Takeoff Ping: ${ping.takeoff}ms`);
    console.log(`Landing Ping: ${ping.landing}ms`);
    console.log(`TX: ${formatBytes(stats.txBytes)}`);
    console.log(`RX: ${formatBytes(stats.rxBytes)}`);
}, 5000);

// Cleanup on exit
process.on('SIGINT', () => {
    clearInterval(interval);
    lingti.stopPing();
    lingti.stopTun2R();
    console.log('\nShutdown complete');
    process.exit(0);
});

function formatBytes(bytes) {
    if (bytes < 1024) return bytes + ' B';
    if (bytes < 1024 ** 2) return (bytes / 1024).toFixed(2) + ' KB';
    if (bytes < 1024 ** 3) return (bytes / (1024 ** 2)).toFixed(2) + ' MB';
    return (bytes / (1024 ** 3)).toFixed(2) + ' GB';
}
```

## Electron Application

### Main Process Integration

**main.js:**
```javascript
const { app, BrowserWindow, ipcMain } = require('electron');
const lingti = require('lingti-sdk');
const path = require('path');

let mainWindow;
let monitorInterval;

function createWindow() {
    mainWindow = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js'),
            contextIsolation: true
        }
    });

    mainWindow.loadFile('index.html');
}

// Start tunnel
ipcMain.handle('start-tunnel', async (event, configPath) => {
    if (!lingti.isAddonAvailable()) {
        return { success: false, error: 'Platform not supported' };
    }

    const result = lingti.startTun2RWithConfigFile(configPath);
    if (result === 0) {
        // Start ping monitoring
        lingti.runPing();

        // Send stats updates every second
        monitorInterval = setInterval(() => {
            const stats = lingti.getTrafficStats();
            const ping = lingti.getLastPingStats();
            mainWindow.webContents.send('stats-update', { stats, ping });
        }, 1000);

        return { success: true };
    } else {
        return {
            success: false,
            error: lingti.getLastErrorMessage()
        };
    }
});

// Stop tunnel
ipcMain.handle('stop-tunnel', async () => {
    if (monitorInterval) {
        clearInterval(monitorInterval);
        monitorInterval = null;
    }

    lingti.stopPing();
    const result = lingti.stopTun2R();

    return { success: result === 0 };
});

// Get tunnel status
ipcMain.handle('get-status', async () => {
    return {
        running: lingti.isServiceRunning(),
        version: lingti.getSDKVersion()
    };
});

app.whenReady().then(createWindow);

app.on('before-quit', () => {
    if (monitorInterval) {
        clearInterval(monitorInterval);
    }
    lingti.stopPing();
    lingti.stopTun2R();
});
```

**preload.js:**
```javascript
const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('lingtiAPI', {
    startTunnel: (configPath) => ipcRenderer.invoke('start-tunnel', configPath),
    stopTunnel: () => ipcRenderer.invoke('stop-tunnel'),
    getStatus: () => ipcRenderer.invoke('get-status'),
    onStatsUpdate: (callback) => ipcRenderer.on('stats-update', (event, data) => callback(data))
});
```

**renderer.js:**
```javascript
// Start tunnel button
document.getElementById('start-btn').addEventListener('click', async () => {
    const result = await window.lingtiAPI.startTunnel('encrypted_config.txt');
    if (result.success) {
        updateStatus('Connected');
    } else {
        alert('Failed to start: ' + result.error);
    }
});

// Stop tunnel button
document.getElementById('stop-btn').addEventListener('click', async () => {
    await window.lingtiAPI.stopTunnel();
    updateStatus('Disconnected');
});

// Listen for stats updates
window.lingtiAPI.onStatsUpdate((data) => {
    document.getElementById('tx-bytes').textContent = formatBytes(data.stats.txBytes);
    document.getElementById('rx-bytes').textContent = formatBytes(data.stats.rxBytes);
    document.getElementById('router-ping').textContent = data.ping.router + 'ms';
});

function formatBytes(bytes) {
    if (bytes < 1024) return bytes + ' B';
    if (bytes < 1024 ** 2) return (bytes / 1024).toFixed(2) + ' KB';
    if (bytes < 1024 ** 3) return (bytes / (1024 ** 2)).toFixed(2) + ' MB';
    return (bytes / (1024 ** 3)).toFixed(2) + ' GB';
}
```

## Express Server Integration

### RESTful API Server

```javascript
const express = require('express');
const lingti = require('lingti-sdk');
const app = express();

app.use(express.json());

let tunnelStatus = {
    running: false,
    startTime: null,
    stats: null
};

// Start tunnel endpoint
app.post('/api/tunnel/start', (req, res) => {
    const { configPath } = req.body;

    if (!lingti.isAddonAvailable()) {
        return res.status(400).json({
            success: false,
            error: 'Platform not supported'
        });
    }

    if (lingti.isServiceRunning()) {
        return res.status(400).json({
            success: false,
            error: 'Tunnel already running'
        });
    }

    const result = lingti.startTun2RWithConfigFile(configPath || 'encrypted_config.txt');

    if (result === 0) {
        tunnelStatus.running = true;
        tunnelStatus.startTime = Date.now();
        lingti.runPing();

        res.json({
            success: true,
            message: 'Tunnel started successfully',
            version: lingti.getSDKVersion()
        });
    } else {
        res.status(500).json({
            success: false,
            error: lingti.getLastErrorMessage()
        });
    }
});

// Stop tunnel endpoint
app.post('/api/tunnel/stop', (req, res) => {
    lingti.stopPing();
    const result = lingti.stopTun2R();

    tunnelStatus.running = false;
    tunnelStatus.startTime = null;

    res.json({
        success: result === 0,
        message: result === 0 ? 'Tunnel stopped' : 'Failed to stop tunnel'
    });
});

// Get tunnel status
app.get('/api/tunnel/status', (req, res) => {
    const running = lingti.isServiceRunning();

    if (running) {
        const stats = lingti.getTrafficStats();
        const ping = lingti.getLastPingStats();
        const uptime = Date.now() - tunnelStatus.startTime;

        res.json({
            running: true,
            uptime: Math.floor(uptime / 1000), // seconds
            stats: {
                txBytes: stats.txBytes,
                rxBytes: stats.rxBytes,
                txPkts: stats.txPkts,
                rxPkts: stats.rxPkts
            },
            ping: {
                router: ping.router,
                takeoff: ping.takeoff,
                landing: ping.landing
            }
        });
    } else {
        res.json({ running: false });
    }
});

// Health check
app.get('/health', (req, res) => {
    res.json({
        status: 'ok',
        platform: lingti.getPlatform(),
        addonAvailable: lingti.isAddonAvailable(),
        sdkVersion: lingti.getSDKVersion()
    });
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
    console.log(`Lingti API Server running on port ${PORT}`);
});

// Graceful shutdown
process.on('SIGTERM', () => {
    console.log('Shutting down...');
    lingti.stopPing();
    lingti.stopTun2R();
    process.exit(0);
});
```

## Game Launcher

### Automated Game Launch with Tunnel

```javascript
const lingti = require('lingti-sdk');
const { spawn } = require('child_process');
const path = require('path');

class GameLauncher {
    constructor(gameExePath, configPath) {
        this.gameExePath = gameExePath;
        this.configPath = configPath;
        this.gameProcess = null;
    }

    async startTunnel() {
        console.log('Starting tunnel...');

        const result = lingti.startTun2RWithConfigFile(this.configPath);
        if (result !== 0) {
            throw new Error(`Failed to start tunnel: ${lingti.getLastErrorMessage()}`);
        }

        // Wait for tunnel to establish
        await new Promise(resolve => setTimeout(resolve, 2000));

        // Verify tunnel is running
        if (!lingti.isServiceRunning()) {
            throw new Error('Tunnel failed to start');
        }

        console.log('Tunnel started successfully');
    }

    launchGame() {
        console.log('Launching game:', this.gameExePath);

        this.gameProcess = spawn(this.gameExePath, [], {
            cwd: path.dirname(this.gameExePath),
            detached: false
        });

        this.gameProcess.on('close', (code) => {
            console.log(`Game exited with code ${code}`);
            this.cleanup();
        });

        this.gameProcess.on('error', (err) => {
            console.error('Failed to launch game:', err);
            this.cleanup();
        });
    }

    cleanup() {
        console.log('Cleaning up...');
        lingti.stopTun2R();
        this.gameProcess = null;
    }

    async run() {
        try {
            await this.startTunnel();
            this.launchGame();
        } catch (error) {
            console.error('Error:', error.message);
            process.exit(1);
        }
    }
}

// Usage
const launcher = new GameLauncher(
    'C:\\Program Files\\Game\\game.exe',
    'encrypted_config.txt'
);

launcher.run();
```

## Traffic Monitoring Dashboard

### Real-time Console Dashboard

```javascript
const lingti = require('lingti-sdk');
const blessed = require('blessed');

class TunnelDashboard {
    constructor() {
        this.screen = blessed.screen({
            smartCSR: true,
            title: 'Lingti Tunnel Monitor'
        });

        this.createUI();
        this.startMonitoring();

        // Quit on Ctrl-C
        this.screen.key(['C-c'], () => {
            lingti.stopPing();
            lingti.stopTun2R();
            process.exit(0);
        });
    }

    createUI() {
        // Status box
        this.statusBox = blessed.box({
            top: 0,
            left: 0,
            width: '100%',
            height: 3,
            content: 'Status: Initializing...',
            tags: true,
            border: { type: 'line' },
            style: {
                fg: 'white',
                border: { fg: 'cyan' }
            }
        });

        // Traffic stats box
        this.trafficBox = blessed.box({
            top: 3,
            left: 0,
            width: '50%',
            height: 10,
            content: '',
            tags: true,
            label: ' Traffic Statistics ',
            border: { type: 'line' },
            style: {
                fg: 'green',
                border: { fg: 'green' }
            }
        });

        // Ping stats box
        this.pingBox = blessed.box({
            top: 3,
            left: '50%',
            width: '50%',
            height: 10,
            content: '',
            tags: true,
            label: ' Latency (ms) ',
            border: { type: 'line' },
            style: {
                fg: 'yellow',
                border: { fg: 'yellow' }
            }
        });

        // Log box
        this.logBox = blessed.log({
            top: 13,
            left: 0,
            width: '100%',
            height: '100%-13',
            label: ' Log ',
            tags: true,
            border: { type: 'line' },
            scrollable: true,
            style: {
                fg: 'white',
                border: { fg: 'blue' }
            }
        });

        this.screen.append(this.statusBox);
        this.screen.append(this.trafficBox);
        this.screen.append(this.pingBox);
        this.screen.append(this.logBox);
    }

    log(message) {
        this.logBox.log(message);
        this.screen.render();
    }

    update() {
        const running = lingti.isServiceRunning();

        // Update status
        if (running) {
            this.statusBox.setContent('{green-fg}Status: CONNECTED{/green-fg}');
        } else {
            this.statusBox.setContent('{red-fg}Status: DISCONNECTED{/red-fg}');
        }

        if (running) {
            // Update traffic stats
            const stats = lingti.getTrafficStats();
            this.trafficBox.setContent(
                `  TX: ${this.formatBytes(stats.txBytes)}\n` +
                `  RX: ${this.formatBytes(stats.rxBytes)}\n` +
                `  \n` +
                `  TX Packets: ${stats.txPkts}\n` +
                `  RX Packets: ${stats.rxPkts}`
            );

            // Update ping stats
            const ping = lingti.getLastPingStats();
            this.pingBox.setContent(
                `  Router:  ${this.formatPing(ping.router)}\n` +
                `  Takeoff: ${this.formatPing(ping.takeoff)}\n` +
                `  Landing: ${this.formatPing(ping.landing)}`
            );
        }

        this.screen.render();
    }

    formatBytes(bytes) {
        if (bytes < 1024) return bytes + ' B';
        if (bytes < 1024 ** 2) return (bytes / 1024).toFixed(2) + ' KB';
        if (bytes < 1024 ** 3) return (bytes / (1024 ** 2)).toFixed(2) + ' MB';
        return (bytes / (1024 ** 3)).toFixed(2) + ' GB';
    }

    formatPing(ms) {
        if (ms < 0) return 'N/A';
        if (ms < 50) return `{green-fg}${ms}ms{/green-fg}`;
        if (ms < 100) return `{yellow-fg}${ms}ms{/yellow-fg}`;
        return `{red-fg}${ms}ms{/red-fg}`;
    }

    startMonitoring() {
        this.log('Starting tunnel...');

        const result = lingti.startTun2RWithConfigFile();
        if (result === 0) {
            this.log('Tunnel started successfully');
            lingti.runPing();

            // Update every second
            setInterval(() => this.update(), 1000);
        } else {
            this.log(`Failed to start: ${lingti.getLastErrorMessage()}`);
        }
    }
}

// Start dashboard
new TunnelDashboard();
```

## Multi-User Management

### Managing Multiple Configurations

```javascript
const lingti = require('lingti-sdk');
const fs = require('fs');
const path = require('path');

class TunnelManager {
    constructor(configDir = './configs') {
        this.configDir = configDir;
        this.activeUser = null;

        if (!fs.existsSync(configDir)) {
            fs.mkdirSync(configDir, { recursive: true });
        }
    }

    saveUserConfig(userId, encryptedConfig) {
        const configPath = this.getUserConfigPath(userId);
        fs.writeFileSync(configPath, encryptedConfig, 'utf8');
        console.log(`Config saved for user ${userId}`);
    }

    getUserConfigPath(userId) {
        return path.join(this.configDir, `${userId}.config`);
    }

    async startForUser(userId) {
        if (lingti.isServiceRunning()) {
            throw new Error('Another tunnel is already running');
        }

        const configPath = this.getUserConfigPath(userId);
        if (!fs.existsSync(configPath)) {
            throw new Error(`No config found for user ${userId}`);
        }

        const result = lingti.startTun2RWithConfigFile(configPath);
        if (result !== 0) {
            throw new Error(lingti.getLastErrorMessage());
        }

        this.activeUser = userId;
        console.log(`Tunnel started for user ${userId}`);
    }

    stop() {
        if (!lingti.isServiceRunning()) {
            return;
        }

        lingti.stopTun2R();
        console.log(`Tunnel stopped for user ${this.activeUser}`);
        this.activeUser = null;
    }

    getActiveUser() {
        return this.activeUser;
    }
}

// Usage
const manager = new TunnelManager();

// Save configs for different users
manager.saveUserConfig('user1', 'encrypted_config_1...');
manager.saveUserConfig('user2', 'encrypted_config_2...');

// Start for user1
manager.startForUser('user1')
    .then(() => {
        console.log('Active user:', manager.getActiveUser());

        // Switch to user2 after 30 seconds
        setTimeout(() => {
            manager.stop();
            manager.startForUser('user2');
        }, 30000);
    })
    .catch(error => console.error('Error:', error));
```

## Error Handling

### Comprehensive Error Handling

```javascript
const lingti = require('lingti-sdk');

class TunnelService {
    constructor(configPath) {
        this.configPath = configPath;
        this.retryCount = 0;
        this.maxRetries = 3;
    }

    async start() {
        // Platform check
        if (!lingti.isAddonAvailable()) {
            throw new Error(`Platform ${lingti.getPlatform()} is not supported`);
        }

        // Check if already running
        if (lingti.isServiceRunning()) {
            console.log('Tunnel is already running');
            return;
        }

        // Attempt to start with retries
        while (this.retryCount < this.maxRetries) {
            try {
                const result = lingti.startTun2RWithConfigFile(this.configPath);

                if (result === lingti.ErrorCodes.SUCCESS) {
                    console.log('Tunnel started successfully');
                    this.retryCount = 0;
                    return;
                }

                // Handle specific errors
                this.handleError(result);

            } catch (error) {
                console.error('Unexpected error:', error);
                this.retryCount++;

                if (this.retryCount < this.maxRetries) {
                    console.log(`Retrying... (${this.retryCount}/${this.maxRetries})`);
                    await this.delay(2000);
                } else {
                    throw new Error(`Failed after ${this.maxRetries} attempts`);
                }
            }
        }
    }

    handleError(errorCode) {
        const errorMsg = lingti.getLastErrorMessage();

        switch (errorCode) {
            case lingti.ErrorCodes.ERR_NULL_CONFIG:
                throw new Error('Invalid configuration: ' + errorMsg);

            case lingti.ErrorCodes.ERR_LOAD_CONFIG:
                throw new Error(`Config file not found: ${this.configPath}`);

            case lingti.ErrorCodes.ERR_JSON_PARSE:
                throw new Error('Config parsing failed: ' + errorMsg);

            case lingti.ErrorCodes.ERR_ALREADY_RUN:
                console.log('Service already running');
                return;

            default:
                throw new Error(`Unknown error (${errorCode}): ${errorMsg}`);
        }
    }

    delay(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    stop() {
        try {
            lingti.stopTun2R();
            console.log('Tunnel stopped');
        } catch (error) {
            console.error('Error stopping tunnel:', error);
        }
    }
}

// Usage
const service = new TunnelService('encrypted_config.txt');

service.start()
    .then(() => console.log('Running'))
    .catch(error => console.error('Failed to start:', error));
```

## TypeScript Examples

### Type-Safe Implementation

```typescript
import * as lingti from 'lingti-sdk';

interface TunnelStats {
    txBytes: number;
    rxBytes: number;
    txPkts: number;
    rxPkts: number;
}

interface PingStats {
    router: number;
    takeoff: number;
    landing: number;
}

class TypedTunnelService {
    private configPath: string;
    private monitorInterval?: NodeJS.Timeout;

    constructor(configPath: string) {
        this.configPath = configPath;
    }

    start(): void {
        if (!lingti.isAddonAvailable()) {
            throw new Error('Platform not supported');
        }

        const result = lingti.startTun2RWithConfigFile(this.configPath);

        if (result !== 0) {
            throw new Error(lingti.getLastErrorMessage());
        }

        lingti.runPing();
    }

    stop(): void {
        if (this.monitorInterval) {
            clearInterval(this.monitorInterval);
        }

        lingti.stopPing();
        lingti.stopTun2R();
    }

    getStats(): TunnelStats {
        return lingti.getTrafficStats();
    }

    getPing(): PingStats {
        return lingti.getLastPingStats();
    }

    startMonitoring(callback: (stats: TunnelStats, ping: PingStats) => void): void {
        this.monitorInterval = setInterval(() => {
            const stats = this.getStats();
            const ping = this.getPing();
            callback(stats, ping);
        }, 1000);
    }
}

// Usage
const service = new TypedTunnelService('encrypted_config.txt');

try {
    service.start();

    service.startMonitoring((stats, ping) => {
        console.log('TX:', stats.txBytes, 'RX:', stats.rxBytes);
        console.log('Ping:', ping.router, 'ms');
    });

    setTimeout(() => service.stop(), 60000);
} catch (error) {
    console.error('Error:', error);
}
```

## See Also

- [API Reference](API.md) - Complete API documentation
- [Configuration Guide](CONFIGURATION.md) - Configuration management
- [Troubleshooting](TROUBLESHOOTING.md) - Common issues and solutions
