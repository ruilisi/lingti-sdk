# Configuration Guide

[English](CONFIGURATION.md) | [简体中文](CONFIGURATION.zh-CN.md)

Complete guide for configuring the Lingti SDK with encrypted configuration files.

## Table of Contents

- [Overview](#overview)
- [Encrypted Configuration](#encrypted-configuration)
- [Obtaining Configuration](#obtaining-configuration)
- [Configuration File Format](#configuration-file-format)
- [Using Configuration](#using-configuration)
- [Configuration Storage](#configuration-storage)
- [Security Best Practices](#security-best-practices)

## Overview

The Lingti SDK uses **encrypted configuration** for enhanced security. All configuration data is Base64-encoded and encrypted, ensuring that sensitive information like server addresses and tokens remain protected.

### Why Encrypted Configuration?

- **Security**: Prevents unauthorized access to server details and authentication tokens
- **Tamper-proof**: Configuration cannot be easily modified by end users
- **Centralized Control**: Backend service controls which configurations are valid
- **Simple Distribution**: Single encrypted file can be distributed to end users

## Encrypted Configuration

### What is Encrypted Configuration?

Encrypted configuration is a Base64-encoded text string that contains all necessary settings for the tunnel service, including:

- Server addresses and ports
- Authentication tokens
- Tunnel mode settings
- Game executable names
- Routing rules
- Log levels

The encryption ensures that sensitive data cannot be read or modified without proper decryption keys.

### Configuration Workflow

```
1. User selects game and line
   ↓
2. Frontend sends selection to backend
   ↓
3. Backend generates encrypted config
   ↓
4. Frontend receives encrypted config
   ↓
5. SDK uses encrypted config to start tunnel
```

## Obtaining Configuration

### From Backend Service

Configuration must be obtained from the Lingti backend service. The typical flow is:

1. **User Selection**: User selects the game and preferred routing line through your application UI
2. **API Request**: Your application sends the selection to the backend API
3. **Config Generation**: Backend generates and encrypts the appropriate configuration
4. **Config Delivery**: Encrypted configuration is returned to your application

### API Endpoint (Example)

```javascript
const axios = require('axios');

async function getEncryptedConfig(gameId, lineId) {
    try {
        const response = await axios.post('https://api.lingti.com/v1/config/generate', {
            game: gameId,
            line: lineId,
            userId: 'user123'  // Your user identifier
        }, {
            headers: {
                'Authorization': 'Bearer YOUR_API_KEY'
            }
        });

        return response.data.encryptedConfig;
    } catch (error) {
        console.error('Failed to get config:', error);
        throw error;
    }
}
```

**Note**: Contact Lingti support to get API access. See: https://xiemala.com/f/rY1aZz

### Configuration Formats

The encrypted configuration can be used in two ways:

#### 1. Direct String (Base64 encoded)

```javascript
const encryptedConfig = "SGVsbG8gV29ybGQhVGhpcyBpcyBhbiBlbmNyeXB0ZWQgY29uZmlndXJhdGlvbg==";
lingti.startTun2R(encryptedConfig);
```

#### 2. Configuration File

Save the encrypted string to a file and load it:

```javascript
// Save to file
const fs = require('fs');
fs.writeFileSync('encrypted_config.txt', encryptedConfig);

// Load from file
lingti.startTun2RWithConfigFile('encrypted_config.txt');
```

## Configuration File Format

### File Structure

The configuration file is a plain text file containing only the Base64-encoded encrypted configuration string:

**encrypted_config.txt:**
```
SGVsbG8gV29ybGQhVGhpcyBpcyBhbiBlbmNyeXB0ZWQgY29uZmlndXJhdGlvbg==
```

### Important Notes

- **No extra whitespace**: The file should contain only the encrypted string
- **No line breaks**: Single line of Base64-encoded text
- **UTF-8 encoding**: File must be saved with UTF-8 encoding
- **No BOM**: Avoid Byte Order Mark (BOM) characters

### Validating Configuration File

```javascript
const fs = require('fs');

function validateConfigFile(filePath) {
    try {
        const content = fs.readFileSync(filePath, 'utf8').trim();

        // Check if it's Base64
        const base64Regex = /^[A-Za-z0-9+/=]+$/;
        if (!base64Regex.test(content)) {
            console.error('Invalid Base64 format');
            return false;
        }

        // Check length
        if (content.length < 100) {
            console.error('Configuration seems too short');
            return false;
        }

        console.log('Configuration file is valid');
        return true;
    } catch (error) {
        console.error('Failed to read config file:', error);
        return false;
    }
}
```

## Using Configuration

### Method 1: Direct String

Use when you receive the configuration dynamically:

```javascript
const lingti = require('lingti-sdk');

// Get configuration from backend
const encryptedConfig = await getEncryptedConfig('GAME_ID', 'LINE_ID');

// Start service directly
const result = lingti.startTun2R(encryptedConfig);
if (result === 0) {
    console.log('Tunnel started successfully');
}
```

### Method 2: Configuration File

Use when you want to persist configuration:

```javascript
const lingti = require('lingti-sdk');
const fs = require('fs');

// Save configuration to file
async function saveAndStartTunnel(gameId, lineId) {
    // Get configuration
    const encryptedConfig = await getEncryptedConfig(gameId, lineId);

    // Save to file
    fs.writeFileSync('encrypted_config.txt', encryptedConfig);

    // Start service from file
    const result = lingti.startTun2RWithConfigFile('encrypted_config.txt');

    if (result === 0) {
        console.log('Tunnel started successfully');
    } else {
        console.error('Failed to start:', lingti.getLastErrorMessage());
    }
}
```

### Method 3: Default Configuration File

Place `encrypted_config.txt` in your application directory:

```javascript
// SDK will look for encrypted_config.txt in current directory
const result = lingti.startTun2RWithConfigFile();
```

## Configuration Storage

### Local Storage

For Electron or browser-based applications:

```javascript
// Save configuration
localStorage.setItem('lingti_config', encryptedConfig);

// Load configuration
const savedConfig = localStorage.getItem('lingti_config');
if (savedConfig) {
    lingti.startTun2R(savedConfig);
}
```

### File System Storage

For Node.js applications:

```javascript
const fs = require('fs');
const path = require('path');
const os = require('os');

// Get user data directory
const configDir = path.join(os.homedir(), '.lingti');
const configPath = path.join(configDir, 'encrypted_config.txt');

// Save configuration
function saveConfig(encryptedConfig) {
    // Create directory if it doesn't exist
    if (!fs.existsSync(configDir)) {
        fs.mkdirSync(configDir, { recursive: true });
    }

    // Write configuration
    fs.writeFileSync(configPath, encryptedConfig, 'utf8');
    console.log('Configuration saved to:', configPath);
}

// Load configuration
function loadConfig() {
    if (fs.existsSync(configPath)) {
        return fs.readFileSync(configPath, 'utf8').trim();
    }
    return null;
}

// Usage
const config = loadConfig();
if (config) {
    lingti.startTun2R(config);
} else {
    console.log('No saved configuration found');
}
```

### Database Storage

For applications with database:

```javascript
// Save to database (example with SQLite)
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
                console.error('Database error:', err);
                callback(null);
            } else {
                callback(row ? row.config : null);
            }
        }
    );
}
```

## Security Best Practices

### 1. Protect Configuration Files

```javascript
const fs = require('fs');

// Set file permissions (Unix-like systems)
if (process.platform !== 'win32') {
    fs.chmodSync('encrypted_config.txt', 0o600); // Read/write for owner only
}
```

### 2. Avoid Logging Configuration

```javascript
// ❌ Bad: Logs configuration
console.log('Config:', encryptedConfig);

// ✅ Good: Logs only status
console.log('Configuration loaded, length:', encryptedConfig.length);
```

### 3. Validate Configuration Source

```javascript
function isConfigValid(config) {
    // Check format
    if (!config || typeof config !== 'string') {
        return false;
    }

    // Check Base64 format
    const base64Regex = /^[A-Za-z0-9+/=]+$/;
    if (!base64Regex.test(config)) {
        return false;
    }

    // Check minimum length
    if (config.length < 100) {
        return false;
    }

    return true;
}

// Use validation
if (isConfigValid(encryptedConfig)) {
    lingti.startTun2R(encryptedConfig);
} else {
    console.error('Invalid configuration format');
}
```

### 4. Handle Configuration Updates

```javascript
// Check if configuration needs update
async function checkAndUpdateConfig(currentConfig) {
    try {
        // Check with backend if config is still valid
        const response = await axios.post('https://api.lingti.com/v1/config/validate', {
            configHash: hashConfig(currentConfig)
        });

        if (!response.data.isValid) {
            // Configuration expired, get new one
            const newConfig = await getEncryptedConfig(gameId, lineId);
            saveConfig(newConfig);
            return newConfig;
        }

        return currentConfig;
    } catch (error) {
        console.error('Failed to validate config:', error);
        return currentConfig; // Use existing config on error
    }
}
```

### 5. Secure Configuration Transmission

```javascript
// Use HTTPS for API calls
const https = require('https');
const axios = require('axios');

const httpsAgent = new https.Agent({
    rejectUnauthorized: true, // Verify SSL certificates
});

async function getEncryptedConfigSecure(gameId, lineId) {
    return axios.post('https://api.lingti.com/v1/config/generate', {
        game: gameId,
        line: lineId
    }, {
        httpsAgent,
        timeout: 10000 // 10 second timeout
    });
}
```

## Complete Configuration Example

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

        // Create config directory
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
            console.error('Failed to fetch config:', error.message);
            throw error;
        }
    }

    saveConfig(encryptedConfig) {
        fs.writeFileSync(this.configFile, encryptedConfig, 'utf8');

        // Set file permissions (Unix-like systems)
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
            throw new Error('Platform not supported');
        }

        const config = await this.ensureConfig(gameId, lineId);
        const result = lingti.startTun2R(config);

        if (result !== 0) {
            throw new Error(`Failed to start tunnel: ${lingti.getLastErrorMessage()}`);
        }

        console.log('Tunnel started successfully');
    }
}

// Usage
const manager = new LingtiConfigManager('YOUR_API_KEY');
manager.startTunnel('GAME_ID', 'LINE_ID')
    .then(() => console.log('Running'))
    .catch(error => console.error('Error:', error));
```

## See Also

- [API Reference](API.md) - Complete API documentation
- [Examples](EXAMPLES.md) - Usage examples
- [Troubleshooting](TROUBLESHOOTING.md) - Common configuration issues
