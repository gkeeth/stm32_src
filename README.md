# Small STM32 code projects, proofs-of-concept, and examples

## Usage:
```
make -C libopencm3
make -C <project name>
```

`make all`, `make clean`, `make flash`, and `make debug` rules are available.

GDB/OpenOCD scripts are set up to use a JLINK EDU programmer. Adapt the scripts
as necessary for other interfaces.

## License:
Unless otherwise noted, examples are MIT-licensed.
8x8 Bitmap font (`font8x8_basic.h`) is public domain and obtained from:
github.com/dhepper/font8x8

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
