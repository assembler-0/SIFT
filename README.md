# SIFT (System Intensive Function Tester) 

> ⚠️ **WARNING: POTENTIAL HARDWARE DAMAGE**  
> This tool can cause system instability, crashes, data loss, and permanent hardware damage from overheating.  
> **Requirements:** Adequate compute power, proper cooling.

## ✅ Current version

![Version](https://img.shields.io/badge/Current%20Version-v0.9.0-blue)

## 🐧 Compatibility
![Linux](https://img.shields.io/badge/Compatibility-Linux-important)


## 💥 Features

| Module | Target | Description |
|--------|--------|-------------|
| **Integer Arithmetic** (`3np1.asm`/`primes.asm`) | ALUs, Branch Prediction | Collatz conjecture & prime factorization |
| **AES Encryption/Decryption** (`aesENC.asm`/`aesDEC.asm`)| Crypto Accelerators | AES-256 XTS mode with key expansion |
| **AVX/FMA Floating-Point** (`avx.asm`) | Vector Units | 12-wave FPU torture with dependency chains |
| **Branch Prediction** (`branch.asm`) | Branch Predictors | Real-world patterns: gaming, database, compiler |
| **CPU Rendering** (`render.asm`) | Mathematical Units |  polynomial & trigonometric  |
| **Disk I/O Stress** (`diskWrite.asm`) | Storage Subsystem | Multi-pattern 16GB write cycles |
| **Memory Flooding** (`flood.asm`) | DRAM & Cache Integrity | L1/L2/L3 cache hierarchy + rowhammer attacks |
| **LZMA Compression** (`lzma.module.cpp`) | CPU Compression | Multi-threaded LZMA level 9 + DEFLATE |
| **SHA-256 Hashing** (`sha256.asm`) | Crypto Extensions | SHA-NI instruction with multiple rounds |
| **System Monitor** (`systemMonitor.manage.cpp`) | Real-time Monitoring | ImGui interface with thermal detection |

## 🎯 Test Categories

### **🔥 Tests** (Maximum CPU Saturation)
- **`avx`** - AVX/FMA vector unit destruction
- **`render`** - Mathematical mode
- **`mem`** - Memory controller + rowhammer attacks
- **`full`** - All tests combined

### **🎮 Real-World Tests** (Application Performance Prediction)
- **`branch`** - Branch prediction with realistic patterns
- **`3np1`** - Integer arithmetic with unpredictable workloads
- **`primes`** - Complex mathematical algorithms
- **`lzma`** - CPU compression workloads

### **🛡️ Security Tests** (Crypto Performance)
- **`aesenc/aesdec`** - AES encryption/decryption
- **`sha`** - SHA-256 hashing performance

### **💾 I/O Tests** (System Integration)
- **`disk`** - Storage subsystem stress testing

## 🚀 Versions

| Version | Description                                       |
|---------|---------------------------------------------------|
| `SIFT` (Full) ![Full Version](https://img.shields.io/badge/Version-Full-red) | Complete system torture with real-time monitoring |
| `SIFTCLI` ![CLI Version](https://img.shields.io/badge/Version-CLI-orange) | Command-line only (deprecated)                    |
| `SIFTLIB` ![Library Version](https://img.shields.io/badge/Version-Library-blue) | Static library for integration                    |

## 📊 Scoring System

All tests use **standardized scoring**: `iterations/second`
- Higher scores = better performance
- Consistent across all test modules
- Real-time monitoring with thermal throttling detection

## 📚 Documentation

| Guide | Description |
|-------|-------------|
| **[📖 USAGE.md](USAGE.md)** | Complete user guide - how to run tests and interpret results |
| **[🔧 BUILD.md](BUILD.md)** | Build instructions for all platforms and configurations |
| **[⚙️ PRESETS.md](PRESETS.md)** | Configuration file guide - create custom test presets |
| **[🔬 TECHNICAL.md](TECHNICAL.md)** | Technical documentation - architecture and implementation details |

## 🚀 Quick Start

```bash
# 1. Build SIFT
git clone <repository>
cd SIFT
mkdir build && cd build
cmake ..
make -j$(nproc)

# 2. Run interactive mode
./sift

# 3. Or run a preset
./sift gaming.cfg
```

**📖 New to SIFT?** Start with **[USAGE.md](USAGE.md)** for complete instructions.

**🔧 Build issues?** Check **[BUILD.md](BUILD.md)** for platform-specific help.

## 🎮 Usage Examples

```bash
# Quick CPU torture
./sift
> avx
Iterations?: 1000
Lower bound?: 0.001
Upper bound?: 1000000

# Branch prediction testing
./sift  
> branch
Iterations?: 5000
Pattern (1=Gaming, 2=Database, 3=Compiler, 4=Mixed): 4

# Nuclear option (all tests)
./sift
> full
Intensity (1 = default): 2
```

## 🏆 Honorable Mentions

Special thanks to the original project, ESST, which inspired the structure and intensity of this enhanced version. (I just make a fork because esst was not portable due to ROCm)

## ⚡ Performance Notes

- **Real-world correlation**: Unlike synthetic benchmarks, SIFT tests predict actual application performance
- **Thermal awareness**: Automatically detects and reports CPU throttling
- **Multi-threaded**: Scales across all available CPU cores
- **Assembly-optimized**: Hand-tuned assembly for maximum hardware stress

## 📜 License

![GitHub license](https://img.shields.io/badge/license-MIT-green)

## ⚠️ Disclaimer

> This software comes with absolutely no warranty. Use at your own risk.  
> Monitor temperatures and ensure adequate cooling before running intensive tests.  
> The authors are not responsible for any hardware damage or system instability.

---

**SIFT v0.9.0** - *Don't sue me* 🔥💀