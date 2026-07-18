#!/bin/bash

# Configuration
# The expected clang-tidy version
EXPECTED_CLANG_TIDY_VERSION="22"
# The path to clang-tidy provided in the requirements
DEFAULT_CLANG_TIDY_PATH="$HOME/Applications/CLion.app/Contents/bin/clang/mac/aarch64/bin/clang-tidy"
CLANG_TIDY_PATH="${CLANG_TIDY_PATH:-$DEFAULT_CLANG_TIDY_PATH}"

# macOS SDK path for clang-tidy
EXTRA_ARGS=""
if [[ "$OSTYPE" == "darwin"* ]]; then
    SDK_PATH=$(xcrun --sdk macosx --show-sdk-path 2>/dev/null)
    if [ -n "$SDK_PATH" ]; then
        # Add SDK path and architecture
        EXTRA_ARGS="--extra-arg=-isysroot --extra-arg=$SDK_PATH --extra-arg=-arch --extra-arg=arm64"
        # Explicitly add system include paths to help clang-tidy find standard headers
        # These paths are derived from 'c++ -E -x c++ - -v < /dev/null'
        EXTRA_ARGS="$EXTRA_ARGS --extra-arg=-isystem --extra-arg=$SDK_PATH/usr/include/c++/v1"
        EXTRA_ARGS="$EXTRA_ARGS --extra-arg=-isystem --extra-arg=/Library/Developer/CommandLineTools/usr/lib/clang/21/include"
        EXTRA_ARGS="$EXTRA_ARGS --extra-arg=-isystem --extra-arg=$SDK_PATH/usr/include"
        EXTRA_ARGS="$EXTRA_ARGS --extra-arg=-isystem --extra-arg=/Library/Developer/CommandLineTools/usr/include"
    fi
fi
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

# 1. Determine the clang-tidy executable to use
if [ -n "$CLANG_TIDY_PATH" ]; then
    # Use the path provided by the environment variable
    if [ ! -x "$CLANG_TIDY_PATH" ]; then
        # Handle tilde expansion manually if needed (though shell might have done it)
        if [[ "$CLANG_TIDY_PATH" == "~"* ]]; then
            CLANG_TIDY_PATH="${HOME}${CLANG_TIDY_PATH:1}"
        fi
        if [ ! -x "$CLANG_TIDY_PATH" ]; then
            echo "Error: Specified CLANG_TIDY_PATH does not exist or is not executable: $CLANG_TIDY_PATH"
            exit 1
        fi
    fi
elif [ -x "$DEFAULT_CLANG_TIDY_PATH" ]; then
    CLANG_TIDY_PATH="$DEFAULT_CLANG_TIDY_PATH"
elif command -v "clang-tidy-$EXPECTED_CLANG_TIDY_VERSION" &> /dev/null; then
    CLANG_TIDY_PATH="clang-tidy-$EXPECTED_CLANG_TIDY_VERSION"
elif command -v clang-tidy &> /dev/null; then
    CLANG_TIDY_PATH="clang-tidy"
else
    echo "Error: clang-tidy not found."
    exit 1
fi

echo "Using clang-tidy: $CLANG_TIDY_PATH"

# 2. Verify version for consistency
ACTUAL_VERSION=$("$CLANG_TIDY_PATH" --version | grep -oE "version [0-9]+" | cut -d' ' -f2)
if [ -z "$ACTUAL_VERSION" ]; then
    # Fallback for versions that don't match the regex (e.g. custom builds)
    ACTUAL_VERSION=$("$CLANG_TIDY_PATH" --version | head -n 2 | tail -n 1 | awk '{print $3}' | cut -d. -f1)
fi

if [ "$ACTUAL_VERSION" != "$EXPECTED_CLANG_TIDY_VERSION" ]; then
    echo "Warning: clang-tidy version mismatch!"
    echo "  Expected: $EXPECTED_CLANG_TIDY_VERSION"
    echo "  Actual:   $ACTUAL_VERSION"
    echo "This may cause inconsistent results between local and CI environments."
    # We don't exit here by default to allow users with newer/older versions to still run it,
    # but we warn them. If we want to be strict, we could exit 1.
fi

# Run clang-tidy
echo "$FILES" | xargs "$CLANG_TIDY_PATH" -p "$BUILD_DIR" $EXTRA_ARGS --quiet

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
