/**
 * Lingti SDK - Network Tunneling Service
 *
 * This SDK provides network tunneling capabilities for game traffic routing.
 *
 * Copyright (c) 2025 Ruilisi
 * Version: 1.4.4
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
 * @return Version string (e.g., "1.4.4")
 *
 * Note: Caller must call free() to release the returned string.
 */
char* GetSDKVersion(void);

/**
 * Get the last error message
 *
 * @return Error message string, or "No error" if no error occurred
 *
 * Note: Caller must call free() to release the returned string.
 */
char* GetLastErrorMessage(void);

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
 * Get the latest ping statistics
 *
 * Retrieves the most recent ping measurements to various network endpoints.
 *
 * @param router - Pointer to receive ping time to router in milliseconds (can be NULL)
 * @param takeoff - Pointer to receive ping time to takeoff server in milliseconds (can be NULL)
 * @param landing - Pointer to receive ping time to landing server in milliseconds (can be NULL)
 *
 * Example:
 *   long long routerPing, takeoffPing, landingPing;
 *   GetLastPingStats(&routerPing, &takeoffPing, &landingPing);
 *   printf("Router: %lld ms, Takeoff: %lld ms, Landing: %lld ms\n",
 *          routerPing, takeoffPing, landingPing);
 *
 * Notes:
 *   - Router ping is to the local gateway
 *   - Takeoff ping is to the proxy entry server
 *   - Landing ping is to the proxy exit server
 */
void GetLastPingStats(long long* router,
                      long long* takeoff,
                      long long* landing);

/**
 * Start periodic ping monitoring
 *
 * Starts a background thread that periodically pings the server and measures latency.
 * Ping statistics can be retrieved using GetLastPingStats().
 *
 * @return 0 on success, negative error code on failure:
 *         -1: Invalid server pointer (NULL)
 *         -2: Ping is already running
 *
 * Example:
 *   int result = RunPing();
 *   if (result == 0) {
 *       printf("Ping monitoring started\n");
 *   }
 *
 * Notes:
 *   - Runs in background thread
 *   - Pings every 5 seconds by default
 *   - Call StopPing() to stop monitoring
 *   - Only one ping session can run at a time
 */
int RunPing();

/**
 * Stop periodic ping monitoring
 *
 * Stops the background ping monitoring thread started by RunPing().
 *
 * @return 0 on success, negative error code on failure:
 *         -1: Ping is not running
 *
 * Example:
 *   if (StopPing() == 0) {
 *       printf("Ping monitoring stopped\n");
 *   }
 *
 * Notes:
 *   - Safe to call even if already stopped
 *   - Thread stops gracefully
 */
int StopPing(void);

/**
 * Flush the DNS cache
 *
 * This function flushes the local DNS cache to force fresh DNS lookups.
 *
 * @return 0 on success
 */
int FlushDNSCache(void);

/**
 * Get console configuration parameters
 *
 * Retrieves the console mode network configuration including gateway, subnet mask, IP, and DNS.
 *
 * @param gateway - Pointer to receive gateway address string (can be NULL)
 * @param mask - Pointer to receive subnet mask string (can be NULL)
 * @param ip - Pointer to receive console IP address string (can be NULL)
 * @param dns - Pointer to receive DNS server string (can be NULL)
 * @return Console IP state:
 *         0 = completed (IP assignment successful)
 *         1 = failed (IP assignment failed)
 *         2 = idle (not started)
 *         3 = in_progress (IP assignment in progress)
 *
 * Example:
 *   char *gateway, *mask, *ip, *dns;
 *   int state = GetConsoleConfig(&gateway, &mask, &ip, &dns);
 *   printf("State: %d, Gateway: %s, Mask: %s, IP: %s, DNS: %s\n",
 *          state, gateway, mask, ip, dns);
 *   free(gateway);
 *   free(mask);
 *   free(ip);
 *   free(dns);
 *
 * Note: Caller must call free() on each returned string to avoid memory leaks.
 */
int GetConsoleConfig(char** gateway, char** mask, char** ip, char** dns);

/**
 * Get the unique device ID
 *
 * Returns the Windows Machine GUID which uniquely identifies this Windows installation.
 * The Machine GUID is read from the Windows registry at:
 * HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\MachineGuid
 *
 * @return Device ID string (Windows Machine GUID)
 *
 * Example:
 *   char* deviceId = GetDeviceID();
 *   printf("Device ID: %s\n", deviceId);
 *   free(deviceId);
 *
 * Note: Caller must call free() to release the returned string.
 */
char* GetDeviceID(void);

#ifdef __cplusplus
}
#endif

#endif // LINGTI_SDK_H
