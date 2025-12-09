# 配置指南

[English](CONFIGURATION.md) | [简体中文](CONFIGURATION.zh-CN.md)

使用加密配置文件配置 Lingti SDK 的完整指南。

## 目录

- [概述](#概述)
- [加密配置](#加密配置)
- [获取配置](#获取配置)
- [配置文件格式](#配置文件格式)
- [使用配置](#使用配置)
- [配置存储](#配置存储)
- [安全最佳实践](#安全最佳实践)

## 概述

Lingti SDK 使用**加密配置**以增强安全性。所有配置数据均经过 Base64 编码和加密，确保服务器地址和令牌等敏感信息得到保护。

### 为什么使用加密配置？

- **安全性**：防止未经授权访问服务器详细信息和身份验证令牌
- **防篡改**：最终用户无法轻易修改配置
- **集中控制**：后端服务控制哪些配置有效
- **简化分发**：可以向最终用户分发单个加密文件

## 加密配置

### 什么是加密配置？

加密配置是一个 Base64 编码的文本字符串，包含隧道服务的所有必要设置，包括：

- 服务器地址和端口
- 身份验证令牌
- 隧道模式设置
- 游戏可执行文件名称
- 路由规则
- 日志级别

加密确保在没有适当解密密钥的情况下无法读取或修改敏感数据。

### 配置工作流

```
1. 用户选择游戏和线路
   ↓
2. 前端将选择发送到后端
   ↓
3. 后端生成加密配置
   ↓
4. 前端接收加密配置
   ↓
5. SDK 使用加密配置启动隧道
```

## 获取配置

### 从后端服务获取

配置必须从 Lingti 后端服务获取。典型流程是：

1. **用户选择**：用户通过应用程序 UI 选择游戏和首选路由线路
2. **API 请求**：应用程序将选择发送到后端 API
3. **配置生成**：后端生成并加密适当的配置
4. **配置交付**：加密配置返回到应用程序

### API 端点（示例）

```javascript
const axios = require('axios');

async function getEncryptedConfig(gameId, lineId) {
    try {
        const response = await axios.post('https://api.lingti.com/v1/config/generate', {
            game: gameId,
            line: lineId,
            userId: 'user123'  // 您的用户标识符
        }, {
            headers: {
                'Authorization': 'Bearer YOUR_API_KEY'
            }
        });

        return response.data.encryptedConfig;
    } catch (error) {
        console.error('获取配置失败:', error);
        throw error;
    }
}
```

**注意**：联系 Lingti 支持以获取 API 访问权限。参见：https://xiemala.com/f/rY1aZz

### 配置格式

加密配置可以通过两种方式使用：

#### 1. 直接字符串（Base64 编码）

```javascript
const encryptedConfig = "SGVsbG8gV29ybGQhVGhpcyBpcyBhbiBlbmNyeXB0ZWQgY29uZmlndXJhdGlvbg==";
lingti.startTun2R(encryptedConfig);
```

#### 2. 配置文件

将加密字符串保存到文件并加载：

```javascript
// 保存到文件
const fs = require('fs');
fs.writeFileSync('encrypted_config.txt', encryptedConfig);

// 从文件加载
lingti.startTun2RWithConfigFile('encrypted_config.txt');
```

## 配置文件格式

### 文件结构

配置文件是一个纯文本文件，仅包含 Base64 编码的加密配置字符串：

**encrypted_config.txt：**
```
SGVsbG8gV29ybGQhVGhpcyBpcyBhbiBlbmNyeXB0ZWQgY29uZmlndXJhdGlvbg==
```

### 重要注意事项

- **无多余空格**：文件应仅包含加密字符串
- **无换行符**：单行 Base64 编码文本
- **UTF-8 编码**：文件必须使用 UTF-8 编码保存
- **无 BOM**：避免字节顺序标记（BOM）字符

### 验证配置文件

```javascript
const fs = require('fs');

function validateConfigFile(filePath) {
    try {
        const content = fs.readFileSync(filePath, 'utf8').trim();

        // 检查是否为 Base64
        const base64Regex = /^[A-Za-z0-9+/=]+$/;
        if (!base64Regex.test(content)) {
            console.error('无效的 Base64 格式');
            return false;
        }

        // 检查长度
        if (content.length < 100) {
            console.error('配置似乎太短');
            return false;
        }

        console.log('配置文件有效');
        return true;
    } catch (error) {
        console.error('读取配置文件失败:', error);
        return false;
    }
}
```

## 使用配置

### 方法 1：直接字符串

在动态接收配置时使用：

```javascript
const lingti = require('lingti-sdk');

// 从后端获取配置
const encryptedConfig = await getEncryptedConfig('GAME_ID', 'LINE_ID');

// 直接启动服务
const result = lingti.startTun2R(encryptedConfig);
if (result === 0) {
    console.log('隧道启动成功');
}
```

### 方法 2：配置文件

在需要持久化配置时使用：

```javascript
const lingti = require('lingti-sdk');
const fs = require('fs');

// 保存配置到文件
async function saveAndStartTunnel(gameId, lineId) {
    // 获取配置
    const encryptedConfig = await getEncryptedConfig(gameId, lineId);

    // 保存到文件
    fs.writeFileSync('encrypted_config.txt', encryptedConfig);

    // 从文件启动服务
    const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');

    if (result === 0) {
        console.log('隧道启动成功');
    } else {
        console.error('启动失败:', lingti.getLastErrorMessage());
    }
}
```

### 方法 3：默认配置文件

将 `encrypted_config.txt` 放在应用程序目录中：

```javascript
// SDK 将在当前目录中查找 encrypted_config.txt
const result = lingti.startTun2RWithConfigFile();
```

## 配置存储

### 本地存储

对于 Electron 或基于浏览器的应用程序：

```javascript
// 保存配置
localStorage.setItem('lingti_config', encryptedConfig);

// 加载配置
const savedConfig = localStorage.getItem('lingti_config');
if (savedConfig) {
    lingti.startTun2R(savedConfig);
}
```

### 文件系统存储

对于 Node.js 应用程序：

```javascript
const fs = require('fs');
const path = require('path');
const os = require('os');

// 获取用户数据目录
const configDir = path.join(os.homedir(), '.lingti');
const configPath = path.join(configDir, 'encrypted_config.txt');

// 保存配置
function saveConfig(encryptedConfig) {
    // 如果目录不存在则创建
    if (!fs.existsSync(configDir)) {
        fs.mkdirSync(configDir, { recursive: true });
    }

    // 写入配置
    fs.writeFileSync(configPath, encryptedConfig, 'utf8');
    console.log('配置已保存到:', configPath);
}

// 加载配置
function loadConfig() {
    if (fs.existsSync(configPath)) {
        return fs.readFileSync(configPath, 'utf8').trim();
    }
    return null;
}

// 使用
const config = loadConfig();
if (config) {
    lingti.startTun2R(config);
} else {
    console.log('未找到保存的配置');
}
```

### 数据库存储

对于使用数据库的应用程序：

```javascript
// 保存到数据库（SQLite 示例）
const sqlite3 = require('sqlite3');
const db = new sqlite3.Database('app.db');

function saveConfigToDB(userId, encryptedConfig) {
    db.run(
        'INSERT OR REPLACE INTO configs (user_id, config, updated_at) VALUES (?, ?, ?)',
        [userId, encryptedConfig, Date.now()]
    );
}

function loadConfigFromDB(userId, callback) {
    db.get(
        'SELECT config FROM configs WHERE user_id = ?',
        [userId],
        (err, row) => {
            if (err) {
                console.error('数据库错误:', err);
                callback(null);
            } else {
                callback(row ? row.config : null);
            }
        }
    );
}
```

## 安全最佳实践

### 1. 保护配置文件

```javascript
const fs = require('fs');

// 设置文件权限（类 Unix 系统）
if (process.platform !== 'win32') {
    fs.chmodSync('encrypted_config.txt', 0o600); // 仅所有者可读/写
}
```

### 2. 避免记录配置

```javascript
// ❌ 错误：记录配置
console.log('配置:', encryptedConfig);

// ✅ 正确：仅记录状态
console.log('配置已加载，长度:', encryptedConfig.length);
```

### 3. 验证配置来源

```javascript
function isConfigValid(config) {
    // 检查格式
    if (!config || typeof config !== 'string') {
        return false;
    }

    // 检查 Base64 格式
    const base64Regex = /^[A-Za-z0-9+/=]+$/;
    if (!base64Regex.test(config)) {
        return false;
    }

    // 检查最小长度
    if (config.length < 100) {
        return false;
    }

    return true;
}

// 使用验证
if (isConfigValid(encryptedConfig)) {
    lingti.startTun2R(encryptedConfig);
} else {
    console.error('配置格式无效');
}
```

### 4. 处理配置更新

```javascript
// 检查配置是否需要更新
async function checkAndUpdateConfig(currentConfig) {
    try {
        // 向后端检查配置是否仍然有效
        const response = await axios.post('https://api.lingti.com/v1/config/validate', {
            configHash: hashConfig(currentConfig)
        });

        if (!response.data.isValid) {
            // 配置已过期，获取新配置
            const newConfig = await getEncryptedConfig(gameId, lineId);
            saveConfig(newConfig);
            return newConfig;
        }

        return currentConfig;
    } catch (error) {
        console.error('验证配置失败:', error);
        return currentConfig; // 出错时使用现有配置
    }
}
```

### 5. 安全配置传输

```javascript
// 对 API 调用使用 HTTPS
const https = require('https');
const axios = require('axios');

const httpsAgent = new https.Agent({
    rejectUnauthorized: true, // 验证 SSL 证书
});

async function getEncryptedConfigSecure(gameId, lineId) {
    return axios.post('https://api.lingti.com/v1/config/generate', {
        game: gameId,
        line: lineId
    }, {
        httpsAgent,
        timeout: 10000 // 10 秒超时
    });
}
```

## 完整配置示例

```javascript
const lingti = require('lingti-sdk');
const fs = require('fs');
const path = require('path');
const axios = require('axios');

class LingtiConfigManager {
    constructor(apiKey, configDir = './config') {
        this.apiKey = apiKey;
        this.configDir = configDir;
        this.configFile = path.join(configDir, 'encrypted_config.txt');

        // 创建配置目录
        if (!fs.existsSync(configDir)) {
            fs.mkdirSync(configDir, { recursive: true });
        }
    }

    async fetchConfig(gameId, lineId) {
        try {
            const response = await axios.post(
                'https://api.lingti.com/v1/config/generate',
                { game: gameId, line: lineId },
                { headers: { 'Authorization': `Bearer ${this.apiKey}` } }
            );

            return response.data.encryptedConfig;
        } catch (error) {
            console.error('获取配置失败:', error.message);
            throw error;
        }
    }

    saveConfig(encryptedConfig) {
        fs.writeFileSync(this.configFile, encryptedConfig, 'utf8');

        // 设置文件权限（类 Unix 系统）
        if (process.platform !== 'win32') {
            fs.chmodSync(this.configFile, 0o600);
        }
    }

    loadConfig() {
        if (fs.existsSync(this.configFile)) {
            return fs.readFileSync(this.configFile, 'utf8').trim();
        }
        return null;
    }

    async ensureConfig(gameId, lineId, forceRefresh = false) {
        if (!forceRefresh) {
            const existing = this.loadConfig();
            if (existing) {
                return existing;
            }
        }

        const config = await this.fetchConfig(gameId, lineId);
        this.saveConfig(config);
        return config;
    }

    async startTunnel(gameId, lineId) {
        if (!lingti.isAddonAvailable()) {
            throw new Error('平台不受支持');
        }

        const config = await this.ensureConfig(gameId, lineId);
        const result = lingti.startTun2R(config);

        if (result !== 0) {
            throw new Error(`启动隧道失败: ${lingti.getLastErrorMessage()}`);
        }

        console.log('隧道启动成功');
    }
}

// 使用
const manager = new LingtiConfigManager('YOUR_API_KEY');
manager.startTunnel('GAME_ID', 'LINE_ID')
    .then(() => console.log('运行中'))
    .catch(error => console.error('错误:', error));
```

## 另请参阅

- [API 参考](API.zh-CN.md) - 完整的 API 文档
- [示例](EXAMPLES.zh-CN.md) - 使用示例
- [故障排除](TROUBLESHOOTING.zh-CN.md) - 常见配置问题
