# Changelog

## [Updated] - 2025-12-09

### Changed - Encrypted Config Parameter

The example apps and SDK integration now use `encrypted_config` (a base64 encoded string) instead of individual configuration parameters.

#### Modified Files:

1. **[node-addon/example.js](example.js)**
   - Changed from `config` object to `encrypted_config` string
   - Added example of how to encode config for testing

2. **[node-addon/example/main.js](example/main.js)**
   - Updated `sdk:start` IPC handler to accept `encryptedConfig`
   - Removed config object construction, now passes string directly

3. **[node-addon/example/preload.js](example/preload.js)**
   - Renamed parameter from `encryptedToken` to `encryptedConfig`

4. **[node-addon/example/index.html](example/index.html)**
   - Changed input label from "Encrypted Token" to "Encrypted Config (Base64)"
   - Updated input ID from `tokenInput` to `configInput`
   - Updated placeholder text

5. **[node-addon/example/renderer.js](example/renderer.js)**
   - Renamed all references from `token`/`tokenInput` to `config`/`configInput`
   - Updated localStorage key from `lingti_token` to `lingti_encrypted_config`
   - Updated error messages

#### New Files:

1. **[node-addon/example/config-helper.js](example/config-helper.js)**
   - Utility to encode/decode configs for testing
   - Example usage included
   - Can be run standalone: `node config-helper.js`

2. **[node-addon/example/ENCRYPTED_CONFIG_GUIDE.md](example/ENCRYPTED_CONFIG_GUIDE.md)**
   - Comprehensive guide on using encrypted_config
   - Production vs development usage patterns
   - Security considerations and best practices
   - Testing, troubleshooting, and examples

#### Updated Documentation:

- [example/README.md](example/README.md) - Updated usage instructions
- [example/SUCCESS.md](example/SUCCESS.md) - Updated configuration section
- [example/QUICKSTART.md](example/QUICKSTART.md) - Added encrypted_config explanation

### Migration

**Before:**
```javascript
const config = { Mode: "...", Server: "...", Token: "...", ... };
lingtiSdk.startTun2R(config);
```

**After:**
```javascript
const encrypted_config = "base64-encoded-string-here";
lingtiSdk.startTun2R(encrypted_config);
```

For testing, use the helper:
```bash
node example/config-helper.js
```

### Benefits

- ✅ More secure - entire config is encrypted, not just token
- ✅ Simpler API - single parameter instead of object
- ✅ Backend controlled - server manages all configuration
- ✅ Production ready - matches deployment patterns

---

## [1.0.0] - 2025-12-09

### Added - Initial Release

#### Core Components:

1. **Node.js Native Addon** ([node-addon/](./))
   - Native C++ bindings to Lingti SDK DLL
   - Full SDK API exposed to Node.js
   - Cross-platform support (Windows required for native features)

2. **SDK Build System**
   - [build_sdk.bat](../build_sdk.bat) - Windows SDK build script
   - [build_sdk.sh](../build_sdk.sh) - Linux/macOS cross-compile script
   - Automatic DLL and import library generation

3. **Electron Example App** ([example/](example/))
   - Complete Electron application demonstrating SDK usage
   - Modern dark theme UI
   - Real-time statistics display
   - Token/config persistence
   - Error handling and user feedback

#### Features:

- ✅ Start/Stop tunnel service
- ✅ Service status monitoring
- ✅ Real-time traffic statistics
- ✅ Ping monitoring
- ✅ DNS cache flushing
- ✅ Console configuration retrieval
- ✅ SDK version information
- ✅ Error message reporting

#### API Functions:

- `startTun2R(configJSON)` - Start service with JSON config
- `startTun2RWithConfigFile(path)` - Start with config file
- `stopTun2R()` - Stop service
- `isServiceRunning()` - Check service status
- `getSDKVersion()` - Get version string
- `getLastErrorMessage()` - Get last error
- `getTrafficStats()` - Get traffic statistics
- `getLastPingStats()` - Get ping statistics
- `runPing()` - Start ping monitoring
- `stopPing()` - Stop ping monitoring
- `flushDNSCache()` - Flush DNS cache
- `getConsoleConfig()` - Get console configuration

#### Documentation:

- [README.md](example/README.md) - Full documentation
- [QUICKSTART.md](example/QUICKSTART.md) - Quick start guide
- [SUCCESS.md](example/SUCCESS.md) - Setup success guide
- [INSTALL_STATUS.md](example/INSTALL_STATUS.md) - Installation status
- [ENCRYPTED_CONFIG_GUIDE.md](example/ENCRYPTED_CONFIG_GUIDE.md) - Config guide

#### Build Configuration:

- [package.json](package.json) - Node addon package config
- [binding.gyp](binding.gyp) - Node-gyp build configuration
- [index.js](index.js) - Main entry point
- [index.d.ts](index.d.ts) - TypeScript definitions

#### Examples:

- [example.js](example.js) - Simple Node.js usage example
- [test.js](test.js) - Test script
- [example/](example/) - Full Electron app

### Technical Details:

- **Language**: C++ (addon), JavaScript (wrapper), TypeScript (types)
- **Runtime**: Node.js 16+
- **Platform**: Windows (native features), cross-platform (fallback)
- **Framework**: Electron 33.2.0 (example app)
- **Build System**: node-gyp, Visual Studio 2022
- **SDK Version**: 1.4.3

### Security:

- Context isolation enabled
- No node integration in renderer
- IPC-based communication
- Controlled API exposure via preload
- Error code constants exported

---

## Notes

### Version Numbering

This addon follows semantic versioning: MAJOR.MINOR.PATCH

- **MAJOR**: Incompatible API changes
- **MINOR**: New functionality (backward compatible)
- **PATCH**: Bug fixes (backward compatible)

### Platform Support

- **Windows**: Full support (native addon)
- **macOS/Linux**: Limited (addon loads but features unavailable)

### Dependencies

- **Runtime**: Node.js 16+, Visual C++ Redistributable
- **Build**: Python 3, Visual Studio 2022, node-gyp
- **Development**: Electron, TypeScript (optional)
