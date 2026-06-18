#!/bin/bash

# Configuration
# The path to clang-tidy provided in the requirements
DEFAULT_CLANG_TIDY_PATH="$HOME/Applications/CLion.app/Contents/bin/clang/mac/aarch64/bin/clang-tidy"
CLANG_TIDY_PATH="${CLANG_TIDY_PATH:-$DEFAULT_CLANG_TIDY_PATH}"
BUILD_DIR="${BUILD_DIR:-cmake-build-debug}"

# Resolve tilde in path if present
if [[ "$CLANG_TIDY_PATH" == "~"* ]]; then
    CLANG_TIDY_PATH="${HOME}${CLANG_TIDY_PATH:1}"
fi

# Find compile_commands.json
if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
    # Try searching for it if not in the specified BUILD_DIR
    SEARCH_RESULT=$(find . -maxdepth 3 -name compile_commands.json -not -path "*/.*" | head -n 1)
    if [ -n "$SEARCH_RESULT" ]; then
        BUILD_DIR=$(dirname "$SEARCH_RESULT")
        echo "Found compile_commands.json in $BUILD_DIR"
    else
        echo "Error: Could not find compile_commands.json."
        echo "Ensure you have configured the project with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON and built the project."
        exit 1
    fi
fi

# Get list of files to check (src, include, tests, tools)
# We use find and handle possible spaces in filenames safely
FILES=$(find src include tests tools \( -name "*.cpp" -o -name "*.hpp" \) -not -path "*/.*")

if [ -z "$FILES" ]; then
    echo "No source files found to check."
    exit 0
fi

echo "--- Running clang-format ---"
# Run clang-format in-place
echo "$FILES" | xargs clang-format -i
if [ $? -ne 0 ]; then
    echo "Error: clang-format failed."
    exit 1
fi

echo "--- Running clang-tidy ---"
# Check if the specified clang-tidy exists and is executable
if [ ! -x "$CLANG_TIDY_PATH" ]; then
    echo "Warning: clang-tidy not found at $CLANG_TIDY_PATH"
    echo "Attempting to use clang-tidy from PATH..."
    CLANG_TIDY_PATH="clang-tidy"
    
    if ! command -v clang-tidy &> /dev/null; then
        echo "Error: clang-tidy is not available on PATH either."
        exit 1
    fi
fi

# Run clang-tidy
# -p specifies the directory containing compile_commands.json
# Note: we don't use -fix here by default to avoid unexpected changes, 
# but the agent is expected to fix issues if reported.
echo "$FILES" | xargs "$CLANG_TIDY_PATH" -p "$BUILD_DIR" --quiet

TIDY_EXIT_CODE=$?

if [ $TIDY_EXIT_CODE -ne 0 ]; then
    echo "------------------------------------------------"
    echo "Quality checks FAILED (clang-tidy reported issues)"
    exit $TIDY_EXIT_CODE
else
    echo "------------------------------------------------"
    echo "Quality checks PASSED"
    exit 0
fi
