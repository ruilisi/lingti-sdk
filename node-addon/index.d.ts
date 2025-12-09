/**
 * Lingti SDK - Node.js Native Addon TypeScript Definitions
 */

export interface TunnelConfig {
    Mode: string;
    Server: string;
    Token: string;
    LogLevel?: string;
    GameExes?: string[];
    GameID: string;
}

export interface TrafficStats {
    txBytes: number;
    rxBytes: number;
    txPkts: number;
    rxPkts: number;
}

export interface PingStats {
    router: number;
    takeoff: number;
    landing: number;
}

export interface ConsoleConfig {
    state: number;
    stateStr: 'completed' | 'failed' | 'idle' | 'in_progress';
    gateway: string;
    mask: string;
    ip: string;
    dns: string;
}

export enum ErrorCode {
    SUCCESS = 0,
    ERR_NULL_CONFIG = -1,
    ERR_JSON_PARSE = -2,
    ERR_ALREADY_RUN = -3,
    ERR_LOAD_CONFIG = -4,
    ERR_NOT_RUNNING = -1
}

export enum ConsoleIPState {
    COMPLETED = 0,
    FAILED = 1,
    IDLE = 2,
    IN_PROGRESS = 3
}

/**
 * Start the TUN2R service with JSON configuration
 * @param config - Configuration object or JSON string
 * @returns 0 on success, negative error code on failure
 */
export function startTun2R(config: TunnelConfig | string): number;

/**
 * Start the TUN2R service using a config file
 * @param configPath - Path to config file (optional)
 * @returns 0 on success, negative error code on failure
 */
export function startTun2RWithConfigFile(configPath?: string): number;

/**
 * Stop the TUN2R service gracefully
 * @returns 0 on success, negative error code on failure
 */
export function stopTun2R(): number;

/**
 * Check if the service is currently running
 * @returns true if running, false otherwise
 */
export function isServiceRunning(): boolean;

/**
 * Get the SDK version string
 * @returns Version string (e.g., "1.4.3")
 */
export function getSDKVersion(): string;

/**
 * Get the last error message
 * @returns Error message string
 */
export function getLastErrorMessage(): string;

/**
 * Get current traffic statistics
 * @returns Traffic statistics object
 */
export function getTrafficStats(): TrafficStats;

/**
 * Get the latest ping statistics
 * @returns Ping statistics object
 */
export function getLastPingStats(): PingStats;

/**
 * Start periodic ping monitoring
 * @returns 0 on success, negative error code on failure
 */
export function runPing(): number;

/**
 * Stop periodic ping monitoring
 * @returns 0 on success, negative error code on failure
 */
export function stopPing(): number;

/**
 * Flush the DNS cache
 * @returns 0 on success
 */
export function flushDNSCache(): number;

/**
 * Delete the lingtiwfp Windows service
 * @returns 0 on success
 */
export function deleteService(): number;

/**
 * Get console configuration parameters
 * @returns Console configuration object
 */
export function getConsoleConfig(): ConsoleConfig;

/**
 * Error codes from the SDK
 */
export const ErrorCodes: {
    SUCCESS: number;
    ERR_NULL_CONFIG: number;
    ERR_JSON_PARSE: number;
    ERR_ALREADY_RUN: number;
    ERR_LOAD_CONFIG: number;
    ERR_NOT_RUNNING: number;
};

/**
 * Check if the native addon is available on this platform
 * @returns true if addon is loaded, false otherwise (e.g., on non-Windows platforms)
 */
export function isAddonAvailable(): boolean;

/**
 * Get the current platform
 * @returns Platform string (e.g., 'win32', 'darwin', 'linux')
 */
export function getPlatform(): string;
