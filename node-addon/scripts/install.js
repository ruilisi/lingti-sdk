#!/usr/bin/env node

/**
 * Platform-aware install script
 * Only builds the native addon on Windows (where the DLL is available)
 */

const { execSync } = require('child_process');
const os = require('os');

const platform = os.platform();

console.log(`Detected platform: ${platform}`);

if (platform === 'win32') {
    console.log('Building native addon for Windows...');
    try {
        execSync('node-gyp rebuild', { stdio: 'inherit' });
        console.log('✓ Native addon built successfully');
    } catch (error) {
        console.error('✗ Failed to build native addon');
        console.error('Make sure you have Visual Studio C++ build tools installed');
        process.exit(1);
    }
} else {
    console.log('⚠ Skipping native addon build (Windows-only)');
    console.log('This package requires Windows to build and run the native addon.');
    console.log('Install completed without building the native module.');
}
