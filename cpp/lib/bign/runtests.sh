#!/bin/bash
# Run all bign tests

set -e

# Prefer clang++ if available, fallback to g++
if command -v clang++ &> /dev/null; then
    CXX=clang++
else
    CXX=g++
fi

CXXFLAGS="-std=c++17 -I. -I.. -I../.. -g -Wall -Wextra"
LDFLAGS=""

# Build directory
BUILD_DIR=".test_build"
mkdir -p "$BUILD_DIR"

echo "Compiling test framework and implementation files..."

# Compile implementation files
IMPL_FILES=(
  "BigNumberNaive.cpp"
  "BigNumberCore.cpp"
)

for impl in "${IMPL_FILES[@]}"; do
  if [ -f "$impl" ]; then
    echo "  Compiling $impl..."
    $CXX $CXXFLAGS -c "$impl" -o "$BUILD_DIR/${impl%.cpp}.o" || {
      echo "Failed to compile $impl"
      exit 1
    }
  fi
done

echo ""
echo "Compiling tests..."
echo "========================================"

# Compile main test file
TEST_BINARY="$BUILD_DIR/bign_tests"
$CXX $CXXFLAGS -o "$TEST_BINARY" tests.cc \
  "$BUILD_DIR/BigNumberNaive.o" \
  "$BUILD_DIR/BigNumberCore.o" \
  $LDFLAGS || {
  echo "Failed to compile tests"
  exit 1
}

echo ""
echo "Running tests..."
echo "========================================"
echo ""

# Run tests
"$TEST_BINARY"
EXIT_CODE=$?

echo ""
if [ $EXIT_CODE -eq 0 ]; then
  echo "All tests passed!"
else
  echo "Some tests failed (exit code: $EXIT_CODE)"
fi

exit $EXIT_CODE

