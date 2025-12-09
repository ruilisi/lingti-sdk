# 示例

[English](EXAMPLES.md) | [简体中文](EXAMPLES.zh-CN.md)

Lingti SDK Node.js 扩展的实用示例和用例。

## 目录

- [基础用法](#基础用法)
- [Electron 应用程序](#electron-应用程序)
- [Express 服务器集成](#express-服务器集成)
- [游戏启动器](#游戏启动器)
- [流量监控仪表板](#流量监控仪表板)
- [多用户管理](#多用户管理)
- [错误处理](#错误处理)
- [TypeScript 示例](#typescript-示例)

## 基础用法

### 简单的启动和停止

```javascript
const lingti = require('lingti-sdk');

// 检查平台支持
if (!lingti.isAddonAvailable()) {
    console.error('此平台不受支持');
    process.exit(1);
}

console.log('SDK 版本:', lingti.getSDKVersion());

// 启动隧道
const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
if (result === 0) {
    console.log('隧道启动成功');
} else {
    console.error('启动失败:', lingti.getLastErrorMessage());
    process.exit(1);
}

// 监控 60 秒
setTimeout(() => {
    // 获取统计信息
    const stats = lingti.getTrafficStats();
    console.log('流量统计:');
    console.log('  发送:', stats.txBytes, '字节');
    console.log('  接收:', stats.rxBytes, '字节');

    // 停止隧道
    lingti.stopTun2R();
    console.log('隧道已停止');
}, 60000);
```

### 带 Ping 监控

```javascript
const lingti = require('lingti-sdk');

// 启动服务
lingti.startTun2RWithConfigFile();

// 启动 ping 监控
lingti.runPing();

// 每 5 秒显示统计信息
const interval = setInterval(() => {
    const ping = lingti.getLastPingStats();
    const stats = lingti.getTrafficStats();

    console.clear();
    console.log('='.repeat(50));
    console.log('LINGTI 隧道状态');
    console.log('='.repeat(50));
    console.log(`路由器 Ping:  ${ping.router}ms`);
    console.log(`起飞 Ping:   ${ping.takeoff}ms`);
    console.log(`着陆 Ping:   ${ping.landing}ms`);
    console.log(`发送: ${formatBytes(stats.txBytes)}`);
    console.log(`接收: ${formatBytes(stats.rxBytes)}`);
}, 5000);

// 退出时清理
process.on('SIGINT', () => {
    clearInterval(interval);
    lingti.stopPing();
    lingti.stopTun2R();
    console.log('\n关闭完成');
    process.exit(0);
});

function formatBytes(bytes) {
    if (bytes < 1024) return bytes + ' B';
    if (bytes < 1024 ** 2) return (bytes / 1024).toFixed(2) + ' KB';
    if (bytes < 1024 ** 3) return (bytes / (1024 ** 2)).toFixed(2) + ' MB';
    return (bytes / (1024 ** 3)).toFixed(2) + ' GB';
}
```

## Electron 应用程序

完整的 Electron 集成示例请参见[英文版](EXAMPLES.md#electron-application)，包括：
- 主进程集成
- 预加载脚本
- 渲染进程通信
- IPC 处理

## Express 服务器集成

### RESTful API 服务器

```javascript
const express = require('express');
const lingti = require('lingti-sdk');
const app = express();

app.use(express.json());

// 启动隧道端点
app.post('/api/tunnel/start', (req, res) => {
    const { configPath } = req.body;

    if (!lingti.isAddonAvailable()) {
        return res.status(400).json({
            success: false,
            error: '平台不受支持'
        });
    }

    const result = lingti.startTun2RWithConfigFile(configPath || 'encrypted_config.txt');

    if (result === 0) {
        lingti.runPing();
        res.json({
            success: true,
            message: '隧道启动成功',
            version: lingti.getSDKVersion()
        });
    } else {
        res.status(500).json({
            success: false,
            error: lingti.getLastErrorMessage()
        });
    }
});

// 停止隧道端点
app.post('/api/tunnel/stop', (req, res) => {
    lingti.stopPing();
    const result = lingti.stopTun2R();

    res.json({
        success: result === 0,
        message: result === 0 ? '隧道已停止' : '停止隧道失败'
    });
});

// 获取隧道状态
app.get('/api/tunnel/status', (req, res) => {
    const running = lingti.isServiceRunning();

    if (running) {
        const stats = lingti.getTrafficStats();
        const ping = lingti.getLastPingStats();

        res.json({
            running: true,
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

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
    console.log(`Lingti API 服务器运行在端口 ${PORT}`);
});
```

## 游戏启动器

### 自动游戏启动与隧道

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
        console.log('启动隧道...');

        const result = lingti.startTun2RWithConfigFile(this.configPath);
        if (result !== 0) {
            throw new Error(`启动隧道失败: ${lingti.getLastErrorMessage()}`);
        }

        // 等待隧道建立
        await new Promise(resolve => setTimeout(resolve, 2000));

        // 验证隧道正在运行
        if (!lingti.isServiceRunning()) {
            throw new Error('隧道启动失败');
        }

        console.log('隧道启动成功');
    }

    launchGame() {
        console.log('启动游戏:', this.gameExePath);

        this.gameProcess = spawn(this.gameExePath, [], {
            cwd: path.dirname(this.gameExePath),
            detached: false
        });

        this.gameProcess.on('close', (code) => {
            console.log(`游戏退出，代码 ${code}`);
            this.cleanup();
        });

        this.gameProcess.on('error', (err) => {
            console.error('启动游戏失败:', err);
            this.cleanup();
        });
    }

    cleanup() {
        console.log('清理...');
        lingti.stopTun2R();
        this.gameProcess = null;
    }

    async run() {
        try {
            await this.startTunnel();
            this.launchGame();
        } catch (error) {
            console.error('错误:', error.message);
            process.exit(1);
        }
    }
}

// 使用
const launcher = new GameLauncher(
    'C:\\Program Files\\Game\\game.exe',
    'encrypted_config.txt'
);

launcher.run();
```

## 流量监控仪表板

使用 `blessed` 库创建实时控制台仪表板的完整示例请参见[英文版](EXAMPLES.md#traffic-monitoring-dashboard)。

## 多用户管理

### 管理多个配置

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
        console.log(`已为用户 ${userId} 保存配置`);
    }

    getUserConfigPath(userId) {
        return path.join(this.configDir, `${userId}.config`);
    }

    async startForUser(userId) {
        if (lingti.isServiceRunning()) {
            throw new Error('另一个隧道已在运行');
        }

        const configPath = this.getUserConfigPath(userId);
        if (!fs.existsSync(configPath)) {
            throw new Error(`未找到用户 ${userId} 的配置`);
        }

        const result = lingti.startTun2RWithConfigFile(configPath);
        if (result !== 0) {
            throw new Error(lingti.getLastErrorMessage());
        }

        this.activeUser = userId;
        console.log(`已为用户 ${userId} 启动隧道`);
    }

    stop() {
        if (!lingti.isServiceRunning()) {
            return;
        }

        lingti.stopTun2R();
        console.log(`已为用户 ${this.activeUser} 停止隧道`);
        this.activeUser = null;
    }

    getActiveUser() {
        return this.activeUser;
    }
}

// 使用
const manager = new TunnelManager();

// 为不同用户保存配置
manager.saveUserConfig('user1', '加密配置_1...');
manager.saveUserConfig('user2', '加密配置_2...');

// 为 user1 启动
manager.startForUser('user1')
    .then(() => {
        console.log('活动用户:', manager.getActiveUser());

        // 30 秒后切换到 user2
        setTimeout(() => {
            manager.stop();
            manager.startForUser('user2');
        }, 30000);
    })
    .catch(error => console.error('错误:', error));
```

## 错误处理

### 全面的错误处理

```javascript
const lingti = require('lingti-sdk');

class TunnelService {
    constructor(configPath) {
        this.configPath = configPath;
        this.retryCount = 0;
        this.maxRetries = 3;
    }

    async start() {
        // 平台检查
        if (!lingti.isAddonAvailable()) {
            throw new Error(`平台 ${lingti.getPlatform()} 不受支持`);
        }

        // 检查是否已在运行
        if (lingti.isServiceRunning()) {
            console.log('隧道已在运行');
            return;
        }

        // 尝试启动并重试
        while (this.retryCount < this.maxRetries) {
            try {
                const result = lingti.startTun2RWithConfigFile(this.configPath);

                if (result === lingti.ErrorCodes.SUCCESS) {
                    console.log('隧道启动成功');
                    this.retryCount = 0;
                    return;
                }

                // 处理特定错误
                this.handleError(result);

            } catch (error) {
                console.error('意外错误:', error);
                this.retryCount++;

                if (this.retryCount < this.maxRetries) {
                    console.log(`重试中... (${this.retryCount}/${this.maxRetries})`);
                    await this.delay(2000);
                } else {
                    throw new Error(`在 ${this.maxRetries} 次尝试后失败`);
                }
            }
        }
    }

    handleError(errorCode) {
        const errorMsg = lingti.getLastErrorMessage();

        switch (errorCode) {
            case lingti.ErrorCodes.ERR_NULL_CONFIG:
                throw new Error('配置无效: ' + errorMsg);

            case lingti.ErrorCodes.ERR_LOAD_CONFIG:
                throw new Error(`未找到配置文件: ${this.configPath}`);

            case lingti.ErrorCodes.ERR_JSON_PARSE:
                throw new Error('配置解析失败: ' + errorMsg);

            case lingti.ErrorCodes.ERR_ALREADY_RUN:
                console.log('服务已在运行');
                return;

            default:
                throw new Error(`未知错误 (${errorCode}): ${errorMsg}`);
        }
    }

    delay(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    stop() {
        try {
            lingti.stopTun2R();
            console.log('隧道已停止');
        } catch (error) {
            console.error('停止隧道错误:', error);
        }
    }
}

// 使用
const service = new TunnelService('encrypted_config.txt');

service.start()
    .then(() => console.log('运行中'))
    .catch(error => console.error('启动失败:', error));
```

## TypeScript 示例

### 类型安全实现

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
            throw new Error('平台不受支持');
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

// 使用
const service = new TypedTunnelService('encrypted_config.txt');

try {
    service.start();

    service.startMonitoring((stats, ping) => {
        console.log('发送:', stats.txBytes, '接收:', stats.rxBytes);
        console.log('Ping:', ping.router, 'ms');
    });

    setTimeout(() => service.stop(), 60000);
} catch (error) {
    console.error('错误:', error);
}
```

## 另请参阅

- [API 参考](API.zh-CN.md) - 完整的 API 文档
- [配置指南](CONFIGURATION.zh-CN.md) - 配置管理
- [故障排除](TROUBLESHOOTING.zh-CN.md) - 常见问题和解决方案
