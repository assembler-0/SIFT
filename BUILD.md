## 🧱 Build Instructions

### 🔹 Normal Build

```bash
mkdir build
cd build
cmake ..
make
```

### 🔹 Normal CLI Build

```bash
mkdir build
cd build
cmake .. -DBUILD_CLI
make
```


### 🔸 Library Build

```bash
mkdir buildLIB
cd buildLIB
cmake -DBUILD_LIBRARY=ON ..
make
```

> Tip: use any build system of your choice, Make, Ninja, etc.
