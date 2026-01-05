# Prime Field Arithmetic

A C++ implementation of constant-time and constant memory access finite field arithmetic.

## Overview

This is a reimagining of [Finite-field-arithmetic](https://github.com/nkKolja/Finite-field-arithmetic) (C implementation) using modern C++ features:

- **Template metaprogramming** for zero-overhead prime-specific optimizations
- **`constexpr` computation** of Montgomery parameters at compile time
- **Type-safe field elements** preventing mixing of different prime fields
- **Operator overloading** for natural mathematical syntax


## Comparison with C Implementation

| Aspect | C Version | This C++ Version |
|--------|-----------|------------------|
| Montgomery params | Python codegen | `constexpr` functions |
| Prime selection | Compile-time macro | Template parameter |
| Type safety | None | Compile-time enforced |
| Syntax | `f_add(a, b, c)` | `c = a + b` |
| Optimizations | Weak symbols | Template specialization |
| Custom primes | Rebuild required | Define struct + done |

## Building

```bash
mkdir build && cd build
cmake ..
make
./examples
```



## License

MIT License - Copyright (c) 2026 Novak Kaluđerović

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
