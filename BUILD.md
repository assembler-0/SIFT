# SIFT Build Instructions

## Prerequisites

### Required Dependencies
```bash
# Ubuntu/Debian
sudo apt install cmake clang nasm libc++-dev

# Fedora/RHEL
sudo dnf install cmake clang nasm libcxx-devel

# Arch Linux
sudo pacman -S cmake clang nasm libc++
```

### Optional Dependencies
```bash
# For system monitor GUI
sudo apt install libglfw3-dev libgl1-mesa-dev

# For LZMA compression tests
sudo apt install liblzma-dev

# For development
sudo apt install gdb valgrind
```

## Build Process

### Standard Build
```bash
git clone <repository>
cd SIFT
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Build Variants

#### CLI-Only Build (No GUI)
```bash
cmake -DBUILD_CLI=ON ..
make -j$(nproc)
```

#### Library Build
```bash
cmake -DBUILD_LIBRARY=ON ..
make -j$(nproc)
```

#### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

#### Release Build (Optimized)
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_CLI` | OFF | Build CLI-only version |
| `BUILD_LIBRARY` | OFF | Build static library |
| `ENABLE_GUI` | ON | Include system monitor |
| `ENABLE_LZMA` | ON | Include LZMA tests |
| `ENABLE_TESTS` | OFF | Build unit tests |

## Compiler Requirements

### Minimum Versions
- **Clang++** ≥ 19.0 (recommended)
- **GCC** ≥ 13.0 (supported)
- **MSVC** ≥ 19.30 (Windows only)

### Required Features
- **C++23** standard support
- **AVX/AVX2** instruction support
- **AES-NI** instruction support (optional)
- **SHA-NI** instruction support (optional)

## Platform-Specific Instructions

### Linux
```bash
# Install build tools
sudo apt install build-essential cmake

# Clone and build
git clone <repository>
cd SIFT
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Run
./sift
```

### Windows (MSYS2)
```bash
# Install MSYS2, then:
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-clang
pacman -S mingw-w64-x86_64-nasm

# Build
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make -j$(nproc)
```

### macOS
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install dependencies via Homebrew
brew install cmake nasm

# Build
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

## Troubleshooting

### Common Issues

#### "NASM not found"
```bash
# Install NASM assembler
sudo apt install nasm  # Ubuntu/Debian
sudo dnf install nasm  # Fedora
brew install nasm      # macOS
```

#### "C++23 not supported"
```bash
# Update compiler
sudo apt install clang-19  # Ubuntu
# Or use GCC 13+
sudo apt install gcc-13 g++-13
```

#### "AVX instructions not supported"
- Ensure CPU supports AVX/AVX2
- Check compiler flags: `-march=native`
- Verify target architecture

#### Assembly compilation errors
```bash
# Check NASM version
nasm -v  # Should be ≥ 2.14

# Verify 64-bit target
file /usr/bin/nasm
```

### Performance Issues

#### Slow compilation
```bash
# Use more parallel jobs
make -j$(nproc)

# Use faster linker
sudo apt install lld
export LDFLAGS="-fuse-ld=lld"
```

#### Runtime performance
```bash
# Ensure release build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Check CPU governor
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
# Should be "performance" for benchmarking
```

## Development Setup

### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DENABLE_TESTS=ON \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make -j$(nproc)
```

### Code Analysis
```bash
# Static analysis
clang-tidy src/*.cpp

# Memory checking
valgrind --tool=memcheck ./sift

# Performance profiling
perf record ./sift
perf report
```

### Testing
```bash
# Build with tests
cmake -DENABLE_TESTS=ON ..
make -j$(nproc)

# Run tests
ctest --verbose
```

## Installation

### System Installation
```bash
sudo make install
# Installs to /usr/local/bin/sift
```

### Custom Installation
```bash
cmake -DCMAKE_INSTALL_PREFIX=/opt/sift ..
make install
```

### Package Creation
```bash
# Create DEB package
cpack -G DEB

# Create RPM package  
cpack -G RPM

# Create ZIP archive
cpack -G ZIP
```