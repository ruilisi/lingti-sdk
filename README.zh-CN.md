# Lingti SDK

[English](README.md) | 简体中文

轻量级 C SDK，用于游戏流量隧道和网络加速，支持实时流量监控。

<p align="center">
  <a href="https://game.lingti.com" style="text-decoration:none">
    <img src="docs/assets/lingti-homepage.png" alt="灵缇官网" width="100%" style="border-radius:14px;" />
    <br /><em>点击进入灵缇加速器官网</em>
  </a>
</p>

## 概览

Lingti SDK 是一套高性能网络隧道库，提供简单的 API，帮助游戏/应用快速接入网络加速能力，包含实时流量监控、智能路由和多平台支持。

## 安装

### 必要文件

源码仓库：**https://github.com/ruilisi/lingti-sdk**  
预编译 DLL/lib 可在 [GitHub Releases](https://github.com/ruilisi/lingti-sdk/releases) 获取：

- `lingti_sdk.dll` - 主库（13MB，运行时必需）
- `lingti_sdk.lib` - 导入库（8.6KB，MSVC 编译时使用）
- `lingti_sdk.h` - 头文件
- `lingti_sdk.def` - 模块定义
- `lingtiwfp64.sys` - Windows 驱动，需与可执行文件同目录

### DLL 与 LIB

- DLL：包含全部实现，运行时必须存在，可放在 exe 同目录或 PATH 目录。
- LIB：仅 MSVC 编译时需要；运行时不需要。
- MinGW/GCC 可直接链接 DLL。

### 编译示例

```bash
make example    # 生成 example/ 下的可执行程序及所需文件
make clean
```

### Node.js 包 (lingti-sdk) （推荐）

- 运行需要 Windows；在 macOS/Linux 上可安装但会跳过原生构建。
- npm 安装：

```bash
npm install lingti-sdk
```

```javascript
const lingti = require('lingti-sdk');
if (lingti.isAddonAvailable()) {
  lingti.startTun2RWithConfigFile('encrypted_config.txt');
}
```

手动编译：

- Windows (MinGW): `gcc your_app.c -L. -llingti_sdk -o your_app.exe`
- Windows (MSVC): `cl your_app.c lingti_sdk.lib`
- macOS/Linux 交叉编译: `x86_64-w64-mingw32-gcc your_app.c lingti_sdk.lib -o your_app.exe`

## 功能

- 简洁 C API，启动/停止服务
- 异步后台运行
- 流量监控（字节/包数）
- DNS 缓存刷新
- 多平台：Windows DLL，可在 macOS/Linux 交叉编译
- 加密配置：字符串或文件
- 错误码与错误信息

## 快速开始

最小示例（5 行）：

```c
#include "../lingti_sdk.h"
int main() {
    StartTun2RWithConfigFile("encrypted_config.txt");
    return 0;
}
```

更多示例见 `sdk_example.c`、`sdk_example_min.c`。

## 加密配置 encrypted_config

SDK 只接受加密配置（Base64 文本）。获取方式：

1. 访问 https://game.lingti.com/sdk
2. 选择需要加速的游戏
3. 选择线路 (Area)
4. 复制生成的 `encrypted_config`（可保存为 `encrypted_config.txt`）

点击下方图片打开生成页并下载配置：

<p align="center">
  <a href="https://game.lingti.com/sdk" style="text-decoration:none">
    <img src="docs/assets/lingti-sdk.png" alt="生成 encrypted_config" width="80%" style="border-radius:14px;" />
    <br /><em>点击进入 encrypted_config 生成页</em>
  </a>
</p>

## API 概要

- `StartTun2RWithConfigFile(const char* path)`：从加密配置文件启动
- `StopTun2R()`：停止服务
- `IsServiceRunning()`：查询状态
- `GetTrafficStats(...)`：获取流量统计
- `GetSDKVersion()` / `GetLastErrorMessage()` / `GetDeviceID()`
- `FlushDNSCache()`：刷新 DNS
- 详细见 `lingti_sdk.h`。

## 错误码

- `0` 成功
- `-1` 配置为空 / 服务未运行（Stop）
- `-2` JSON 解析失败
- `-3` 服务已在运行
- `-4` 加载配置文件失败

## 版本

`1.6.0`(最新): 大大提升流量稳定性并降低丢包率。

## 演示视频

<p align="center">
  <a href="https://www.bilibili.com/video/BV1Y4SiBLEWB/?share_source=copy_web&vd_source=6b41ec669aaccb71ab94fe20193f3cd1" style="text-decoration:none">
    <img src="docs/assets/sdk-video-frontpage.jpeg" alt="Bilibili 教学视频" width="65%" style="border-radius:14px;" />
    <br /><em>点击观看 B 站教程</em>
  </a>
</p>

## 联系我们

<p align="center">
  <a href="https://game.lingti.com/contact_us" style="text-decoration:none">
    <img src="docs/assets/wechat-qr.png" alt="联系灵缇" width="180" style="border-radius:14px;" />
    <br /><em>扫码或点击联系灵缇团队</em>
  </a>
</p>
