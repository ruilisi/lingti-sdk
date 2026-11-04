/**
 * Simple example of using the Lingti SDK
 */

const lingti = require('./index');

console.log('Lingti SDK Example\n');

// Helper function to format bytes
function formatBytes(bytes) {
    if (bytes === 0) return '0 B';
    const k = 1024;
    const sizes = ['B', 'KB', 'MB', 'GB', 'TB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}

// Check platform compatibility
console.log('Platform:', lingti.getPlatform());
console.log('Native addon available:', lingti.isAddonAvailable());

if (!lingti.isAddonAvailable()) {
    console.log('\nThis platform does not support the native addon.');
    console.log('Windows is required to run the Lingti SDK.');
    process.exit(1);
}

// Example usage (Windows only)
// To obtain encrypted_config: visit https://game.lingti.com/sdk
// Select your game (需要加速的游戏) and tunnel line (线路)
// encrypted_config is a base64 encoded string containing the encrypted configuration
const encrypted_config = ""

try {
    console.log('\nDeleting existing service if any...');
    lingti.deleteService();
    console.log('Service cleanup completed');

    console.log('\nStarting Lingti service...');
    const result = lingti.startTun2R(encrypted_config);

    if (result === lingti.ErrorCodes.SUCCESS) {
        console.log('Service started successfully');
        console.log('SDK Version:', lingti.getSDKVersion());
        console.log('Service running:', lingti.isServiceRunning());
        console.log('\nLogging traffic stats every second...\n');

        // Track previous stats for rate calculation
        let prevStats = { txBytes: 0, rxBytes: 0, txPkts: 0, rxPkts: 0 };

        // Log traffic stats every second
        const statsInterval = setInterval(() => {
            const stats = lingti.getTrafficStats();
            const timestamp = new Date().toLocaleTimeString();

            // Calculate delta (rate per second)
            const txBytesPerSec = stats.txBytes - prevStats.txBytes;
            const rxBytesPerSec = stats.rxBytes - prevStats.rxBytes;
            const txPktsPerSec = stats.txPkts - prevStats.txPkts;
            const rxPktsPerSec = stats.rxPkts - prevStats.rxPkts;

            console.log(`[${timestamp}] Total - TX: ${formatBytes(stats.txBytes)}, RX: ${formatBytes(stats.rxBytes)}, TX Pkts: ${stats.txPkts}, RX Pkts: ${stats.rxPkts}`);
            console.log(`           Rate  - TX: ${formatBytes(txBytesPerSec)}/s (${txPktsPerSec} pkt/s), RX: ${formatBytes(rxBytesPerSec)}/s (${rxPktsPerSec} pkt/s)\n`);

            // Update previous stats
            prevStats = { ...stats };
        }, 1000);

        // Stop after 2 hours (7200000 ms)
        setTimeout(() => {
            clearInterval(statsInterval);
            console.log('\n2 hours elapsed. Stopping service...');
            lingti.stopTun2R();
            console.log('Service stopped');
            process.exit(0);
        }, 2 * 60 * 60 * 1000);

    } else {
        console.error('Failed to start service');
        console.error('Error:', lingti.getLastErrorMessage());
        console.error('Error code:', result);
    }

} catch (error) {
    console.error('Error:', error.message);
}
