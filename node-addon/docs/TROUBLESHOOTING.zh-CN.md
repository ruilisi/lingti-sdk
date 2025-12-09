# 故障排除指南

[English](TROUBLESHOOTING.md) | [简体中文](TROUBLESHOOTING.zh-CN.md)

Lingti SDK Node.js 扩展的常见问题和解决方案。

## 目录

- [平台问题](#平台问题)
- [安装问题](#安装问题)
- [配置错误](#配置错误)
- [连接问题](#连接问题)
- [性能问题](#性能问题)
- [错误码](#错误码)
- [调试模式](#调试模式)
- [获取帮助](#获取帮助)

## 平台问题

### 错误：平台不受支持

**问题：**
```javascript
if (!lingti.isAddonAvailable()) {
    // 返回 false
}
```

**原因：** 原生扩展仅在 Windows 上工作。不支持 macOS 和 Linux。

**解决方案：**
```javascript
const lingti = require('lingti-sdk');

if (!lingti.isAddonAvailable()) {
    console.error(`平台 ${lingti.getPlatform()} 不受支持`);
    console.error('此扩展需要 Windows 才能运行');
    process.exit(1);
}
```

**开发环境的解决方法：**
- 使用 Windows 虚拟机
- 在 Windows 机器上测试
- 使用带有 Windows 二进制文件的 Windows Subsystem for Linux (WSL)

---

### 模块未自注册

**问题：**
```
Error: Module did not self-register
```

**原因：**
1. Node.js 版本不匹配
2. node_modules 损坏
3. 架构错误（x86 vs x64）

**解决方案：**

1. **重新构建扩展：**
```bash
cd node_modules/lingti-sdk
npm run rebuild
```

2. **重新安装包：**
```bash
npm uninstall lingti-sdk
npm cache clean --force
npm install lingti-sdk
```

3. **检查 Node.js 版本：**
```bash
node --version  # 应该 >= 16.0.0
```

4. **确保架构正确：**
```bash
node -p "process.arch"  # 应与您的操作系统匹配（64 位 Windows 为 x64）
```

---

## 安装问题

### Windows 上 npm install 失败

**问题：**
```
npm ERR! gyp ERR! build error
npm ERR! gyp ERR! stack Error: `msbuild.exe` failed with exit code: 1
```

**原因：**
- 缺少 Visual Studio 构建工具
- 缺少 Python
- Python 版本错误

**解决方案：**

1. **安装 Visual Studio 构建工具：**
```bash
# 下载地址：https://visualstudio.microsoft.com/downloads/
# 安装 "使用 C++ 的桌面开发"
```

2. **安装 Python 3.x：**
```bash
# 下载地址：https://www.python.org/downloads/
# 确保勾选 "Add Python to PATH"
```

3. **配置 npm 使用正确的工具：**
```bash
npm config set msvs_version 2019
npm config set python "C:\Python39\python.exe"
```

4. **再次尝试安装：**
```bash
npm install lingti-sdk
```

---

## 配置错误

### ERR_LOAD_CONFIG (-4)：加载配置文件失败

**问题：**
```javascript
const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
// 返回 -4
```

**原因：**
1. 文件不存在
2. 文件路径错误
3. 无读取权限

**解决方案：**

1. **检查文件是否存在：**
```javascript
const fs = require('fs');
const configPath = 'encrypted_config.txt';

if (!fs.existsSync(configPath)) {
    console.error('未找到配置文件:', configPath);
    console.log('当前目录:', process.cwd());
}
```

2. **使用绝对路径：**
```javascript
const path = require('path');
const configPath = path.join(__dirname, 'encrypted_config.txt');
lingti.startTun2RWithConfigFile(configPath);
```

3. **检查文件权限：**
```javascript
try {
    fs.accessSync(configPath, fs.constants.R_OK);
    console.log('文件可读');
} catch (err) {
    console.error('无法读取文件:', err);
}
```

---

### ERR_NULL_CONFIG (-1)：配置无效

**问题：**
```javascript
const result = lingti.startTun2R(config);
// 返回 -1
```

**原因：**
1. 配置为空
2. Base64 编码无效
3. 配置损坏

**解决方案：**

1. **验证配置格式：**
```javascript
function validateConfig(config) {
    if (!config || typeof config !== 'string') {
        return false;
    }

    // 检查是否为有效的 Base64
    const base64Regex = /^[A-Za-z0-9+/=]+$/;
    if (!base64Regex.test(config.trim())) {
        console.error('Base64 格式无效');
        return false;
    }

    // 检查最小长度
    if (config.length < 100) {
        console.error('配置太短');
        return false;
    }

    return true;
}

if (validateConfig(encryptedConfig)) {
    lingti.startTun2R(encryptedConfig);
} else {
    console.error('配置无效');
}
```

2. **从后端重新获取配置：**
```javascript
const newConfig = await fetchConfigFromBackend();
fs.writeFileSync('encrypted_config.txt', newConfig);
```

---

## 连接问题

### ERR_ALREADY_RUN (-3)：服务已在运行

**问题：**
```javascript
lingti.startTun2RWithConfigFile();
// 返回 -3：服务已在运行
```

**解决方案：**

1. **检查服务是否正在运行：**
```javascript
if (lingti.isServiceRunning()) {
    console.log('服务已在运行');
    // 使用现有服务或先停止它
    lingti.stopTun2R();
}
```

2. **实现单例模式：**
```javascript
class TunnelService {
    static start(configPath) {
        if (lingti.isServiceRunning()) {
            console.log('使用现有隧道');
            return 0;
        }

        return lingti.startTun2RWithConfigFile(configPath);
    }
}
```

---

### 隧道启动但无流量

**问题：**
服务启动成功但没有数据传输。

**诊断：**

1. **检查服务状态：**
```javascript
console.log('服务运行:', lingti.isServiceRunning());

const stats = lingti.getTrafficStats();
console.log('发送字节:', stats.txBytes);
console.log('接收字节:', stats.rxBytes);
```

2. **检查网络配置：**
```javascript
const config = lingti.getConsoleConfig();
console.log('IP 状态:', config.stateStr);
console.log('IP 地址:', config.ip);
console.log('网关:', config.gateway);

if (config.state !== lingti.ConsoleIPState.COMPLETED) {
    console.error('IP 分配未完成');
}
```

3. **测试连接性：**
```javascript
lingti.runPing();

setTimeout(() => {
    const ping = lingti.getLastPingStats();
    console.log('路由器 ping:', ping.router, 'ms');
    console.log('起飞 ping:', ping.takeoff, 'ms');

    if (ping.router < 0) {
        console.error('无法连接到路由器');
    }
}, 2000);
```

**解决方案：**

1. 验证防火墙设置允许应用程序
2. 检查游戏可执行文件是否正确配置
3. 确保 Windows 驱动程序 (lingtiwfp64.sys) 在正确位置
4. 尝试重启服务
5. 使用 `lingti.flushDNSCache()` 检查 DNS 设置

---

### 高延迟或丢包

**问题：**
Ping 值高或不稳定。

**诊断：**
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

    // 检查丢包
    const ratio = stats.rxPkts / stats.txPkts;
    if (ratio < 0.9) {
        console.warn('检测到可能的丢包');
    }

    // 检查高延迟
    if (ping.router > 100) {
        console.warn('路由器延迟高');
    }
}, 5000);
```

**解决方案：**

1. **切换到不同的路由线路：**
```javascript
// 使用不同线路获取新配置
const newConfig = await getConfigWithDifferentLine(gameId, differentLineId);
lingti.stopTun2R();
lingti.startTun2R(newConfig);
```

2. **检查本地网络：**
- 测试本地网络速度
- 检查其他占用带宽的应用程序
- 尝试有线连接而不是 WiFi

3. **刷新 DNS 缓存：**
```javascript
lingti.flushDNSCache();
```

---

## 性能问题

### CPU 使用率高

**问题：**
Node.js 进程消耗过多 CPU。

**原因：**
1. 轮询频率太高
2. 太多并发监控间隔
3. 内存泄漏

**解决方案：**

1. **降低轮询频率：**
```javascript
// 错误：每 100ms 轮询一次
setInterval(getStats, 100);

// 正确：每 1-2 秒轮询一次
setInterval(getStats, 1000);
```

2. **正确清除间隔：**
```javascript
let monitorInterval;

function startMonitoring() {
    // 先停止现有间隔
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

## 错误码

### 完整错误码参考

| 代码 | 常量 | 描述 | 解决方案 |
|------|------|------|----------|
| 0 | SUCCESS | 操作成功 | - |
| -1 | ERR_NULL_CONFIG | 无效/空配置 | 检查配置格式 |
| -2 | ERR_JSON_PARSE | JSON 解析错误 | 验证配置编码 |
| -3 | ERR_ALREADY_RUN | 服务已在运行 | 先停止现有服务 |
| -4 | ERR_LOAD_CONFIG | 加载配置文件失败 | 检查文件路径和权限 |
| -1 | ERR_NOT_RUNNING | 服务未运行 | 操作前启动服务 |

### 处理所有错误

```javascript
function handleTunnelError(errorCode) {
    const errorMsg = lingti.getLastErrorMessage();

    switch (errorCode) {
        case lingti.ErrorCodes.SUCCESS:
            return '成功';

        case lingti.ErrorCodes.ERR_NULL_CONFIG:
            return `配置无效: ${errorMsg}`;

        case lingti.ErrorCodes.ERR_JSON_PARSE:
            return `配置解析失败: ${errorMsg}`;

        case lingti.ErrorCodes.ERR_ALREADY_RUN:
            return '隧道已在运行';

        case lingti.ErrorCodes.ERR_LOAD_CONFIG:
            return `无法加载配置文件: ${errorMsg}`;

        case lingti.ErrorCodes.ERR_NOT_RUNNING:
            return '隧道未运行';

        default:
            return `未知错误 (${errorCode}): ${errorMsg}`;
    }
}

// 使用
const result = lingti.startTun2RWithConfigFile();
if (result !== 0) {
    console.error(handleTunnelError(result));
}
```

---

## 调试模式

### 启用详细日志

```javascript
const lingti = require('lingti-sdk');

// 记录所有操作
function debugLog(operation, ...args) {
    console.log(`[DEBUG] ${new Date().toISOString()} - ${operation}`, ...args);
}

debugLog('平台检查', {
    available: lingti.isAddonAvailable(),
    platform: lingti.getPlatform(),
    version: lingti.getSDKVersion()
});

const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
debugLog('启动结果', result);

if (result !== 0) {
    debugLog('错误', lingti.getLastErrorMessage());
}

// 监控所有统计信息
setInterval(() => {
    const stats = lingti.getTrafficStats();
    const ping = lingti.getLastPingStats();
    const config = lingti.getConsoleConfig();

    debugLog('统计', {
        running: lingti.isServiceRunning(),
        traffic: stats,
        ping: ping,
        network: config
    });
}, 5000);
```

### 日志到文件

```javascript
const fs = require('fs');

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
    logger.log('INFO', '隧道启动结果', { code: result });

    if (result !== 0) {
        logger.log('ERROR', '启动失败', {
            code: result,
            message: lingti.getLastErrorMessage()
        });
    }
} catch (error) {
    logger.log('ERROR', '发生异常', {
        error: error.message,
        stack: error.stack
    });
}

process.on('exit', () => logger.close());
```

---

## 获取帮助

### 寻求帮助前

收集以下信息：

```javascript
const diagnostics = {
    // 系统信息
    platform: process.platform,
    arch: process.arch,
    nodeVersion: process.version,

    // 扩展信息
    addonAvailable: lingti.isAddonAvailable(),
    sdkVersion: lingti.getSDKVersion(),

    // 服务状态
    serviceRunning: lingti.isServiceRunning(),

    // 如果服务正在运行
    stats: lingti.isServiceRunning() ? lingti.getTrafficStats() : null,
    ping: lingti.isServiceRunning() ? lingti.getLastPingStats() : null,
    config: lingti.isServiceRunning() ? lingti.getConsoleConfig() : null,

    // 最后一次错误
    lastError: lingti.getLastErrorMessage()
};

console.log(JSON.stringify(diagnostics, null, 2));
```

### 支持渠道

1. **GitHub Issues**: https://github.com/ruilisi/lingti-sdk/issues
2. **社区论坛**: https://xiemala.com/f/rY1aZz
3. **电子邮件支持**: 联系您的 Lingti 代表

### 在错误报告中包含

1. 完整的错误消息和堆栈跟踪
2. 系统诊断信息（如上）
3. 重现问题的步骤
4. 预期与实际行为
5. 配置（不包含敏感数据）
6. SDK 版本和 Node.js 版本

---

## 另请参阅

- [API 参考](API.zh-CN.md) - 完整的 API 文档
- [配置指南](CONFIGURATION.zh-CN.md) - 配置管理
- [示例](EXAMPLES.zh-CN.md) - 代码示例和用例
