#!/bin/bash
# Build script for SDK example (cross-compile for Windows)

set -e

echo "========================================"
echo "Building Lingti SDK Example"
echo "========================================"
echo ""

# For cross-compilation from Linux/macOS
if command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    echo "Using MinGW cross-compiler..."
    x86_64-w64-mingw32-gcc sdk_example.c -L.. -llingti_sdk -o sdk_example.exe
    echo ""
    echo "Build successful! Created: sdk_example.exe"
    echo ""
    echo "To run on Windows: sdk_example.exe"
elif command -v gcc &> /dev/null; then
    echo "Using native GCC compiler..."
    gcc sdk_example.c -L.. -llingti_sdk -o sdk_example
    echo ""
    echo "Build successful! Created: sdk_example"
    echo ""
    echo "To run: ./sdk_example"
else
    echo "ERROR: No suitable C compiler found!"
    echo "Please install gcc or mingw-w64"
    exit 1
fi
