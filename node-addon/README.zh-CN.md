# Lingti SDK - Node.js 原生扩展

[English](README.md) | 简体中文

Lingti SDK 的 Node.js 原生扩展（N-API），为游戏流量路由提供网络隧道功能。

此扩展位于 lingti-core 项目的 `node-addon` 目录中，并链接到 `../dist/lingti_sdk/` 中的 SDK DLL。

## 前置要求

- **Node.js** >= 16.0.0
- **运行时要求**：Windows 操作系统（SDK DLL 仅支持 Windows）
- **构建要求**（仅 Windows）：
  - Visual Studio 2019 或更高版本，包含 C++ 构建工具
  - Python 3.x
  - node-gyp（作为依赖项自动安装）

### 安装构建工具

```bash
# 安装 Visual Studio 构建工具
# 下载地址：https://visualstudio.microsoft.com/downloads/

# 全局安装 node-gyp
npm install -g node-gyp
```

## 安装

```bash
# 进入 node-addon 目录
cd node-addon

# 安装依赖
npm install

# 构建原生扩展
npm run build
```

## 使用方法

### 基础示例

```javascript
const lingti = require('lingti-sdk');

// 首先检查平台兼容性
if (!lingti.isAddonAvailable()) {
    console.log('平台:', lingti.getPlatform());
    console.log('此扩展需要 Windows 系统才能运行。');
    process.exit(1);
}

// 使用加密配置文件启动服务（base64 编码文本）
// 获取加密配置：访问 https://game.lingti.com/sdk
// 选择您的游戏（需要加速的游戏）和隧道线路（线路）
const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
if (result === 0) {
    console.log('服务启动成功！');
    console.log('SDK 版本:', lingti.getSDKVersion());
} else {
    console.error('启动失败:', lingti.getLastErrorMessage());
}

// 检查服务是否正在运行
if (lingti.isServiceRunning()) {
    console.log('服务正在运行');
}

// 获取流量统计
const stats = lingti.getTrafficStats();
console.log('发送:', stats.txBytes, '接收:', stats.rxBytes);

// 完成后停止服务
lingti.stopTun2R();
```

### TypeScript 支持

该扩展包含完整的 TypeScript 类型定义：

```typescript
import * as lingti from 'lingti-sdk';

// 使用加密配置文件启动服务
lingti.startTun2RWithConfigFile('encrypted_config.txt');
```

## 文档

### 完整指南

- **[API 参考](docs/API.zh-CN.md)** - 带有详细示例的完整 API 文档
- **[配置指南](docs/CONFIGURATION.zh-CN.md)** - 如何获取和使用加密配置
- **[示例](docs/EXAMPLES.zh-CN.md)** - 实用代码示例和用例
- **[故障排除](docs/TROUBLESHOOTING.zh-CN.md)** - 常见问题和解决方案

### 快速链接

- [安装](#安装)
- [使用方法](#使用方法)
- [测试](#测试)
- [平台支持](#平台支持)

## API 参考

### 核心函数

#### `startTun2R(encryptedConfig)`

使用加密配置（base64 编码文本）启动 TUN2R 服务。

- **参数：**
  - `encryptedConfig` (string)：Base64 编码的加密配置文本
- **返回值：** `number` - 成功返回 0，失败返回负错误码

```javascript
// 使用加密配置文本（base64 编码）
const encryptedConfig = "...base64 加密配置...";
const result = lingti.startTun2R(encryptedConfig);
```

#### `startTun2RWithConfigFile([configPath])`

使用加密配置文件（base64 编码文本）启动服务。

- **参数：**
  - `configPath` (string, 可选)：加密配置文件路径（默认为 'encrypted_config.txt'）
- **返回值：** `number` - 成功返回 0，失败返回负错误码

```javascript
const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');
```

#### `stopTun2R()`

优雅地停止服务。

- **返回值：** `number` - 成功返回 0，失败返回负错误码

```javascript
lingti.stopTun2R();
```

#### `isServiceRunning()`

检查服务是否正在运行。

- **返回值：** `boolean` - 运行中返回 true，否则返回 false

```javascript
const running = lingti.isServiceRunning();
```

### 信息函数

#### `getSDKVersion()`

获取 SDK 版本字符串。

- **返回值：** `string` - 版本号（例如："1.5.4"）

```javascript
const version = lingti.getSDKVersion();
```

#### `getLastErrorMessage()`

获取最后一次错误消息。

- **返回值：** `string` - 错误消息

```javascript
const error = lingti.getLastErrorMessage();
```

### 统计函数

#### `getTrafficStats()`

获取当前流量统计。

- **返回值：** `object`
  - `txBytes` (number)：发送字节数
  - `rxBytes` (number)：接收字节数
  - `txPkts` (number)：发送数据包数
  - `rxPkts` (number)：接收数据包数

```javascript
const stats = lingti.getTrafficStats();
console.log(`发送: ${stats.txBytes} 字节, 接收: ${stats.rxBytes} 字节`);
```

#### `getLastPingStats()`

获取最新的 ping 统计信息。

- **返回值：** `object`
  - `router` (number)：到路由器的 ping 值（毫秒）
  - `takeoff` (number)：到起飞服务器的 ping 值（毫秒）
  - `landing` (number)：到着陆服务器的 ping 值（毫秒）

```javascript
const ping = lingti.getLastPingStats();
console.log(`路由器: ${ping.router}ms, 起飞: ${ping.takeoff}ms`);
```

### Ping 监控

#### `runPing()`

启动周期性 ping 监控。

- **返回值：** `number` - 成功返回 0，失败返回负错误码

```javascript
lingti.runPing();
```

#### `stopPing()`

停止周期性 ping 监控。

- **返回值：** `number` - 成功返回 0，失败返回负错误码

```javascript
lingti.stopPing();
```

### 实用函数

#### `flushDNSCache()`

刷新 DNS 缓存。

- **返回值：** `number` - 成功返回 0

```javascript
lingti.flushDNSCache();
```

#### `getConsoleConfig()`

获取控制台配置参数。

- **返回值：** `object`
  - `state` (number)：IP 状态码（0-3）
  - `stateStr` (string)：状态描述
  - `gateway` (string)：网关地址
  - `mask` (string)：子网掩码
  - `ip` (string)：控制台 IP
  - `dns` (string)：DNS 服务器

```javascript
const config = lingti.getConsoleConfig();
console.log(`状态: ${config.stateStr}, IP: ${config.ip}`);
```

### 常量

#### `ErrorCodes`

错误码常量：

- `SUCCESS` (0)：操作成功
- `ERR_NULL_CONFIG` (-1)：无效/空配置
- `ERR_JSON_PARSE` (-2)：JSON 解析错误
- `ERR_ALREADY_RUN` (-3)：服务已在运行
- `ERR_LOAD_CONFIG` (-4)：加载配置文件失败
- `ERR_NOT_RUNNING` (-1)：服务未运行

```javascript
if (result === lingti.ErrorCodes.ERR_ALREADY_RUN) {
    console.log('服务已在运行');
}
```

#### `ConsoleIPState`

控制台 IP 状态常量：

- `COMPLETED` (0)：IP 分配成功
- `FAILED` (1)：IP 分配失败
- `IDLE` (2)：未启动
- `IN_PROGRESS` (3)：IP 分配进行中

```javascript
const config = lingti.getConsoleConfig();
if (config.state === lingti.ConsoleIPState.COMPLETED) {
    console.log('IP 分配完成');
}
```

## 测试

运行测试套件：

```bash
npm test
```

**在 Windows 上**：运行完整测试套件，包含所有 SDK 函数
**在 macOS/Linux 上**：显示平台信息和可用常量

参见 `example.js` 查看包含平台检测的简单使用示例。

## 构建

### 开发构建

```bash
npm run build
```

### 清理构建

```bash
npm run clean
npm run build
```

## 架构

该扩展包含：

1. **原生层** (`src/addon.cc`)：C++ N-API 绑定到 DLL
2. **JavaScript 层** (`index.js`)：高级 JavaScript 包装器
3. **TypeScript 定义** (`index.d.ts`)：TypeScript 的类型定义

## 平台支持

- **Windows**：完全支持（原生 DLL）
- **macOS/Linux**：可以安装包，但不会构建原生扩展（仅 Windows DLL）

**注意：** 该包可以在任何平台上安装，但原生扩展仅在 Windows 上构建和工作。在 macOS/Linux 上，安装将成功完成但会跳过原生构建步骤。

## 许可证

专有软件 - Copyright (c) 2025 Ruilisi

## 版本

SDK 版本：1.5.4
