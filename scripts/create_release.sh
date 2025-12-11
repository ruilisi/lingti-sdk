#!/bin/bash

# Script to create a GitHub release with version from lingti_sdk.h

# Read version from line 7 of lingti_sdk.h
# Expected format: " * Version: 1.4.4"
VERSION=$(sed -n '7p' lingti_sdk.h | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')

if [ -z "$VERSION" ]; then
    echo "Error: Could not extract version from lingti_sdk.h line 7"
    exit 1
fi

echo "Creating release for version: v$VERSION"

# Check if required files exist
if [ ! -f "lingti_sdk.dll" ]; then
    echo "Error: lingti_sdk.dll not found"
    exit 1
fi

if [ ! -f "lingti_sdk.lib" ]; then
    echo "Error: lingti_sdk.lib not found"
    exit 1
fi

# Create GitHub release
gh release create "v$VERSION" \
    lingti_sdk.dll \
    lingti_sdk.lib \
    --title "v$VERSION" \
    --notes "Release v$VERSION

## Files included:
- \`lingti_sdk.dll\` - Main SDK library
- \`lingti_sdk.lib\` - Import library for linking

See [README.md](https://github.com/ruilisi/lingti-sdk/blob/main/README.md) for installation and usage instructions."

if [ $? -eq 0 ]; then
    echo "✓ Release v$VERSION created successfully!"
else
    echo "✗ Failed to create release"
    exit 1
fi
