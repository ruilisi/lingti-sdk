/**
 * Lingti SDK - Node.js Native Addon
 *
 * High-level JavaScript wrapper for the Lingti SDK native addon.
 */

const os = require('os');

// Try to load the native addon
let addon = null;
let addonError = null;

try {
    addon = require('./build/Release/lingti_sdk.node');
} catch (err) {
    addonError = err;
}

// Helper function to throw error if addon not available
function ensureAddon() {
    if (!addon) {
        const platform = os.platform();
        throw new Error(
            `Lingti SDK native addon is not available.\n` +
            `Current platform: ${platform}\n` +
            `This addon requires Windows to build and run.\n` +
            (platform === 'win32'
                ? `Try running: npm run build\nOriginal error: ${addonError?.message}`
                : `Please run this code on Windows.`)
        );
    }
}

/**
 * Error codes exported from the SDK
 */
const ErrorCodes = addon ? addon.ErrorCodes : {
    SUCCESS: 0,
    ERR_NULL_CONFIG: -1,
    ERR_JSON_PARSE: -2,
    ERR_ALREADY_RUN: -3,
    ERR_LOAD_CONFIG: -4,
    ERR_NOT_RUNNING: -1
};

/**
 * Start the TUN2R service with JSON configuration
 * @param {string|object} config - JSON string or object containing configuration
 * @returns {number} 0 on success, negative error code on failure
 * @example
 * const result = lingti.startTun2R({
 *   Mode: "tun_switch",
 *   Server: "server.com:port",
 *   Token: "your-token",
 *   LogLevel: "info",
 *   GameExes: ["game.exe"],
 *   GameID: "YOUR_GAME"
 * });
 */
function startTun2R(config) {
    ensureAddon();
    const configJSON = typeof config === 'string' ? config : JSON.stringify(config);
    return addon.startTun2R(configJSON);
}

/**
 * Start the TUN2R service using a config file
 * @param {string} [configPath] - Path to config file (optional, uses default if not provided)
 * @returns {number} 0 on success, negative error code on failure
 */
function startTun2RWithConfigFile(configPath) {
    ensureAddon();
    return addon.startTun2RWithConfigFile(configPath);
}

/**
 * Stop the TUN2R service gracefully
 * @returns {number} 0 on success, negative error code on failure
 */
function stopTun2R() {
    ensureAddon();
    return addon.stopTun2R();
}

/**
 * Check if the service is currently running
 * @returns {boolean} true if running, false otherwise
 */
function isServiceRunning() {
    ensureAddon();
    return addon.isServiceRunning();
}

/**
 * Get the SDK version string
 * @returns {string} Version string (e.g., "1.4.3")
 */
function getSDKVersion() {
    ensureAddon();
    return addon.getSDKVersion();
}

/**
 * Get the last error message
 * @returns {string} Error message string
 */
function getLastErrorMessage() {
    ensureAddon();
    return addon.getLastErrorMessage();
}

/**
 * Get current traffic statistics
 * @returns {{txBytes: number, rxBytes: number, txPkts: number, rxPkts: number}}
 */
function getTrafficStats() {
    ensureAddon();
    return addon.getTrafficStats();
}

/**
 * Get the latest ping statistics
 * @returns {{router: number, takeoff: number, landing: number}}
 */
function getLastPingStats() {
    ensureAddon();
    return addon.getLastPingStats();
}

/**
 * Start periodic ping monitoring
 * @returns {number} 0 on success, negative error code on failure
 */
function runPing() {
    ensureAddon();
    return addon.runPing();
}

/**
 * Stop periodic ping monitoring
 * @returns {number} 0 on success, negative error code on failure
 */
function stopPing() {
    ensureAddon();
    return addon.stopPing();
}

/**
 * Flush the DNS cache
 * @returns {number} 0 on success
 */
function flushDNSCache() {
    ensureAddon();
    return addon.flushDNSCache();
}

/**
 * Delete the lingtiwfp Windows service
 * @returns {number} 0 on success
 */
function deleteService() {
    ensureAddon();
    return addon.deleteService();
}

/**
 * Get console configuration parameters
 * @returns {{state: number, stateStr: string, gateway: string, mask: string, ip: string, dns: string}}
 */
function getConsoleConfig() {
    ensureAddon();
    return addon.getConsoleConfig();
}

/**
 * Console IP states enum
 */
const ConsoleIPState = {
    COMPLETED: 0,
    FAILED: 1,
    IDLE: 2,
    IN_PROGRESS: 3
};

module.exports = {
    // Core functions
    startTun2R,
    startTun2RWithConfigFile,
    stopTun2R,
    isServiceRunning,

    // Info functions
    getSDKVersion,
    getLastErrorMessage,

    // Stats functions
    getTrafficStats,
    getLastPingStats,

    // Ping functions
    runPing,
    stopPing,

    // Utility functions
    flushDNSCache,
    deleteService,
    getConsoleConfig,

    // Constants
    ErrorCodes,
    ConsoleIPState,

    // Platform info
    isAddonAvailable: () => addon !== null,
    getPlatform: () => os.platform()
};
