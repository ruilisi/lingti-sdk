# Lingti SDK Node.js 扩展 - API 参考

[English](API.md) | [简体中文](API.zh-CN.md)

Lingti SDK Node.js 原生扩展的完整 API 参考文档。

## 目录

- [平台检测](#平台检测)
- [服务管理](#服务管理)
- [信息与监控](#信息与监控)
- [网络统计](#网络统计)
- [Ping 操作](#ping-操作)
- [实用函数](#实用函数)
- [常量](#常量)
- [错误处理](#错误处理)

## 平台检测

### `isAddonAvailable()`

检查当前平台是否支持原生扩展。

**返回值：** `boolean`
- `true` - 原生扩展可用（Windows）
- `false` - 原生扩展不可用（macOS/Linux）

**示例：**
```javascript
const lingti = require('lingti-sdk');

if (!lingti.isAddonAvailable()) {
    console.error('此平台不受支持');
    process.exit(1);
}
```

**使用场景：** 在使用任何其他 SDK 函数之前始终调用此函数以确保平台兼容性。

---

### `getPlatform()`

获取当前平台名称。

**返回值：** `string`
- `"win32"` - Windows
- `"darwin"` - macOS
- `"linux"` - Linux
- Node.js `process.platform` 返回的其他平台名称

**示例：**
```javascript
console.log('运行平台:', lingti.getPlatform());
```

---

## 服务管理

### `startTun2R(encryptedConfig)`

使用加密配置启动 TUN2R 隧道服务。

**参数：**
- `encryptedConfig` (string) - Base64 编码的加密配置文本

**返回值：** `number`
- `0` - 成功
- `-1` - 无效/空配置 (ERR_NULL_CONFIG)
- `-2` - JSON 解析错误 (ERR_JSON_PARSE)
- `-3` - 服务已在运行 (ERR_ALREADY_RUN)

**示例：**
```javascript
// 从后端服务获取的加密配置文本
const encryptedConfig = "SGVsbG8gV29ybGQhIFRoaXMgaXMgYmFzZTY0...";

const result = lingti.startTun2R(encryptedConfig);
if (result === 0) {
    console.log('服务启动成功');
} else {
    console.error('启动失败:', lingti.getLastErrorMessage());
}
```

**注意：**
- 配置必须是 base64 编码的加密文本
- 服务在后台异步运行
- 调用 `isServiceRunning()` 验证服务状态
- 一次只能运行一个服务实例

---

### `startTun2RWithConfigFile([configPath])`

使用加密配置文件启动 TUN2R 服务。

**参数：**
- `configPath` (string, 可选) - 加密配置文件路径
  - 默认值：当前目录下的 `"encrypted_config.txt"`
  - 支持相对路径和绝对路径

**返回值：** `number`
- `0` - 成功
- `-1` - 无效/空配置 (ERR_NULL_CONFIG)
- `-4` - 加载配置文件失败 (ERR_LOAD_CONFIG)
- `-3` - 服务已在运行 (ERR_ALREADY_RUN)

**示例：**
```javascript
// 使用默认配置文件
let result = lingti.startTun2RWithConfigFile();

// 使用自定义配置文件路径
result = lingti.startTun2RWithConfigFile('./config/encrypted_config.txt');

// 使用绝对路径
result = lingti.startTun2RWithConfigFile('C:\\Users\\Game\\config\\encrypted_config.txt');
```

**注意：**
- 配置文件必须包含 base64 编码的加密配置
- 文件在启动时同步读取
- 如果文件不存在或无法读取，返回 ERR_LOAD_CONFIG

---

### `stopTun2R()`

优雅地停止 TUN2R 服务。

**返回值：** `number`
- `0` - 成功
- `-1` - 服务未运行 (ERR_NOT_RUNNING)

**示例：**
```javascript
const result = lingti.stopTun2R();
if (result === 0) {
    console.log('服务停止成功');
} else {
    console.log('服务未在运行');
}
```

**注意：**
- 优雅关闭 - 等待进行中的操作完成
- 即使服务未运行也可以安全调用
- 如果 ping 监控处于活动状态，会自动停止

---

### `isServiceRunning()`

检查 TUN2R 服务是否正在运行。

**返回值：** `boolean`
- `true` - 服务正在运行
- `false` - 服务未运行

**示例：**
```javascript
if (lingti.isServiceRunning()) {
    console.log('服务正在运行');

    // 获取统计信息
    const stats = lingti.getTrafficStats();
    console.log('流量:', stats);
} else {
    console.log('服务已停止');
}
```

**使用场景：**
- 在启动/停止服务之前检查状态
- 定期健康检查
- 防止重复的服务实例

---

## 信息与监控

### `getSDKVersion()`

获取 SDK 版本字符串。

**返回值：** `string` - 语义化版本格式（例如 "1.4.3"）

**示例：**
```javascript
const version = lingti.getSDKVersion();
console.log('SDK 版本:', version);

// 版本比较
const [major, minor, patch] = version.split('.').map(Number);
if (major >= 1 && minor >= 4) {
    console.log('使用受支持的 SDK 版本');
}
```

---

### `getLastErrorMessage()`

获取 SDK 的最后一次错误消息。

**返回值：** `string` - 人类可读的错误消息

**示例：**
```javascript
const result = lingti.startTun2R(invalidConfig);
if (result !== 0) {
    const error = lingti.getLastErrorMessage();
    console.error('错误:', error);

    // 记录到文件或发送到监控服务
    logError({ code: result, message: error });
}
```

**注意：**
- 消息为英文
- 提供有关错误的详细上下文
- 每次 SDK 操作后更新

---

## 网络统计

### `getTrafficStats()`

获取隧道的当前流量统计。

**返回值：** `object`
```javascript
{
    txBytes: number,  // 发送字节数
    rxBytes: number,  // 接收字节数
    txPkts: number,   // 发送数据包数
    rxPkts: number    // 接收数据包数
}
```

**示例：**
```javascript
const stats = lingti.getTrafficStats();

console.log(`发送: ${formatBytes(stats.txBytes)} (${stats.txPkts} 个数据包)`);
console.log(`接收: ${formatBytes(stats.rxBytes)} (${stats.rxPkts} 个数据包)`);

// 计算吞吐量
function formatBytes(bytes) {
    if (bytes < 1024) return bytes + ' B';
    if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(2) + ' KB';
    if (bytes < 1024 * 1024 * 1024) return (bytes / (1024 * 1024)).toFixed(2) + ' MB';
    return (bytes / (1024 * 1024 * 1024)).toFixed(2) + ' GB';
}
```

**使用场景：**
- 实时带宽监控
- 数据使用跟踪
- 性能指标
- 计费计算

**注意：**
- 计数器从服务启动开始累积
- 服务停止时重置为零
- 实时更新

---

### `getLastPingStats()`

获取到各个网络节点的最新 ping 统计信息。

**返回值：** `object`
```javascript
{
    router: number,   // 到路由器的 ping (ms)
    takeoff: number,  // 到起飞服务器的 ping (ms)
    landing: number   // 到着陆服务器的 ping (ms)
}
```

**示例：**
```javascript
// 必须先调用 runPing()
lingti.runPing();

// 等待片刻获取第一次 ping
setTimeout(() => {
    const ping = lingti.getLastPingStats();

    console.log('网络延迟:');
    console.log(`  路由器:  ${ping.router}ms`);
    console.log(`  起飞:   ${ping.takeoff}ms`);
    console.log(`  着陆:   ${ping.landing}ms`);

    // 检查连接质量
    if (ping.router > 100) {
        console.warn('到路由器的延迟过高');
    }
}, 1000);
```

**注意：**
- 需要先调用 `runPing()`
- 返回最后记录的值（非实时）
- 如果 ping 失败或尚不可用，值为 `-1`

---

## Ping 操作

### `runPing()`

启动到网络节点的周期性 ping 监控。

**返回值：** `number`
- `0` - 成功
- 负值 - 错误码

**示例：**
```javascript
// 启动 ping 监控
const result = lingti.runPing();
if (result === 0) {
    // 每 5 秒监控一次 ping
    const interval = setInterval(() => {
        const ping = lingti.getLastPingStats();
        updateUI(ping);
    }, 5000);

    // 60 秒后停止监控
    setTimeout(() => {
        clearInterval(interval);
        lingti.stopPing();
    }, 60000);
}
```

**注意：**
- Ping 在后台周期性发送
- 不会阻塞执行
- 多次调用 `runPing()` 是安全的（不会重复监控）

---

### `stopPing()`

停止周期性 ping 监控。

**返回值：** `number`
- `0` - 成功
- 负值 - 错误码

**示例：**
```javascript
lingti.stopPing();
console.log('Ping 监控已停止');
```

**注意：**
- 即使 ping 监控未激活也可以安全调用
- 最后的 ping 值通过 `getLastPingStats()` 仍然可用

---

## 实用函数

### `flushDNSCache()`

刷新 DNS 缓存。

**返回值：** `number`
- `0` - 成功
- 负值 - 错误码

**示例：**
```javascript
// 更改网络后刷新 DNS
lingti.flushDNSCache();
console.log('DNS 缓存已刷新');
```

**使用场景：**
- 网络更改后
- 发生 DNS 解析问题时
- 启动隧道服务之前

---

### `getConsoleConfig()`

获取控制台网络配置参数。

**返回值：** `object`
```javascript
{
    state: number,      // IP 状态码 (0-3)
    stateStr: string,   // 状态描述
    gateway: string,    // 网关 IP 地址
    mask: string,       // 子网掩码
    ip: string,         // 控制台 IP 地址
    dns: string         // DNS 服务器地址
}
```

**示例：**
```javascript
const config = lingti.getConsoleConfig();

console.log('网络配置:');
console.log(`  状态: ${config.stateStr}`);
console.log(`  IP: ${config.ip}`);
console.log(`  网关: ${config.gateway}`);
console.log(`  DNS: ${config.dns}`);
console.log(`  掩码: ${config.mask}`);

// 检查 IP 分配是否完成
if (config.state === lingti.ConsoleIPState.COMPLETED) {
    console.log('网络配置已就绪');
}
```

**状态值：**
- `0` (COMPLETED) - IP 分配成功
- `1` (FAILED) - IP 分配失败
- `2` (IDLE) - 未启动
- `3` (IN_PROGRESS) - IP 分配进行中

---

## 常量

### `ErrorCodes`

SDK 操作的错误码常量。

**属性：**
```javascript
{
    SUCCESS: 0,           // 操作成功
    ERR_NULL_CONFIG: -1,  // 无效或空配置
    ERR_JSON_PARSE: -2,   // JSON 解析错误
    ERR_ALREADY_RUN: -3,  // 服务已在运行
    ERR_LOAD_CONFIG: -4,  // 加载配置文件失败
    ERR_NOT_RUNNING: -1   // 服务未运行
}
```

**示例：**
```javascript
const result = lingti.startTun2R(config);

switch(result) {
    case lingti.ErrorCodes.SUCCESS:
        console.log('启动成功');
        break;
    case lingti.ErrorCodes.ERR_ALREADY_RUN:
        console.log('服务已在运行');
        break;
    case lingti.ErrorCodes.ERR_NULL_CONFIG:
        console.error('配置无效');
        break;
    default:
        console.error('未知错误:', result);
}
```

---

### `ConsoleIPState`

控制台 IP 分配状态常量。

**属性：**
```javascript
{
    COMPLETED: 0,     // IP 分配成功
    FAILED: 1,        // IP 分配失败
    IDLE: 2,          // 未启动
    IN_PROGRESS: 3    // IP 分配进行中
}
```

**示例：**
```javascript
const config = lingti.getConsoleConfig();

switch(config.state) {
    case lingti.ConsoleIPState.COMPLETED:
        console.log('准备就绪');
        break;
    case lingti.ConsoleIPState.IN_PROGRESS:
        console.log('正在配置网络...');
        break;
    case lingti.ConsoleIPState.FAILED:
        console.error('网络配置失败');
        break;
    case lingti.ConsoleIPState.IDLE:
        console.log('尚未配置');
        break;
}
```

---

## 错误处理

### 最佳实践

**始终检查返回码：**
```javascript
const result = lingti.startTun2R(config);
if (result !== 0) {
    const error = lingti.getLastErrorMessage();
    throw new Error(`SDK 错误 ${result}: ${error}`);
}
```

**优雅降级：**
```javascript
try {
    if (!lingti.isAddonAvailable()) {
        // 回退到替代实现
        useAlternativeSDK();
    } else {
        lingti.startTun2RWithConfigFile();
    }
} catch (error) {
    console.error('启动隧道失败:', error);
    process.exit(1);
}
```

**正确清理：**
```javascript
process.on('SIGINT', () => {
    console.log('正在关闭...');
    lingti.stopPing();
    lingti.stopTun2R();
    process.exit(0);
});
```

---

## 完整示例

```javascript
const lingti = require('lingti-sdk');

// 检查平台
if (!lingti.isAddonAvailable()) {
    console.error('平台不受支持:', lingti.getPlatform());
    process.exit(1);
}

// 显示版本
console.log('SDK 版本:', lingti.getSDKVersion());

// 启动服务
const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
if (result !== 0) {
    console.error('启动失败:', lingti.getLastErrorMessage());
    process.exit(1);
}

// 启动 ping 监控
lingti.runPing();

// 监控流量
const monitor = setInterval(() => {
    if (!lingti.isServiceRunning()) {
        clearInterval(monitor);
        return;
    }

    const stats = lingti.getTrafficStats();
    const ping = lingti.getLastPingStats();
    const config = lingti.getConsoleConfig();

    console.log('='.repeat(50));
    console.log('流量 - 发送:', stats.txBytes, '接收:', stats.rxBytes);
    console.log('Ping - 路由器:', ping.router, 'ms');
    console.log('网络状态:', config.stateStr);
}, 5000);

// 退出时清理
process.on('SIGINT', () => {
    clearInterval(monitor);
    lingti.stopPing();
    lingti.stopTun2R();
    console.log('服务已停止');
    process.exit(0);
});
```

---

## 另请参阅

- [配置指南](CONFIGURATION.zh-CN.md) - 如何配置加密配置文件
- [示例](EXAMPLES.zh-CN.md) - 更多使用示例
- [故障排除](TROUBLESHOOTING.zh-CN.md) - 常见问题和解决方案
