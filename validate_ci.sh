#!/bin/bash

# Script to validate GitHub Actions workflow file
# Usage: ./validate_ci.sh

set -e

WORKFLOW_FILE=".github/workflows/ci.yml"

echo "=== GitHub Actions CI Validator ==="
echo ""

# Check if workflow file exists
if [ ! -f "$WORKFLOW_FILE" ]; then
    echo "Error: Workflow file not found: $WORKFLOW_FILE"
    exit 1
fi

echo "✓ Workflow file found: $WORKFLOW_FILE"
echo ""

# Basic YAML syntax check
echo "Checking YAML syntax..."
if command -v python3 &> /dev/null; then
    python3 -c "import yaml; yaml.safe_load(open('$WORKFLOW_FILE'))" 2>/dev/null
    echo "✓ YAML syntax is valid"
elif command -v yamllint &> /dev/null; then
    yamllint "$WORKFLOW_FILE"
    echo "✓ YAML syntax is valid"
else
    echo "⚠ No YAML validator found, skipping syntax check"
fi
echo ""

# Check for required sections
echo "Checking required workflow sections..."
REQUIRED_SECTIONS=("name:" "on:" "jobs:" "build-and-test:" "dependency-check:")

for section in "${REQUIRED_SECTIONS[@]}"; do
    if grep -q "$section" "$WORKFLOW_FILE"; then
        echo "✓ Found section: $section"
    else
        echo "✗ Missing section: $section"
    fi
done
echo ""

# Check OS versions
echo "Checking OS versions..."
if grep -q "macos-latest\|macos-15\|macos-14" "$WORKFLOW_FILE"; then
    echo "✓ Found macOS version"
else
    echo "⚠ macOS version not found or non-standard"
fi
echo ""

# Check that compiler matrix is simplified (good for CI speed)
echo "Checking compiler matrix simplification..."
if grep -q "compiler:" "$WORKFLOW_FILE"; then
    echo "⚠ Compiler matrix found (may slow down CI)"
else
    echo "✓ No compiler matrix (faster CI)"
fi
echo ""

# Check for ROOT exclusion
echo "Checking ROOT dependency status..."
if grep -q "ROOT" "$WORKFLOW_FILE"; then
    if grep -q "NO ROOT" "$WORKFLOW_FILE" || grep -q "excluded from CI" "$WORKFLOW_FILE"; then
        echo "✓ ROOT is properly excluded from CI"
    else
        echo "⚠ ROOT mentioned but exclusion note unclear"
    fi
else
    echo "✓ ROOT dependency not found in CI (good)"
fi
echo ""

# Check key dependencies
echo "Checking key dependencies..."
DEPENDENCIES=("eigen" "boost" "gsl" "yaml-cpp" "nlohmann-json" "tomlplusplus")

for dep in "${DEPENDENCIES[@]}"; do
    if grep -q "$dep" "$WORKFLOW_FILE"; then
        echo "✓ Found dependency: $dep"
    else
        echo "⚠ Missing dependency: $dep"
    fi
done

# Check for platform-specific dependencies
echo "Checking platform-specific dependencies..."
if grep -q "rapidyaml" "$WORKFLOW_FILE"; then
    echo "✓ Found rapidyaml (macOS only)"
else
    echo "⚠ Missing rapidyaml configuration"
fi
if grep -q "googletest" "$WORKFLOW_FILE"; then
    echo "✓ Found googletest (macOS testing)"
else
    echo "⚠ Missing googletest configuration"
fi
echo ""

echo "=== Validation Summary ==="
echo "Basic checks completed. For comprehensive validation, use:"
echo "  - GitHub Actions syntax checker: https://lint.github-actions.com/"
echo "  - Or push to GitHub and check the Actions tab"
echo ""
echo "Next steps:"
echo "1. Run ./setup_ci_badge.sh to update README badge"
echo "2. Commit and push: git add .github/workflows/ci.yml"
echo "3. Check Actions tab on GitHub"
