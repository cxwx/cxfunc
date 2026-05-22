#!/bin/bash

# Quick setup script for CxFunc GitHub Actions
# This script helps you configure the GitHub Actions badge in README

set -e

echo "=== CxFunc GitHub Actions Setup ==="
echo ""
echo "This script will help you setup the GitHub Actions badge in your README.md"
echo ""

# Try to get GitHub username and repo name
GIT_REMOTE=$(git config --get remote.origin.url 2>/dev/null || echo "")

if [[ $GIT_REMOTE == *"github.com"* ]]; then
    # Extract username and repo from git remote URL
    if [[ $GIT_REMOTE == *"github.com:"* ]]; then
        # SSH format: git@github.com:username/repo.git
        GITHUB_PATH=$(echo $GIT_REMOTE | sed 's|.*github.com:||' | sed 's|\.git$||')
    else
        # HTTPS format: https://github.com/username/repo.git
        GITHUB_PATH=$(echo $GIT_REMOTE | sed 's|.*github.com/||' | sed 's|\.git$||')
    fi

    USERNAME=$(echo $GITHUB_PATH | cut -d'/' -f1)
    REPO=$(echo $GITHUB_PATH | cut -d'/' -f2)

    echo "Detected GitHub repository: $USERNAME/$REPO"
    echo ""
    read -p "Use these values? [Y/n] " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Nn]$ ]]; then
        # Update README.md
        sed -i.bak "s|github.com/YOUR_USERNAME/YOUR_REPO|github.com/$USERNAME/$REPO|g" README.md
        rm README.md.bak
        echo "✓ README.md updated with GitHub Actions badge"
        echo ""
        echo "Your CI badge will appear as:"
        echo "[![CI](https://github.com/$USERNAME/$REPO/actions/workflows/ci.yml/badge.svg)](https://github.com/$USERNAME/$REPO/actions/workflows/ci.yml)"
        echo ""
        echo "Next steps:"
        echo "1. Commit and push the .github/workflows/ci.yml file"
        echo "2. Check the Actions tab in your GitHub repository"
        echo "3. The badge will show the build status"
        exit 0
    fi
fi

# Manual input
echo "Please enter your GitHub information:"
read -p "GitHub username: " USERNAME
read -p "Repository name: " REPO

if [ -z "$USERNAME" ] || [ -z "$REPO" ]; then
    echo "Error: Username and repository name are required"
    exit 1
fi

# Update README.md
sed -i.bak "s|github.com/YOUR_USERNAME/YOUR_REPO|github.com/$USERNAME/$REPO|g" README.md
rm README.md.bak

echo "✓ README.md updated with GitHub Actions badge"
echo ""
echo "Your CI badge will appear as:"
echo "[![CI](https://github.com/$USERNAME/$REPO/actions/workflows/ci.yml/badge.svg)](https://github.com/$USERNAME/$REPO/actions/workflows/ci.yml)"
