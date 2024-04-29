# Summary

This repository was created to demonstrate a GCC compiler optimization bug that
is observed in GCC versions 12 and above. At the time of this writing, GCC
versions 10, 11, 12, 13, and 14 were tested, with 10 and 11 not reproducing the
optimization bug, while 12 through 14 did.

# Reproduction
The [CMakeLists.txt](./CMakeLists.txt) file will build the
[decode.c](./decode.c) as both optimized and unoptimized binaries, named
`decode_optimized` and `decode_unoptimized`, respectively. Here's an example
invocation to reproduce the issue:

```sh
# Make sure you have GCC 12 or later:
gcc --version

# Now, build the binaries
cmake -B build -G Ninja
cmake --build build -v

# Now, run the two executables and observe the different outputs:
# The unoptimized build correctly decodes the input.
./build/decode_unoptimized

# The optimized build simply returns NULL
./build/decode_optimized
```

# godbolt
This can also been observed via [godbolt](https://godbolt.org):

* Unoptimized, not reproducing the bug: [https://godbolt.org/z/jd174n5hz](https://godbolt.org/z/jd174n5hz).
* Optimized, reproducing the bug: [https://godbolt.org/z/a8esc35vb](https://godbolt.org/z/a8esc35vb).
