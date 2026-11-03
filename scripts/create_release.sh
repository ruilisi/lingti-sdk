#!/bin/bash

set -euo pipefail
cd "$(dirname "$0")/.."
VERSION=$(node -p "require('./node-addon/package.json').version")
bash node-addon/scripts/publish.sh "$VERSION" --dry-run

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
