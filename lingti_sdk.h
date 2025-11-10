/**
 * Lingti SDK - Network Tunneling Service
 *
 * This SDK provides network tunneling capabilities for game traffic routing.
 *
 * Copyright (c) 2025 Ruilisi
 * Version: 1.0.0
 */

#ifndef LINGTI_SDK_H
#define LINGTI_SDK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error Codes
 */
#define LINGTI_SUCCESS           0   // Operation successful
#define LINGTI_ERR_NULL_CONFIG  -1   // Invalid/null configuration pointer
#define LINGTI_ERR_JSON_PARSE   -2   // JSON parsing error
#define LINGTI_ERR_ALREADY_RUN  -3   // Service already running
#define LINGTI_ERR_LOAD_CONFIG  -4   // Failed to load config file
#define LINGTI_ERR_NOT_RUNNING  -1   // Service not running (for Stop)

/**
 * Start the TUN2R service with JSON configuration
 *
 * The configuration must be a valid JSON string containing the game configuration.
 * Example JSON:
 * {
 *   "Mode": "tun_switch",
 *   "Server": "your-server.com:port",
 *   "Token": "your-token",
 *   "LogLevel": "info",
 *   "GameExes": ["game.exe"],
 *   "GameID": "YOUR_GAME"
 * }
 *
 * @param configJSON - JSON string containing configuration
 * @return 0 on success, negative error code on failure:
 *         LINGTI_ERR_NULL_CONFIG: Invalid config pointer
 *         LINGTI_ERR_JSON_PARSE: JSON parse error
 *         LINGTI_ERR_ALREADY_RUN: Service already running
 *
 * Note: This function starts the service asynchronously in a background thread.
 *       Use IsServiceRunning() to check status.
 */
int StartTun2R(const char* configJSON);

/**
 * Start the TUN2R service using the default config file
 *
 * This function loads configuration from the default config.json file
 * in the executable directory.
 *
 * @param configPath - Path to configuration file (can be NULL for default)
 * @return 0 on success, negative error code on failure
 *
 * Note: This is a convenience function that wraps StartTun2R()
 */
int StartTun2RWithConfigFile(const char* configPath);

/**
 * Stop the TUN2R service gracefully
 *
 * This function initiates a graceful shutdown of the service.
 * It will cleanup all resources and restore network settings.
 *
 * @return 0 on success, LINGTI_ERR_NOT_RUNNING if not running
 *
 * Note: The function returns immediately but cleanup continues asynchronously.
 *       Wait a few seconds before terminating your application.
 */
int StopTun2R(void);

/**
 * Check if the service is currently running
 *
 * @return 1 if running, 0 if not running
 */
int IsServiceRunning(void);

/**
 * Get the SDK version string
 *
 * @return Version string (e.g., "1.0.0")
 *
 * Note: Caller must call FreeString() to release the returned string.
 */
char* GetSDKVersion(void);

/**
 * Get the last error message
 *
 * @return Error message string, or "No error" if no error occurred
 *
 * Note: Caller must call FreeString() to release the returned string.
 */
char* GetLastErrorMessage(void);

/**
 * Free a string allocated by the SDK
 *
 * This function must be called to free any string returned by:
 * - GetSDKVersion()
 * - GetLastErrorMessage()
 *
 * @param s - String pointer to free
 *
 * Note: GetLastError was renamed to GetLastErrorMessage to avoid conflict
 * with the Windows API GetLastError() function.
 */
void FreeString(char* s);

/**
 * Get current traffic statistics
 *
 * @param txBytes - Pointer to receive total transmitted bytes (can be NULL)
 * @param rxBytes - Pointer to receive total received bytes (can be NULL)
 * @param txPkts - Pointer to receive total transmitted packets (can be NULL)
 * @param rxPkts - Pointer to receive total received packets (can be NULL)
 *
 * Example:
 *   uint64_t tx, rx;
 *   GetTrafficStats(&tx, &rx, NULL, NULL);
 *   printf("TX: %llu bytes, RX: %llu bytes\n", tx, rx);
 */
void GetTrafficStats(unsigned long long* txBytes,
                     unsigned long long* rxBytes,
                     unsigned long long* txPkts,
                     unsigned long long* rxPkts);

/**
 * Flush the DNS cache
 *
 * This function flushes the local DNS cache to force fresh DNS lookups.
 *
 * @return 0 on success
 */
int FlushDNSCache(void);

#ifdef __cplusplus
}
#endif

#endif // LINGTI_SDK_H
