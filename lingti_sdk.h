/**
 * Lingti SDK - Network Tunneling Service
 *
 * This SDK provides network tunneling capabilities for game traffic routing.
 *
 * Copyright (c) 2025 Ruilisi
 * Version: 1.5.5
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
 * Start the TUN2R service with encrypted configuration
 *
 * @param encryptedConfigJSON - Base64 encoded encrypted configuration string
 * @return 0 on success, negative error code on failure:
 *         LINGTI_ERR_NULL_CONFIG (-1): Invalid/null config pointer
 *         LINGTI_ERR_JSON_PARSE (-2): Config decryption or JSON parse error
 *         LINGTI_ERR_ALREADY_RUN (-3): Service already running
 *
 * Note: Service starts asynchronously in a background thread.
 *       Use IsServiceRunning() to check status.
 */
int StartTun2R(const char* encryptedConfigJSON);

/**
 * Start the TUN2R service using an encrypted configuration file
 *
 * @param configPath - Path to encrypted config file (can be NULL for default "encrypted_config.txt")
 * @return 0 on success, negative error code on failure:
 *         LINGTI_ERR_NULL_CONFIG (-1): Invalid/null configuration
 *         LINGTI_ERR_JSON_PARSE (-2): Config decryption or JSON parse error
 *         LINGTI_ERR_ALREADY_RUN (-3): Service already running
 *         LINGTI_ERR_LOAD_CONFIG (-4): Failed to read config file
 */
int StartTun2RWithConfigFile(const char* configPath);

/**
 * Stop the TUN2R service gracefully
 *
 * @return 0 on success, LINGTI_ERR_NOT_RUNNING (-1) if service not running
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
 * @return Version string in semantic versioning format (e.g., "1.5.5")
 */
char* GetSDKVersion(void);

/**
 * Get the last error message
 *
 * @return Error message string, or "No error" if no error occurred
 */
char* GetLastErrorMessage(void);

/**
 * Get current traffic statistics
 *
 * @param txBytes - Transmitted bytes (can be NULL)
 * @param rxBytes - Received bytes (can be NULL)
 * @param txPkts - Transmitted packets (can be NULL)
 * @param rxPkts - Received packets (can be NULL)
 */
void GetTrafficStats(unsigned long long* txBytes,
                     unsigned long long* rxBytes,
                     unsigned long long* txPkts,
                     unsigned long long* rxPkts);

/**
 * Get the latest ping statistics
 *
 * @param router - Ping to router in milliseconds (can be NULL)
 * @param takeoff - Ping to takeoff server in milliseconds (can be NULL)
 * @param landing - Ping to landing server in milliseconds (can be NULL)
 */
void GetLastPingStats(long long* router,
                      long long* takeoff,
                      long long* landing);

/**
 * Start periodic ping monitoring
 *
 * @param intervalMilliSec - Ping interval in milliseconds. Minimum value is 100ms.
 *                           Values less than 100 will be clamped to 100ms.
 * @return 0 on success, negative error code on failure:
 *         -1: Invalid server configuration
 *         -2: Ping is already running
 */
int RunPing(int intervalMilliSec);

/**
 * Stop periodic ping monitoring
 *
 * @return 0 on success, -1 if ping is not running
 */
int StopPing(void);

/**
 * Flush the DNS cache
 *
 * @return 0 on success
 */
int FlushDNSCache(void);

/**
 * Get console configuration parameters
 *
 * @param gateway - Gateway address string (can be NULL)
 * @param mask - Subnet mask string (can be NULL)
 * @param ip - Console IP address string (can be NULL)
 * @param dns - DNS server string (can be NULL)
 * @return Console IP state:
 *         0 = completed (IP assignment successful)
 *         1 = failed (IP assignment failed)
 *         2 = idle (not started)
 *         3 = in_progress (IP assignment in progress)
 */
int GetConsoleConfig(char** gateway, char** mask, char** ip, char** dns);

/**
 * Get the unique device ID
 *
 * @return Device ID string
 */
char* GetDeviceID(void);

#ifdef __cplusplus
}
#endif

#endif // LINGTI_SDK_H
