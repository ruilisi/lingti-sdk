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

- **返回值：** `string` - 版本号（例如："1.6.1"）

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

#### `runPing(intervalMilliSec)`

启动周期性 ping 监控。

- **参数：**
  - `intervalMilliSec` (number)：Ping 间隔时间（毫秒）。最小值为 100ms。
- **返回值：** `number` - 成功返回 0，失败返回负错误码：
  - `-1`：服务器配置无效
  - `-2`：Ping 已在运行

```javascript
// 每 5 秒 ping 一次
lingti.runPing(5000);

// 其他示例
lingti.runPing(1000);  // 每 1 秒 ping 一次
lingti.runPing(50);    // 将使用 100ms（强制最小值）
```

#### `stopPing()`

停止周期性 ping 监控。

- **返回值：** `number` - 成功返回 0，ping 未运行时返回 `-1`

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

SDK 版本：1.6.1

---

# 底层 C SDK

此 Node.js 扩展封装了 Lingti C SDK。以下是底层 C SDK 的完整文档。

## C SDK 概述

Lingti SDK 是一个专为游戏流量优化设计的高性能网络隧道库。它提供简单的 C API，用于将网络加速功能集成到游戏和应用程序中，具有实时流量监控、智能路由和跨平台支持等特性。

## C SDK 安装

### 下载所需文件

该 SDK 是开源的,可在以下地址获取：**https://github.com/ruilisi/lingti-sdk**

**每个 SDK 版本的预编译 DLL/lib 文件可在 [GitHub Releases](https://github.com/ruilisi/lingti-sdk/releases) 部分获取。**

每个发布版本包含：
- `lingti_sdk.dll` - 主 SDK 库（13MB）
- `lingti_sdk.lib` - 用于链接的导入库（8.6KB）
- `lingti_sdk.h` - 包含 API 声明的 C 头文件
- `lingti_sdk.def` - 模块定义文件

**注意：** `lingtiwfp64.sys` Windows 驱动文件包含在仓库中（不在发布版本中，因为它很少更改）。此文件**必须与编译后的可执行文件放在同一目录中**，SDK 才能在 Windows 上正常工作。

### 理解 DLL 与 LIB

#### 什么是 DLL？

**DLL（动态链接库）**文件（`lingti_sdk.dll`，13MB）包含所有实际编译的代码：
- 完整的 Go 运行时和垃圾回收器
- 所有 SDK 功能和业务逻辑
- 网络隧道实现
- 应用程序执行时在**运行时**需要

**运行时要求：** `lingti_sdk.dll` 必须在应用程序运行时存在。放置位置：
- 与 `.exe` 文件相同的目录（推荐）
- 系统目录（例如，`C:\Windows\System32`）
- 系统 PATH 环境变量中列出的任何目录

#### 什么是 LIB？

**LIB（导入库）**文件（`lingti_sdk.lib`，8.6KB）要小得多，因为它只包含：
- 带有函数名称引用的存根代码
- 告诉链接器在 DLL 中查找函数的元数据
- 导入表信息

**小尺寸（8.6KB vs 13MB）是正常和正确的！** 导入库只包含对 9 个导出函数的引用，而不是实际的实现代码。

**编译时要求：** `lingti_sdk.lib` 仅在使用 MSVC 编译/链接应用程序时需要。运行时不需要。

#### 何时使用每个文件

| 文件 | 使用时机 | 用途 |
|------|----------|------|
| `lingti_sdk.dll` | 运行时（始终） | 包含所有实际代码，必须与应用程序一起分发 |
| `lingti_sdk.lib` | 编译时（仅 MSVC） | 告诉链接器如何查找 DLL 函数 |
| `lingti_sdk.h` | 编译时（始终） | 为 C 代码提供函数声明 |

#### 编译器特定用法

**MSVC（Visual Studio）：**
```bash
# 编译需要 .lib 文件
cl your_app.c lingti_sdk.lib

# 运行时需要 .dll 文件与 .exe 在同一目录
your_app.exe    # 需要 lingti_sdk.dll 存在
```

**MinGW/GCC：**
```bash
# 可以直接链接到 .dll（不需要 .lib）
gcc your_app.c -L. -llingti_sdk -o your_app.exe

# 运行时需要 .dll 文件
./your_app.exe  # 需要 lingti_sdk.dll 存在
```

#### 分发清单

分发应用程序时，包括：
- ✅ 编译后的 `.exe` 文件
- ✅ `lingti_sdk.dll`（13MB - **运行时必需**）
- ✅ `lingtiwfp64.sys`（Windows 驱动 - **运行时必需**）
- ❌ `lingti_sdk.lib`（最终用户不需要）
- ❌ `lingti_sdk.h`（最终用户不需要）

## C SDK 特性

- **简单的 C API** - 简洁的接口，支持启动/停止服务管理
- **异步操作** - 后台线程中的非阻塞服务执行
- **实时监控** - 跟踪发送/接收的字节和数据包
- **DNS 管理** - 内置 DNS 缓存控制
- **跨平台** - 支持 Windows（DLL）、Linux 和 macOS
- **加密配置** - 通过字符串或文件的安全加密配置
- **流量统计** - 字节和数据包级别的监控
- **错误处理** - 全面的错误码和消息

## C SDK 快速开始

### 最少 5 行代码即可工作

```c
#include "../lingti_sdk.h"
int main() {
    StartTun2RWithConfigFile("encrypted_config.txt");
    return 0;
}
```

### C SDK 基础用法

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
    printf("Lingti SDK 示例\n");
    printf("==================\n\n");

    // 检查 SDK 版本
    char* version = GetSDKVersion();
    printf("SDK 版本: %s\n\n", version);
    free(version);

    // 加密配置文件路径
    // 有关加密详情，请参见 API.md
    const char* configFile = "encrypted_config.txt";

    printf("从配置文件启动服务...\n");
    int result = StartTun2RWithConfigFile(configFile);

    if (result != 0) {
        char* error = GetLastErrorMessage();
        printf("启动服务失败（代码 %d）：%s\n", result, error);
        free(error);
        return 1;
    }

    printf("服务启动成功！\n\n");

    // 检查服务状态
    if (IsServiceRunning()) {
        printf("服务状态：运行中\n\n");
    }

    // 监控流量 30 秒
    printf("监控流量 30 秒...\n");
    printf("按 Ctrl+C 提前停止\n\n");

    for (int i = 0; i < 30; i++) {
        unsigned long long txBytes, rxBytes;
        GetTrafficStats(&txBytes, &rxBytes, NULL, NULL);

        printf("\r[%02d/%02d] 发送: %llu 字节 | 接收: %llu 字节",
               i + 1, 30, txBytes, rxBytes);
        fflush(stdout);

        SLEEP(1000);
    }

    printf("\n\n");

    // 停止服务
    printf("停止服务...\n");
    result = StopTun2R();

    if (result == 0) {
        printf("服务停止成功！\n");
    } else {
        char* error = GetLastErrorMessage();
        printf("停止服务失败（代码 %d）：%s\n", result, error);
        free(error);
    }

    printf("\n示例完成。详细文档请参见 API.md。\n");

    return 0;
}
```

### 加密配置

SDK **仅**支持加密配置以增强安全性。

获取加密配置的步骤：
1. 访问 https://game.lingti.com/sdk
2. 选择您的游戏（需要加速的游戏）
3. 选择您的隧道线路（线路）
4. 复制生成的 encrypted_config 字符串

encrypted_config 是一个 Base64 编码的字符串，包含所有必要的隧道设置。

## C SDK API 参考

### 核心函数

- `StartTun2RWithConfigFile(const char* configPath)` - 从加密配置文件启动服务（base64 编码文本）
- `StopTun2R(void)` - 优雅地停止服务
- `IsServiceRunning(void)` - 检查服务是否正在运行

### 监控函数

- `GetTrafficStats(...)` - 获取当前流量统计
- `GetSDKVersion(void)` - 获取 SDK 版本字符串
- `GetLastErrorMessage(void)` - 获取最后一次错误消息
- `FlushDNSCache(void)` - 刷新本地 DNS 缓存
- `RunPing(int intervalMilliSec)` - 启动周期性 ping 监控
- `StopPing(void)` - 停止 ping 监控
- `GetLastPingStats(...)` - 获取 ping 统计信息
- `GetConsoleConfig(...)` - 获取控制台配置
- `GetDeviceID(void)` - 获取设备 ID

## C SDK 错误码

- `LINGTI_SUCCESS (0)` - 操作成功
- `LINGTI_ERR_NULL_CONFIG (-1)` - 无效/空配置
- `LINGTI_ERR_JSON_PARSE (-2)` - JSON 解析错误
- `LINGTI_ERR_ALREADY_RUN (-3)` - 服务已在运行
- `LINGTI_ERR_LOAD_CONFIG (-4)` - 加载配置文件失败
- `LINGTI_ERR_NOT_RUNNING (-1)` - 服务未运行

## C SDK 构建

### 使用 Makefile（推荐）

使用所有必需文件构建示例：

```bash
make example
```

这将创建一个 `example/` 目录，其中包含：

- `example.exe` - 编译后的可执行文件
- `lingtiwfp64.sys` - Windows 驱动文件
- `lingti_sdk.dll` - SDK 库

清理构建：

```bash
make clean
```

Makefile 会自动检测您的平台：

- **Windows**：使用原生 gcc 或 MinGW
- **Linux/macOS**：使用 MinGW 交叉编译器（使用 `brew install mingw-w64` 安装）

### 手动编译

#### Windows（MinGW）

```bash
gcc your_app.c -L. -llingti_sdk -o your_app.exe
```

#### Windows（MSVC）

```bash
cl your_app.c lingti_sdk.lib
```

#### Linux/macOS（交叉编译）

```bash
x86_64-w64-mingw32-gcc your_app.c lingti_sdk.lib -o your_app.exe
```

## C SDK 示例

查看 `examples/` 目录以获取完整的工作示例：

- `sdk_example.c` - 基本 SDK 使用演示
- `sdk_example_min.c` - 最小 5 行示例
