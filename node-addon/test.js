/**
 * Test file for Lingti SDK Node.js addon
 *
 * This demonstrates all available API functions.
 */

const lingti = require('./index');

console.log('=== Lingti SDK Test Suite ===\n');

// Check if addon is available
console.log('Platform:', lingti.getPlatform());
console.log('Addon available:', lingti.isAddonAvailable());

if (!lingti.isAddonAvailable()) {
    console.log('\n⚠ Native addon not available on this platform.');
    console.log('The Lingti SDK requires Windows to run.');
    console.log('\nAvailable constants:');
    console.log('- ErrorCodes:', lingti.ErrorCodes);
    console.log('- ConsoleIPState:', lingti.ConsoleIPState);
    console.log('\nTo run the full test suite, please execute on Windows after building the addon.');
    process.exit(0);
}

console.log('\n=== Running Full Test Suite ===\n');

// 1. Get SDK version
console.log('1. SDK Version:', lingti.getSDKVersion());

// 2. Check initial service status
console.log('2. Service running (initial):', lingti.isServiceRunning());

// 3. Get initial traffic stats
console.log('3. Initial traffic stats:', lingti.getTrafficStats());

// 4. Get initial ping stats
console.log('4. Initial ping stats:', lingti.getLastPingStats());

// 5. Get console config
console.log('5. Console config:', lingti.getConsoleConfig());

// 6. Start service with JSON config (example - adjust values as needed)
console.log('\n=== Testing Service Start ===');
const config = {
    Mode: "tun_switch",
    Server: "your-server.com:port",
    Token: "your-token",
    LogLevel: "info",
    GameExes: ["game.exe"],
    GameID: "TEST_GAME"
};

console.log('6. Starting service...');
const startResult = lingti.startTun2R(config);
console.log('   Start result:', startResult);
if (startResult !== 0) {
    console.log('   Error:', lingti.getLastErrorMessage());
    console.log('   Error codes:', lingti.ErrorCodes);
}

// 7. Check service status after start
setTimeout(() => {
    console.log('\n=== Service Status (after 2s) ===');
    console.log('7. Service running:', lingti.isServiceRunning());
    console.log('8. Traffic stats:', lingti.getTrafficStats());

    // 8. Start ping monitoring
    console.log('\n=== Testing Ping ===');
    const pingResult = lingti.runPing();
    console.log('9. Start ping result:', pingResult);

    setTimeout(() => {
        console.log('10. Ping stats (after 3s):', lingti.getLastPingStats());

        // 9. Stop ping
        console.log('11. Stop ping result:', lingti.stopPing());

        // 10. Flush DNS cache
        console.log('\n=== Testing DNS Flush ===');
        console.log('12. Flush DNS result:', lingti.flushDNSCache());

        // 11. Get final console config
        console.log('\n=== Final Console Config ===');
        console.log('13. Console config:', lingti.getConsoleConfig());

        // 12. Stop service
        console.log('\n=== Stopping Service ===');
        const stopResult = lingti.stopTun2R();
        console.log('14. Stop result:', stopResult);

        setTimeout(() => {
            console.log('15. Service running (after stop):', lingti.isServiceRunning());
            console.log('\n=== Test Complete ===');
        }, 1000);

    }, 3000);

}, 2000);

// Error code reference
console.log('\n=== Error Code Reference ===');
console.log('ErrorCodes:', lingti.ErrorCodes);
console.log('ConsoleIPState:', lingti.ConsoleIPState);
