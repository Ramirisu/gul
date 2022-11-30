# Configure
cmake -B build -DGUL_BUILD_TESTS=ON -DGUL_ENABLE_CODECOV=ON

# Build
cmake --build build

# Test
cd build && ctest && cd ..

# Generate coverage trace file
lcov --capture --directory $(pwd)'/build' --output-file coverage.info
lcov --extract coverage.info $(pwd)'/include/gul/*' --output-file coverage.info

# Generate coverage report in html
genhtml -o coverage_report --legend --title "gul" --prefix=./ coverage.info
