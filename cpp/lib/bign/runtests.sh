#!/bin/bash
# Fast test runner for BigNumber library
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

# Compiler flags optimized for fast compilation
# -O0: No optimization (faster compile)
# -g0: No debug info (faster compile)  
# -std=c++17: C++17 standard
# -DTESTING: Enable test framework
# -pipe: Use pipes instead of temp files (faster)
# -Wno-*: Suppress common warnings for faster compilation
CXXFLAGS="-std=c++17 -O0 -g0 -DTESTING -I.. -I../.. -pipe -Wall -Wno-deprecated-copy -Wno-comment"

# Source files
SRCDIR="."
SOURCES=(
    "BigNumberNaive.cpp"
    "BigNumberCore.cpp"
    "tests.cc"
)

OBJECTS=(
    "BigNumberNaive.o"
    "BigNumberCore.o"
    "tests.o"
)

TARGET="bign_tests"

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
    rm -f "${OBJECTS[@]}" "$TARGET" 2>/dev/null || true
}

# Compile single object file
compile_object() {
    local src=$1
    local obj=$2
    local deps=$3
    
    # Suppress warnings to stdout, show errors
    $CCACHE$CXX $CXXFLAGS -c "$src" -o "$obj" $deps 2>&1 | grep -E "(error|Error)" || true
}

# Build all objects in parallel
build_objects() {
    print_status "Building objects with $JOBS parallel jobs..."
    
    local start=$(date +%s.%N)
    
    # Compile all objects in parallel
    (
        compile_object "BigNumberNaive.cpp" "BigNumberNaive.o" "-MMD -MP" &
        compile_object "BigNumberCore.cpp" "BigNumberCore.o" "-MMD -MP" &
        compile_object "tests.cc" "tests.o" "-MMD -MP" &
        wait
    )
    
    # Check if all compilations succeeded
    for obj in "${OBJECTS[@]}"; do
        if [ ! -f "$obj" ]; then
            print_error "Failed to build $obj"
            exit 1
        fi
    done
    
    local end=$(date +%s.%N)
    local duration=$(echo "$end - $start" | bc 2>/dev/null || echo "0")
    
    print_success "All objects built (${duration}s)"
}

# Link executable
link_executable() {
    print_status "Linking $TARGET..."
    local start=$(date +%s.%N)
    
    $CCACHE$CXX $CXXFLAGS -o "$TARGET" "${OBJECTS[@]}" -lpthread 2>&1 | grep -E "(error|Error)" || true
    
    if [ ! -f "$TARGET" ]; then
        print_error "Failed to link $TARGET"
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
    
    ./"$TARGET"
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
    
    echo -e "${YELLOW}=== BigNumber Test Runner ===${NC}"
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
            build_objects
            link_executable
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
            build_objects
            link_executable
            
            local build_time=$(date +%s.%N)
            local build_duration=$(echo "$build_time - $start_time" | bc)
            
            echo ""
            echo -e "${YELLOW}Build completed in ${build_duration}s${NC}"
            echo ""
            
            run_tests
            local test_exit=$?
            
            local end_time=$(date +%s.%N)
            local total_duration=$(echo "$end_time - $start_time" | bc)
            
            echo ""
            echo -e "${YELLOW}Total time: ${total_duration}s${NC}"
            
            exit $test_exit
            ;;
    esac
}

# Run main function
main "$@"

