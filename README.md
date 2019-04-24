# cpp-varint

[![Build Status](https://travis-ci.org/cpp-ipfs/cpp-varint.svg?branch=master)](https://travis-ci.org/cpp-ipfs/cpp-varint)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/cpp-ipfs/cpp-varint?svg=true)](https://ci.appveyor.com/project/jbrooker/cpp-varint)
[![Coverage Status](https://coveralls.io/repos/github/cpp-ipfs/cpp-varint/badge.svg?branch=master)](https://coveralls.io/github/cpp-ipfs/cpp-varint?branch=master)

A varint implementation in modern C++, providing codecs for encoding and decoding, and an integer-like value type.

## Installation

varint is available as a vcpkg port via the [cpp-ipfs fork](https://github.com/cpp-ipfs/vcpkg).

```shell
> git clone https://github.com/cpp-ipfs/vcpkg
> cd vcpkg

PS> .\bootstrap-vcpkg.bat
Linux:~/$ ./bootstrap-vcpkg.sh

PS> .\vcpkg install varint
Linux:~/$ ./vcpkg install varint

```

## Usage

```cpp
// create a varint
auto vi = varint::uleb128<std::array<char,2>>{};
// assign an integer value
vi = 1233u;
// convert back into an integer
auto i = static_cast<unsigned int>(vi);
```

## License

varint is released under the [MIT License](LICENSE.txt).