#!/bin/bash

# Script to setup GitHub Actions badge in README
# Usage: ./setup_github_badge.sh <username> <repo>

set -e

USERNAME=${1:-$(git config github.user)}
REPO=${2:-$(basename -s .git $(git config --get remote.origin.url))}

if [ -z "$USERNAME" ] || [ -z "$REPO" ]; then
    echo "Usage: $0 <github_username> <repo_name>"
    echo "Example: $0 johndoe cxfunc"
    exit 1
fi

echo "Setting up GitHub Actions badge for: $USERNAME/$REPO"

# Update README.md with correct badge URL
sed -i.bak "s|github.com/YOUR_USERNAME/YOUR_REPO|github.com/$USERNAME/$REPO|g" README.md
rm README.md.bak

echo "✓ GitHub Actions badge updated in README.md"
echo "✓ Badge URL: https://github.com/$USERNAME/$REPO/actions/workflows/ci.yml/badge.svg"
