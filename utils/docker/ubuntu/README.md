# Build Images

```
# Base image
docker build -f Dockerfile.base -t soll:ubuntu-base

# GCC-build image
docker build -f Dockerfile.build-gcc -t soll:ubuntu-gcc

# Clang-build image
docker build -f Dockerfile.build-clang -t soll:ubuntu-clang

# Test image
docker build --build-arg BASE=soll:ubuntu-base \
    -f Dockerfile -t soll:ubuntu-test /path/to/ewasm-testbench/

# Full image
docker build --build-arg BASE=soll:ubuntu-gcc \
    -f Dockerfile -t soll:v0.0.1 /path/to/ewasm-testbench/
```
