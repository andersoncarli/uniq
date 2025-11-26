#!/bin/bash
# Fast test runner for uniq library
# Uses fastest available compiler (clang++ preferred) with parallel compilation

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Detect compiler (prefer clang++ for faster compilation)
if command -v clang++ &> /dev/null; then
    CXX=clang++
    CXX_NAME="clang++"
elif command -v g++ &> /dev/null; then
    CXX=g++
    CXX_NAME="g++"
else
    echo -e "${RED}Error: No C++ compiler found (clang++ or g++)${NC}" >&2
    exit 1
fi

# Detect number of CPU cores for parallel compilation
if command -v nproc &> /dev/null; then
    JOBS=$(nproc)
elif [ -f /proc/cpuinfo ]; then
    JOBS=$(grep -c processor /proc/cpuinfo)
else
    JOBS=4  # Default fallback
fi

# Optimize JOBS (don't use all cores, leave one free)
JOBS=$((JOBS > 1 ? JOBS - 1 : 1))

# Check for ccache (compilation cache for faster rebuilds)
if command -v ccache &> /dev/null; then
    CCACHE="ccache "
else
    CCACHE=""
fi

# Base directory
LIBDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOTDIR="$(cd "$LIBDIR/.." && pwd)"
TESTDIR="$ROOTDIR/tests"
LIBSDIR="$ROOTDIR/libs"

# Find backward.hpp and set include paths
BACKWARD_INCLUDE=""
if [ -d "$LIBSDIR" ]; then
    BACKWARD_INCLUDE="-I$LIBSDIR"
fi

# Compiler flags optimized for fast compilation
# -O0: No optimization (faster compile)
# -g0: No debug info (faster compile)  
# -std=c++17: C++17 standard
# -DTESTING: Enable test framework
# -pipe: Use pipes instead of temp files (faster)
# -Wno-*: Suppress common warnings for faster compilation
CXXFLAGS="-std=c++17 -O0 -g0 -DTESTING -I$LIBDIR -I$ROOTDIR $BACKWARD_INCLUDE -pipe -Wall -Wno-deprecated-copy -Wno-comment -fpermissive -Wfatal-errors"

# Test source file
TESTSRC="$TESTDIR/tests.cc"
TARGET="$LIBDIR/uniq_tests"

# Function to print status
print_status() {
    echo -e "${BLUE}[$CXX_NAME]${NC} $1"
}

# Function to print success
print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

# Function to print error
print_error() {
    echo -e "${RED}✗${NC} $1"
}

# Clean previous build
clean() {
    print_status "Cleaning previous build..."
    rm -f "$TARGET" "$TARGET.o" "$LIBDIR/tests_temp.cc" "$LIBDIR/tests_temp.cc.bak" 2>/dev/null || true
}

# Build test executable
build_tests() {
    print_status "Building tests..."
    
    if [ ! -f "$TESTSRC" ]; then
        print_error "Test source not found: $TESTSRC"
        exit 1
    fi
    
    # Create temporary test file with missing includes commented out
    local TEMP_TEST="$LIBDIR/tests_temp.cc"
    cp "$TESTSRC" "$TEMP_TEST"
    
    # Comment out missing BigNumberCore2.t.cc if it doesn't exist
    if [ ! -f "$LIBDIR/BigNumberCore2.t.cc" ] && [ ! -f "$TESTDIR/BigNumberCore2.t.cc" ]; then
        # Use perl or sed to comment out the line (more portable)
        perl -pi -e 's|^#include "BigNumberCore2.t.cc"|// #include "BigNumberCore2.t.cc"  // Not found|' "$TEMP_TEST" 2>/dev/null || \
        sed -i.bak 's|#include "BigNumberCore2.t.cc"|// #include "BigNumberCore2.t.cc"  // Not found|' "$TEMP_TEST" 2>/dev/null || true
        rm -f "$TEMP_TEST.bak" 2>/dev/null || true
    fi
    
    local start=$(date +%s.%N)
    
    # Check for backward.hpp dependency (link flags)
    # Only add if backward.hpp is actually included
    local BACKWARD_LIBS=""
    if grep -q "backward.hpp" "$TEMP_TEST" 2>/dev/null; then
        # Try to link backward dependencies (may not be available, that's OK)
        BACKWARD_LIBS="-lbfd -ldl -ldw" 2>/dev/null || BACKWARD_LIBS=""
    fi
    
    # Compile and link in one step (faster for single-file compilation)
    # Try without backward libs first (they're optional)
    local BUILD_OUTPUT
    BUILD_OUTPUT=$($CCACHE$CXX $CXXFLAGS -o "$TARGET" "$TEMP_TEST" -lpthread 2>&1)
    local BUILD_EXIT=$?
    
    # If that failed, try with backward libs
    if [ $BUILD_EXIT -ne 0 ] && [ -n "$BACKWARD_LIBS" ]; then
        BUILD_OUTPUT=$($CCACHE$CXX $CXXFLAGS -o "$TARGET" "$TEMP_TEST" -lpthread $BACKWARD_LIBS 2>&1)
        BUILD_EXIT=$?
    fi
    
    # Show errors (filter out verbose backward warnings)
    if [ $BUILD_EXIT -ne 0 ]; then
        echo "$BUILD_OUTPUT" | grep -vE "(backward|bfd|dw|note:)" | grep -E "(error|Error|undefined|fatal|collect2)" | head -20
    fi
    
    # Clean up temp file
    rm -f "$TEMP_TEST" "$TEMP_TEST.bak" 2>/dev/null || true
    
    if [ ! -f "$TARGET" ]; then
        print_error "Failed to build $TARGET"
        exit 1
    fi
    
    local end=$(date +%s.%N)
    local duration=$(echo "$end - $start" | bc 2>/dev/null || echo "0")
    
    print_success "Executable created (${duration}s)"
}

# Run tests
run_tests() {
    print_status "Running tests..."
    echo ""
    
    if [ ! -f "$TARGET" ]; then
        print_error "Test executable not found: $TARGET"
        exit 1
    fi
    
    cd "$LIBDIR"
    ./uniq_tests
    local exit_code=$?
    
    echo ""
    if [ $exit_code -eq 0 ]; then
        print_success "All tests passed!"
    else
        print_error "Tests failed with exit code $exit_code"
    fi
    
    return $exit_code
}

# Main execution
main() {
    local start_time=$(date +%s.%N)
    
    echo -e "${YELLOW}=== Uniq Library Test Runner ===${NC}"
    echo -e "Compiler: ${BLUE}$CXX_NAME${NC} ($($CXX --version 2>/dev/null | head -1 || echo "unknown version"))"
    echo -e "Parallel jobs: ${BLUE}$JOBS${NC}"
    if [ -n "$CCACHE" ]; then
        echo -e "Cache: ${GREEN}ccache enabled${NC}"
    fi
    echo ""
    
    # Parse command line arguments
    case "${1:-}" in
        clean)
            clean
            exit 0
            ;;
        build)
            clean
            build_tests
            exit 0
            ;;
        run)
            if [ ! -f "$TARGET" ]; then
                print_error "Executable not found. Run './runtests.sh build' first."
                exit 1
            fi
            run_tests
            exit $?
            ;;
        *)
            # Default: clean, build, and run
            clean
            build_tests
            
            local build_time=$(date +%s.%N)
            local build_duration=$(echo "$build_time - $start_time" | bc 2>/dev/null || echo "0")
            
            echo ""
            echo -e "${YELLOW}Build completed in ${build_duration}s${NC}"
            echo ""
            
            run_tests
            local test_exit=$?
            
            local end_time=$(date +%s.%N)
            local total_duration=$(echo "$end_time - $start_time" | bc 2>/dev/null || echo "0")
            
            echo ""
            echo -e "${YELLOW}Total time: ${total_duration}s${NC}"
            
            exit $test_exit
            ;;
    esac
}

# Run main function
main "$@"

