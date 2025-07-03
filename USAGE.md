# SIFT Usage Guide

## Quick Start

### Interactive Mode
```bash
./sift
```
Navigate the menu with number/letter keys.

### Preset Mode
```bash
./sift gaming.cfg
./sift full.cfg
```

## Test Categories

### 🔥 Torture Tests
- **[1] AVX Stress** - Vector unit destruction
- **[2] CPU Rendering** - Mathematical death mode  
- **[3] Memory Stress** - Cache flooding + rowhammer
- **[4] Full System** - Nuclear option (all tests)

### 🎮 Real-World Tests
- **[5] Branch Prediction** - Gaming/database patterns
- **[6] Cache Hierarchy** - L1/L2/L3/latency tests
- **[7] 3n+1 Collatz** - Integer arithmetic torture
- **[8] Prime Factorization** - Mathematical algorithms

### 🛡️ Security Tests
- **[9] AES Encryption** - Crypto accelerator stress
- **[A] AES Decryption** - Reverse crypto stress
- **[B] SHA-256 Hashing** - Hash function torture

### 💾 I/O Tests
- **[C] Disk Write** - Storage subsystem stress
- **[D] LZMA Compression** - CPU compression torture

## Understanding Results

### Score Format
All tests report **iterations per second (IPS)**:
- **GIPS** = Billion iterations/second
- **MIPS** = Million iterations/second  
- **KIPS** = Thousand iterations/second
- **IPS** = Raw iterations/second

### Performance Correlation
- **Higher scores** = Better performance
- **L1 > L2 > L3 > Memory** (cache hierarchy)
- **Consistent scores** = Stable system
- **Dropping scores** = Thermal throttling

## Safety Guidelines

### ⚠️ Before Testing
1. **Monitor temperatures** - Keep CPU under 85°C
2. **Ensure adequate cooling** - Stress tests generate heat
3. **Save work** - Tests may cause system instability
4. **Check power supply** - High CPU loads need stable power

### 🚨 Stop Testing If
- CPU temperature exceeds 90°C
- System becomes unresponsive
- Unusual noises from cooling
- Power supply issues

## Troubleshooting

### Low Performance
- **Thermal throttling** - Check temperatures
- **Power limits** - Check CPU power settings
- **Background processes** - Close unnecessary programs

### System Instability
- **Memory errors** - Run memory test first
- **Overclocking** - Return to stock settings
- **Power issues** - Check PSU capacity

### Test Failures
- **Segmentation faults** - Hardware instability
- **Infinite loops** - Kill process with Ctrl+C
- **No results** - Check system monitor logs

## Advanced Usage

### Custom Iterations
Use **[R] Recommendations** to see optimal values for each test.

### Result Analysis
- Check `results.log` for detailed performance data
- Compare scores over time to detect degradation
- Use multiple runs to verify consistency

### System Monitoring
- Real-time temperature/frequency monitoring
- Automatic thermal throttling detection
- Performance graphs during tests