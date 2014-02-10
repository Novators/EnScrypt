# EnScrypt 1.1 build 7

EnScrypt is an acceleration-resistant password hashing library and utility based on the memory-hard PBKDF Scrypt.

## Based On

* SQRL 
    * Author: Steve Gibson
    * [Author's Site](https://www.grc.com/)
    * [Algorithm Detail](https://www.grc.com/sqrl/scrypt.htm)
    * This software (EnScrypt) is an implementation of the EnScrypt algorithm that Steve designed for SQRL.
* scrypt-jane 
    * Author: Andrew M
    * [Source](https://github.com/floodyberry/scrypt-jane)
    * License: Public Domain, or MIT
    * The heavy lifting of the EnScrypt algorithm is handled by scrypt-jane, with minor modifications.
* getRealTime.c 
    * Author: David Robert Nadeau
    * [Author's Site](http://NadeauSoftware.com/)
    * [Article](http://nadeausoftware.com/articles/2012/04/c_c_tip_how_measure_elapsed_real_time_benchmarking)
    * License: Creative Commons Attribution 3.0 Unported License (http://creativecommons.org/licenses/by/3.0/deed.en_US)
    * This is used for accurate cross-platform timing.

## Supported Platforms
This implementation is intended to work on multiple platforms.  Tested platforms include:
* Linux (Ubuntu 32 and 64 bit)
* Android ARM
* Android ARM with NEON (optimized implementation with runtime detection of NEON support and fallback)

Compiles, but has not been thoroughly tested on:
* Android X86
* Android MIPS

Planned, but not yet supported:
* Windows
* OSX
* iOS

## Building and Installing
EnScrypt uses the [CMake](http://www.cmake.org/) build system.  CMake will create project files for your target platform.  To build on a linux system:

```
$ git clone https://github.com/Novators/EnScrypt.git
$ cd EnScrypt
```

If you are building for Android, you'll need the NDK installed (or linked) in ```./ndk```.  You must also have 'ant', 'zip', and 'android' available on your system.

```
$ ln -s /opt/android-ndk-r9c ndk
```

Now it's time to build.  The following will build for all available architectures:

```
$ cmake .
$ make
```

Assuming a successful build, you should find the following files interesting:

```
build/linux/enscrypt -- the command line utility
build/linux/include/enscrypt.h -- header for the library
build/linux/lib/libenscrypt.a -- static library
build/linux/lib/libenscrypt.so -- shared library
build/android/EnScrypt.zip -- Includes the libraries and a Java class to use them.  Import into your Android project's root to add EnScrypt support.
```

If you'd like to run the test suite:

```
$ cd build/linux
$ make check
```

## Using the command line utility
The command line utility is substantially similar to Steve Gibson's [reference implementation](https://www.grc.com/sqrl/scrypt.htm).  Normal use is almost identical:

```
enscrypt [-q] [password] [salt] [iteration_count | duration]
```

All arguments are optional, and order is arbitrary.  This implementation adds the "-q" and "-h" options.

* duration: An integer + 's' or 'S'.  For example, "5s" == 5 seconds.
* iteration_count: An integer + 'i' or 'I'.  For example, "100i" == 100 iterations.
* salt: a 64 character hex string representing a 32 byte salt.  Allowed characters are 0-9, a-f, and A-F.
* -q: Quiet.  Suppresses all output except the computed output key.
* -h: Help.  Displays usage information.
* password: any string not matching the above arguments.

## Using the library
Documentation is available in the enscrypt.h header, or in HTML in the doc directory.

### Basic Use
Typically, you'll just need two functions from the library, enscrypt() and enscrypt_ms().  They look like this:

```
int enscrypt(uint8_t *buf, const char *passwd, const uint8_t *salt, int iterations, enscrypt_progress_fn cb_ptr );
int enscrypt_ms( uint8_t *buf, const char *passwd, const uint8_t *salt, int millis, enscrypt_progress_fn cb_ptr );
```

To run 100 iterations:
```
#include <enscrypt.h>

int main()
{
  int time_elapsed;
  uint8_t buf[32];
  uint8_t salt[32] = {0};
  
  time_elapsed = enscrypt( buf, "password", salt, 100, NULL );
}
```

Or, to run for 5 seconds:
```
#include <enscrypt.h>

int main()
{
  int iteration_count;
  uint8_t buf[32];
  uint8_t salt[32] = {0};
  
  iteration_count = enscrypt_ms( buf, "password", salt, 5000, NULL );
}
```

To use a callback function to monitor progress:
```
#include <enscrypt.h>

void progress_fn( int progress )
{
  printf( "%d%%\n", progress );
}

int main()
{
  uint8_t buf[32];
  
  enscrypt( buf, NULL, NULL, 100, progress_fn );
}
```

### Advanced use
See the documentation in enscrypt.h for more details on each:

* enscrypt_progress_fn - progress callback prototype.
* enscrypt_fatal_errorfn - fatal error function prototype.
* enscrypt_get_real_time() - Get a value representing execution time.
* enscrypt_set_fatal_error() - Set a function to call in the event of an un-recoverable error.

## License
This work is released under the MIT License as follows:

Copyright (c) 2014 Adam Comley

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
