#!/bin/bash
# Publish the exact tarball checked against the corresponding GitHub Release.
set -euo pipefail
cd "$(dirname "$0")/.."
version=$(node -p "require('./package.json').version")
if [[ ${1:-} != "$version" ]]; then
    echo "Usage: $0 $version [--dry-run]" >&2
    exit 1
fi
mode=${2:-}
if [[ $# -gt 2 || ( -n "$mode" && "$mode" != --dry-run ) ]]; then
    echo "Unknown publish option" >&2
    exit 1
fi

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
npm pack --pack-destination "$tmp"
archive="$tmp/lingti-sdk-$version.tgz"
tar -xzf "$archive" -C "$tmp"
for file in lingti_sdk.dll lingti_sdk.lib lingti_sdk.h lingtiwfp64.sys; do
    cmp "../$file" "$tmp/package/$file"
done
if [[ "$mode" == --dry-run ]]; then
    echo "Verified local package for $version"
    exit 0
fi

gh release download "v$version" --repo ruilisi/lingti-sdk --dir "$tmp" \
    --pattern lingti_sdk.dll --pattern lingti_sdk.lib
for file in lingti_sdk.dll lingti_sdk.lib; do
    cmp "$tmp/$file" "$tmp/package/$file"
done
read -r -p "Publish the verified lingti-sdk@$version tarball? (y/n) " reply
[[ "$reply" =~ ^[Yy]$ ]] || exit 0
npm publish "$archive"
