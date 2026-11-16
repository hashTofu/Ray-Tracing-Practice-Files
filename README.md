# Ray Tracing in One Weekend
>
> [!NOTE]
> Here lies the code I've written as a practice based on the amazing [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html)

## Build

### Only Build

```sh
g++ -O2 -fopenmp -o main main.cpp
```

On macOS, Apple clang rejects `-fopenmp`. Use Homebrew libomp instead:

```sh
brew install libomp
clang++ -O2 -Xpreprocessor -fopenmp \
        -I"$(brew --prefix)/opt/libomp/include" \
        -L"$(brew --prefix)/opt/libomp/lib" -lomp \
        -o main main.cpp
```

`make` detects Darwin and picks the right flags automatically.

`-O2` is not optional: the unoptimised build is ~12x slower (21s vs 1.8s at
400x225/100spp), which makes the OpenMP thread count look far worse than it is.

### Build and run

#### Requires ImageMagick

```sh
make
```

## Reference

[_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
