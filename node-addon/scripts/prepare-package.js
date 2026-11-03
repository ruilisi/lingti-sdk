// Stage all native inputs from the same build before npm packs the addon.
const fs = require('fs');
const path = require('path');
const addon = path.resolve(__dirname, '..');
const root = path.dirname(addon);
const header = fs.readFileSync(path.join(root, 'lingti_sdk.h'), 'utf8');
const version = header.match(/^ \* Version: (\d+\.\d+\.\d+)$/m)?.[1];
if (version !== require('../package.json').version) {
  throw new Error('SDK header and package.json versions must match');
}

for (const name of ['lingti_sdk.dll', 'lingti_sdk.lib', 'lingti_sdk.h', 'lingtiwfp64.sys']) {
  const source = path.join(root, name);
  if (!fs.statSync(source).size) throw new Error('Empty SDK file: ' + source);
  fs.copyFileSync(source, path.join(addon, name));
}
