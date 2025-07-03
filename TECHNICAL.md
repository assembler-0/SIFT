# SIFT Technical Documentation

## Architecture Overview

### Core Components
- **Assembly Kernels** - Hand-optimized stress tests
- **C++ Framework** - Threading and orchestration  
- **System Monitor** - Real-time performance tracking
- **Logging System** - Result persistence and analysis

### Test Implementation

#### AVX Stress Test (`avx.asm`)
- **12-wave FPU torture** with dependency chains
- **Vector operations** using AVX/AVX2/FMA instructions
- **Cache-optimized** 256-byte buffer alignment
- **Prevents out-of-order execution** with data dependencies

#### Branch Prediction (`branch.asm`)
- **4 realistic patterns**: Gaming AI, Database, Compiler, Mixed
- **Unpredictable branches** to stress branch predictors
- **Real-world correlation** with application performance
- **Misprediction penalties** measured accurately

#### Cache Hierarchy (`cache.asm`)
- **L1 Test**: 32KB working set (cache-friendly)
- **L2 Test**: 512KB working set (L1 misses, L2 hits)
- **L3 Test**: 8MB working set (L1/L2 misses, L3 hits)
- **Memory Latency**: Random access (cache-hostile)

#### Memory Stress (`flood.asm`)
- **Cache flooding** with multiple access patterns
- **Rowhammer attacks** for DRAM integrity testing
- **Non-temporal stores** to bypass cache
- **Memory controller saturation**

#### AES Crypto (`aesENC.asm`, `aesDEC.asm`)
- **AES-256 XTS mode** with full key expansion
- **Parallel block processing** for maximum throughput
- **AES-NI instruction utilization**
- **Crypto accelerator stress testing**

## Performance Measurement

### Scoring Methodology
All tests use **standardized iterations/second**:
```cpp
auto start = std::chrono::high_resolution_clock::now();
// Execute test
auto end = std::chrono::high_resolution_clock::now();
double score = iterations / elapsed_seconds;
```

### Thread Affinity
Each worker thread is pinned to a specific CPU core:
```cpp
cpu_set_t cpuset;
CPU_ZERO(&cpuset);
CPU_SET(target_core, &cpuset);
pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
```

### Memory Management
- **Huge pages** for large allocations (1GB+ buffers)
- **Aligned allocations** for SIMD operations
- **NUMA-aware** memory placement

## Out-of-Order Execution Prevention

### Data Dependencies
```asm
mov r8, [r13 + rdx]    ; Random read
add r8, rax            ; Use the data (dependency!)
mov [r13 + rdx], r8    ; Write depends on previous read
```

### Memory Barriers
```asm
clflush [rdi]          ; Force cache flush
mfence                 ; Full memory barrier
```

### Compiler Barriers
```cpp
asm volatile("" : : "r"(data) : "memory");
```

## Real-World Correlation

### Gaming Performance
- **L1/L2 cache** → Frame rate consistency
- **Branch prediction** → AI/physics performance
- **AVX performance** → SIMD-optimized engines

### Database Performance  
- **L3 cache** → Query performance on large datasets
- **Memory latency** → Join operations
- **Branch prediction** → Conditional logic

### Rendering Performance
- **AVX/FMA** → Vector math operations
- **Cache hierarchy** → Texture streaming
- **Memory bandwidth** → Large scene processing

## System Integration

### Thermal Management
- **Real-time temperature monitoring**
- **Automatic throttling detection**
- **Thermal runaway prevention**

### Result Logging
```json
{
  "timestamp": "2024-12-19 15:30:45",
  "cpu": "AMD Ryzen 9 7950X", 
  "test": "AVX_Stress",
  "threads": 16,
  "scores": [1234567.89, ...],
  "average": 1240000.00
}
```

### Configuration System
- **INI-style config files**
- **Parameter validation**
- **Default value fallbacks**

## Build System

### Dependencies
- **CMake** ≥ 4.0
- **C++23** compiler (Clang++ ≥ v19)
- **NASM** for assembly compilation
- **Standard libraries** (libc++, libstdc++)

### Compilation Flags
```cmake
-O3 -march=native -mtune=native
-ffast-math -funroll-loops
-DNDEBUG
```

### Assembly Integration
```cmake
enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_FLAGS "-f elf64")
```

## Platform Support

### Linux (Primary)
- **Full feature support**
- **Huge page allocation**
- **Thread affinity control**
- **System monitoring**

### Windows (Partial)
- **Core tests functional**
- **Limited system integration**
- **No huge page support**

## Security Considerations

### Rowhammer Testing
- **Controlled memory access patterns**
- **No persistent damage**
- **User consent required**
- **Automatic cleanup**

### Privilege Requirements
- **User-level execution** (no root required)
- **Standard memory allocation**
- **CPU instruction access only**