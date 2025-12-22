#!/bin/bash

# Publish script for lingti-sdk npm package
# Usage: ./scripts/publish.sh <version>
# Example: ./scripts/publish.sh 1.4.6

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if version argument is provided
if [ -z "$1" ]; then
    echo -e "${RED}Error: Version number is required${NC}"
    echo "Usage: $0 <version>"
    echo "Example: $0 1.4.6"
    exit 1
fi

NEW_VERSION=$1

# Validate version format (x.y.z)
if ! [[ "$NEW_VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo -e "${RED}Error: Invalid version format${NC}"
    echo "Version must be in format x.y.z (e.g., 1.4.6)"
    exit 1
fi

# Get current version from package.json
CURRENT_VERSION=$(node -p "require('./package.json').version")

echo -e "${YELLOW}Current version: ${CURRENT_VERSION}${NC}"
echo -e "${YELLOW}New version: ${NEW_VERSION}${NC}"

# Confirm with user
read -p "Continue with version ${NEW_VERSION}? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Cancelled."
    exit 0
fi

# Check if on main branch
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [ "$CURRENT_BRANCH" != "main" ]; then
    echo -e "${YELLOW}Warning: You are on branch '${CURRENT_BRANCH}', not 'main'${NC}"
    read -p "Continue anyway? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Cancelled."
        exit 0
    fi
fi

steps=6
echo -e "${GREEN}Step 1/$steps: Updating package.json...${NC}"
# Update version in package.json using sed (macOS compatible)
sed -i '' "s/\"version\": \".*\"/\"version\": \"${NEW_VERSION}\"/" package.json

echo -e "${GREEN}Step 2/$steps: Updating README files in parent directory...${NC}"
# Update version in all README files in parent directory
sed -i '' "s/SDK Version: ${CURRENT_VERSION}/SDK Version: ${NEW_VERSION}/g" ../README.md 2>/dev/null || true
sed -i '' "s/SDK 版本：${CURRENT_VERSION}/SDK 版本：${NEW_VERSION}/g" ../README.zh-CN.md 2>/dev/null || true

# Update example version strings in README files
sed -i '' "s/\"${CURRENT_VERSION}\"/\"${NEW_VERSION}\"/g" ../README.md 2>/dev/null || true
sed -i '' "s/\"${CURRENT_VERSION}\"/\"${NEW_VERSION}\"/g" ../README.zh-CN.md 2>/dev/null || true

echo -e "${GREEN}Step 3/$steps: Creating git tag...${NC}"
git tag -a "v${NEW_VERSION}" -m "Release version ${NEW_VERSION}" || echo 0

echo -e "${GREEN}Step 4/$steps: Copying files from parent directory...${NC}"
# Copy the driver file from parent directory
if [ -f "../lingtiwfp64.sys" ]; then
    cp ../lingtiwfp64.sys .
    echo "✓ Copied lingtiwfp64.sys"
else
    echo -e "${RED}Warning: ../lingtiwfp64.sys not found${NC}"
fi

# Copy the header file from parent directory
if [ -f "../lingti_sdk.h" ]; then
    cp ../lingti_sdk.h .
    echo "✓ Copied lingti_sdk.h"
else
    echo -e "${RED}Warning: ../lingti_sdk.h not found${NC}"
fi

# Copy README files from parent directory
if [ -f "../README.md" ]; then
    cp ../README.md .
    echo "✓ Copied README.md"
else
    echo -e "${RED}Warning: ../README.md not found${NC}"
fi

if [ -f "../README.zh-CN.md" ]; then
    cp ../README.zh-CN.md .
    echo "✓ Copied README.zh-CN.md"
else
    echo -e "${RED}Warning: ../README.zh-CN.md not found${NC}"
fi

echo -e "${GREEN}Step 5/$steps: Checking npm authentication...${NC}"
if ! npm whoami &>/dev/null; then
    echo -e "${RED}Error: Not logged in to npm${NC}"
    echo "Please run: npm login"
    exit 1
fi

NPM_USER=$(npm whoami)
echo -e "Logged in as: ${NPM_USER}"

echo -e "${GREEN}Step 6/$steps: Publishing to npm...${NC}"
npm publish

echo ""
echo -e "${GREEN}✓ Successfully published lingti-sdk@${NEW_VERSION}${NC}"
echo ""
echo "Package available at: https://www.npmjs.com/package/lingti-sdk/v/${NEW_VERSION}"
echo "Git tag: v${NEW_VERSION}"
echo ""
echo -e "${YELLOW}Next steps:${NC}"
echo "- Verify the package: npm view lingti-sdk@${NEW_VERSION}"
echo "- Check the npm page: https://www.npmjs.com/package/lingti-sdk"
echo "- Update CHANGELOG.md if needed"
