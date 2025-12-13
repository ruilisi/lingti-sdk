# Lingti SDK

[English](SDK-README.md) | 简体中文

一个轻量级的游戏流量隧道和网络加速 C SDK，支持实时流量监控。

## 概述

Lingti SDK 是一个专为游戏流量优化设计的高性能网络隧道库。它提供简单的 C API，用于将网络加速功能集成到游戏和应用程序中，具有实时流量监控、智能路由和跨平台支持等特性。

## 安装

### 下载所需文件

该 SDK 是开源的，可在以下地址获取：**https://github.com/ruilisi/lingti-sdk**

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

## 特性

- **简单的 C API** - 简洁的接口，支持启动/停止服务管理
- **异步操作** - 后台线程中的非阻塞服务执行
- **实时监控** - 跟踪发送/接收的字节和数据包
- **DNS 管理** - 内置 DNS 缓存控制
- **跨平台** - 支持 Windows（DLL）、Linux 和 macOS
- **加密配置** - 通过字符串或文件的安全加密配置
- **流量统计** - 字节和数据包级别的监控
- **错误处理** - 全面的错误码和消息

## 快速开始

### 最少 5 行代码即可工作

```c
#include "../lingti_sdk.h"
int main() {
    StartTun2RWithConfigFile("encrypted_config.txt");
    return 0;
}
```

### 基础用法

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

## API 参考

### 核心函数

- `StartTun2RWithConfigFile(const char* configPath)` - 从加密配置文件启动服务（base64 编码文本）
- `StopTun2R(void)` - 优雅地停止服务
- `IsServiceRunning(void)` - 检查服务是否正在运行

### 监控函数

- `GetTrafficStats(...)` - 获取当前流量统计
- `GetSDKVersion(void)` - 获取 SDK 版本字符串
- `GetLastErrorMessage(void)` - 获取最后一次错误消息
- `FlushDNSCache(void)` - 刷新本地 DNS 缓存

### 内存管理

- 使用标准 C 的 `free()` 释放 SDK 函数返回的字符串（`GetSDKVersion()`、`GetLastErrorMessage()`、`GetDeviceID()` 以及 `GetConsoleConfig()` 的字符串参数）

## 错误码

- `LINGTI_SUCCESS (0)` - 操作成功
- `LINGTI_ERR_NULL_CONFIG (-1)` - 无效/空配置
- `LINGTI_ERR_JSON_PARSE (-2)` - JSON 解析错误
- `LINGTI_ERR_ALREADY_RUN (-3)` - 服务已在运行
- `LINGTI_ERR_LOAD_CONFIG (-4)` - 加载配置文件失败
- `LINGTI_ERR_NOT_RUNNING (-1)` - 服务未运行

## 构建

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

## 示例

查看 `examples/` 目录以获取完整的工作示例：

- `sdk_example.c` - 基本 SDK 使用演示
- `sdk_example_min.c` - 最小 5 行示例

## 许可证

Copyright (c) 2025 Ruilisi

## 版本

当前版本：1.5.5
