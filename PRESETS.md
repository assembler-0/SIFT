# SIFT Preset Configuration Guide

## Usage
```bash
./sift preset_name.cfg
```

## Basic Format
```ini
# Comments start with #
test_order = test1, test2, test3

# Test parameters
test_parameter = value
```

## Available Tests
| Test | Description |
|------|-------------|
| `avx` | AVX/FMA floating-point torture |
| `3np1` | Collatz conjecture integer math |
| `mem` | Memory stress + rowhammer |
| `branch` | Branch prediction patterns |
| `cache` | Cache hierarchy (L1/L2/L3/Latency) |

## Test Parameters

### AVX Test
```ini
avx_iterations = 200000
avx_lower = 0.0001
avx_upper = 1000000000000000
```

### 3n+1 Test
```ini
3np1_iterations = 20000000
3np1_lower = 1
3np1_upper = 1000000000000000
```

### Memory Test
```ini
mem_iterations = 20
```

### Branch Test
```ini
branch_iterations = 5000000000
branch_pattern = 4    # 1=Gaming, 2=Database, 3=Compiler, 4=Mixed
```

### Cache Test
```ini
cache_iterations = 5000
```

## Example Presets

### Gaming Performance
```ini
# Focus on gaming-relevant tests
test_order = cache, branch, avx, mem

cache_iterations = 5000
branch_iterations = 1000000
branch_pattern = 1
avx_iterations = 50000
avx_lower = 0.0001
avx_upper = 1000000
mem_iterations = 10
```

### CPU Torture
```ini
# Maximum CPU stress
test_order = mem, avx, 3np1, branch

mem_iterations = 20
avx_iterations = 200000
avx_lower = 0.0001
avx_upper = 1000000000000000
3np1_iterations = 20000000
3np1_lower = 1
3np1_upper = 1000000000000000
branch_iterations = 5000000000
branch_pattern = 4
```

### Quick Test
```ini
# Fast validation
test_order = cache, branch

cache_iterations = 1000
branch_iterations = 100000
branch_pattern = 4
```

## Notes
- **Missing parameters** use recommended defaults
- **Invalid tests** are skipped with warning
- **Results logged** to `results.log` as usual
- **Test order** determines execution sequence
- **Comments** ignored (lines starting with #)