/**
 * Lingti SDK - Simple Example
 *
 * This example demonstrates basic usage of the Lingti SDK.
 * For detailed API documentation, see API.md
 *
 * Compilation:
 *   MinGW/GCC: gcc sdk_example.c -L. -llingti_sdk -o example.exe
 *   MSVC:      cl sdk_example.c lingti_sdk.lib
 */

#include <stdio.h>
#include <stdlib.h>
#include "lingti_sdk.h"

#ifdef _WIN32
#include <windows.h>
#define SLEEP(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP(ms) usleep((ms) * 1000)
#endif

int main() {
    printf("Lingti SDK Example\n");
    printf("==================\n\n");

    // Check SDK version
    char* version = GetSDKVersion();
    printf("SDK Version: %s\n", version);
    free(version);

    // Get device ID
    char* deviceId = GetDeviceID();
    printf("Device ID: %s\n\n", deviceId);
    free(deviceId);

    // Path to encrypted config file
    // For encryption details, see API.md
    char* configFile = "encrypted_config.txt";

    printf("Starting service from config file...\n");
    int result = StartTun2RWithConfigFile(configFile);

    if (result != 0) {
        char* error = GetLastErrorMessage();
        printf("Failed to start service (code %d): %s\n", result, error);
        free(error);
        return 1;
    }

    printf("Service started successfully!\n\n");

    // Check service status
    if (IsServiceRunning()) {
        printf("Service status: RUNNING\n\n");
    }

    // Start ping monitoring with 5 second interval (5000ms)
    int result2 = RunPing(5000);
    if (result2 == 0) {
        printf("Ping monitoring started (5s interval)\n");
    } else {
        printf("Failed to start ping (code %d)\n", result2);
    }

    // Monitor traffic for 3600 seconds
    printf("Monitoring traffic for 3600 seconds...\n");
    printf("Press Ctrl+C to stop early\n\n");

    for (int i = 0; i < 3600; i++) {
        unsigned long long txBytes, rxBytes;
        int64_t router, takeoff, landing;
        double udpLoss;
        GetTrafficStats(&txBytes, &rxBytes, NULL, NULL);
        GetLastPingStats(&router, &takeoff, &landing, &udpLoss);

        printf("\r[%02d/%02d] router: %lld | takeoff: %lld | landing: %lld | UDP loss: %.2f%%\n",
               i + 1, 3600, router, takeoff, landing, udpLoss);
        printf("\r[%02d/%02d] TX: %llu bytes | RX: %llu bytes",
               i + 1, 3600, txBytes, rxBytes);

        SLEEP(1000);
    }

    printf("\n\n");

    
    // Stop when done
    if (StopPing() == 0) {
        printf("Ping monitoring stopped\n");
    }

    // Stop the service
    printf("Stopping service...\n");
    result = StopTun2R();

    if (result == 0) {
        printf("Service stopped successfully!\n");
    } else {
        char* error = GetLastErrorMessage();
        printf("Failed to stop service (code %d): %s\n", result, error);
        free(error);
    }

    printf("\nExample completed. See API.md for detailed documentation.\n");

    return 0;
}
