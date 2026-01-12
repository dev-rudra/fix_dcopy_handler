# Build binary with CMakeLists.txt
```bash
mkdir -p build
cmake -S . -B build
cmake --build build
```

# Build binary with Makefile
```bash
make build
```

# Run
```bash
./buld/fix_dropcopy

# Keep running after logout/SSH disconnect.
nohup .build/fix_dropcopy > dropcopy.log 2>&1 &
```

# Stop
```bash
ps -ef | grep fix_dropcopy
kill <pid>
```
