# Build Images

```
# GCC-build image
docker build -f Dockerfile.build-gcc -t soll-build:18.04-gcc

# Clang-build image
docker build -f Dockerfile.build-clang -t soll-build:18.04-clang

# Test image
docker build \
    -f Dockerfile -t soll-test:18.04 /path/to/ewasm-testbench/

# Full image
docker build --build-arg BASE=soll-build:18.04-gcc \
    -f Dockerfile -t soll:v0.0.1 /path/to/ewasm-testbench/
```
