#!/usr/bin/env node

/**
 * Script to prepare README files for npm publication
 * Converts relative links to absolute GitHub URLs for proper rendering on npmjs.com
 */

const fs = require("fs");
const path = require("path");

// Configuration
const GITHUB_REPO = "https://github.com/ruilisi/lingti-sdk";
const GITHUB_BRANCH = "main";
const REPO_ROOT = path.resolve(__dirname, "../..");

/**
 * Transform relative URLs to absolute GitHub URLs
 * @param {string} content - The markdown content
 * @param {string} filename - The filename being processed
 * @returns {string} - Transformed content
 */
function transformReadme(content, filename) {
  let transformed = content;

  // 1. Transform relative markdown links like [简体中文](README.zh-CN.md) or [English](README.md)
  // Match: [text](README*.md)
  transformed = transformed.replace(
    /\[([^\]]+)\]\((README[^\)]*\.md)\)/g,
    (match, text, link) => {
      // Convert to full GitHub raw URL for better npm compatibility
      const fullUrl = `${GITHUB_REPO}/blob/${GITHUB_BRANCH}/${link}`;
      return `[${text}](${fullUrl})`;
    },
  );

  // 2. Transform relative image links like docs/assets/image.png
  // Match: src="docs/assets/..." or (docs/assets/...)
  transformed = transformed.replace(
    /(<img[^>]+src="|!?\[[^\]]*\]\()([^")\s]+\.(png|jpg|jpeg|gif|svg))/g,
    (match, prefix, imagePath, ext) => {
      // Only transform relative paths (not already full URLs)
      if (
        !imagePath.startsWith("http://") &&
        !imagePath.startsWith("https://")
      ) {
        const fullUrl = `${GITHUB_REPO}/raw/${GITHUB_BRANCH}/${imagePath}`;
        return `${prefix}${fullUrl}`;
      }
      return match;
    },
  );

  // 3. Transform relative links in HTML <a> tags like href="docs/..."
  transformed = transformed.replace(
    /<a([^>]+)href="([^"]+)"/g,
    (match, attributes, href) => {
      // Only transform relative paths to docs/
      if (href.startsWith("docs/") || href.match(/^[^/]+\.md$/)) {
        const fullUrl = href.endsWith(".md")
          ? `${GITHUB_REPO}/blob/${GITHUB_BRANCH}/node-addon/${href}`
          : `${GITHUB_REPO}/raw/${GITHUB_BRANCH}/${href}`;
        return `<a${attributes}href="${fullUrl}"`;
      }
      return match;
    },
  );

  return transformed;
}

/**
 * Process a README file
 * @param {string} sourceFile - Source file path (in repo root)
 * @param {string} destFile - Destination file path (in node-addon)
 */
function processReadme(sourceFile, destFile) {
  const sourcePath = path.join(REPO_ROOT, sourceFile);
  const destPath = path.join(__dirname, "..", destFile);

  console.log(`Processing: ${sourceFile}`);

  if (!fs.existsSync(sourcePath)) {
    console.error(`  ✗ Source file not found: ${sourcePath}`);
    return false;
  }

  try {
    // Read source file
    const content = fs.readFileSync(sourcePath, "utf8");

    // Transform content
    const transformed = transformReadme(content, sourceFile);

    // Write to destination
    fs.writeFileSync(destPath, transformed, "utf8");

    console.log(`  ✓ Created: ${destFile}`);
    return true;
  } catch (error) {
    console.error(`  ✗ Error processing file: ${error.message}`);
    return false;
  }
}

/**
 * Main function
 */
function main() {
  console.log("Preparing README files for npm publication...\n");

  const files = [
    { source: "README.md", dest: "README.md" },
    { source: "README.zh-CN.md", dest: "README.zh-CN.md" },
  ];

  let success = true;

  files.forEach(({ source, dest }) => {
    if (!processReadme(source, dest)) {
      success = false;
    }
  });

  console.log("");
  if (success) {
    console.log("✓ All README files prepared successfully");
    console.log("");
    console.log("The following transformations were applied:");
    console.log("  1. Relative markdown links → Full GitHub URLs");
    console.log("  2. Relative image paths → Full GitHub raw URLs");
    console.log("  3. Relative HTML links → Full GitHub URLs");
    process.exit(0);
  } else {
    console.error("✗ Some files failed to process");
    process.exit(1);
  }
}

// Run if called directly
if (require.main === module) {
  main();
}

module.exports = { transformReadme };
