/* Windows x64 console acceleration example. See docs/CONSOLE_ACCELERATION.zh-CN.md. */
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "lingti_sdk.h"

static volatile LONG stop_requested = 0;

static BOOL WINAPI handle_console_signal(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT) {
        InterlockedExchange(&stop_requested, 1);
        return TRUE;
    }
    return FALSE;
}

static int stopping(void) {
    return InterlockedCompareExchange(&stop_requested, 0, 0) != 0;
}

static void print_sdk_error(const char *operation) {
    char *message = GetLastErrorMessage();
    fprintf(stderr, "%s: %s\n", operation, message ? message : "Unknown error");
    free(message);
}

static int has_value(const char *value) {
    return value != NULL && value[0] != '\0';
}

static int wait_for_console_config(void) {
    ULONGLONG deadline = GetTickCount64() + 30000;

    while (!stopping() && GetTickCount64() < deadline) {
        char *gateway = NULL, *mask = NULL, *ip = NULL, *dns = NULL;
        int state;
        int ready;

        if (!IsServiceRunning()) {
            print_sdk_error("Service stopped during startup");
            return 0;
        }

        state = GetConsoleConfig(&gateway, &mask, &ip, &dns);
        ready = state == 0 && has_value(gateway) && has_value(mask)
                && has_value(ip) && has_value(dns);
        if (ready) {
            puts("Enter these values on your game console:");
            printf("IP: %s\nMask: %s\nGateway: %s\nPrimary DNS: %s\n",
                   ip, mask, gateway, dns);
        }

        /* GetConsoleConfig allocates all four strings on every call. */
        free(gateway);
        free(mask);
        free(ip);
        free(dns);

        if (ready) return 1;
        if (state == 1) {
            print_sdk_error("Console IP configuration failed");
            return 0;
        }
        if (state < 0 || state > 3) {
            fprintf(stderr, "Unknown console IP state: %d\n", state);
            return 0;
        }
        Sleep(500);
    }

    if (!stopping()) {
        fprintf(stderr, "Console IP configuration timed out after 30 seconds.\n");
        print_sdk_error("Last SDK error");
    }
    return 0;
}

int main(int argc, char **argv) {
    int result;
    int exit_code = 0;
    int ready;
    char *version;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <encrypted_config.txt>\n", argv[0]);
        return 1;
    }
    if (!SetConsoleCtrlHandler(handle_console_signal, TRUE)) {
        fprintf(stderr, "Cannot register Ctrl+C handler.\n");
        return 1;
    }

    version = GetSDKVersion();
    printf("SDK version: %s\n", version ? version : "Unknown");
    free(version);

    result = StartTun2RWithConfigFile(argv[1]);
    if (result != LINGTI_SUCCESS) {
        fprintf(stderr, "Start returned %d\n", result);
        print_sdk_error("Cannot start service");
        return 1;
    }

    ready = wait_for_console_config();
    if (!ready && !stopping()) exit_code = 1;
    if (ready) {
        puts("Keep this process running while playing. Press Ctrl+C to stop.");
        while (!stopping()) {
            unsigned long long tx = 0, rx = 0;
            if (!IsServiceRunning()) {
                print_sdk_error("Service stopped unexpectedly");
                exit_code = 1;
                break;
            }
            GetTrafficStats(&tx, &rx, NULL, NULL);
            printf("TX: %llu bytes, RX: %llu bytes\n", tx, rx);
            Sleep(1000);
        }
    }

    if (IsServiceRunning() && StopTun2R() != LINGTI_SUCCESS) {
        print_sdk_error("Cannot stop service");
        exit_code = 1;
    }
    /* Allow shutdown to settle before unloading the DLL. */
    Sleep(2000);
    if (IsServiceRunning()) {
        fprintf(stderr, "Service still reports running after stop.\n");
        exit_code = 1;
    }
    puts("Restore your console's IP and DNS settings to Automatic, or its original values.");
    return exit_code;
}
