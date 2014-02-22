# Building EnScrypt

For a higher level overview, see README.md.

## Setting up the build environment
EnScrypt uses the [CMake](http://www.cmake.org/) build system.  CMake will create project files for your target platform.  In order to support multiple toolchains (cross-compilers), the build process has to work a little bit differently than you may be used to.

To get started:

```
$ git clone https://github.com/Novators/EnScrypt.git
$ cd EnScrypt
```

## Building for Android
At this time, android builds are only supported on Linux based systems.  The android build requires the following executables to be installed on your system:

* ant
* zip
* android

These may be available through your system's package manager.

You'll need the latest Android NDK linked to ```EnScrypt/ndk```.  

```
EnScrypt $ ln -s /opt/android-ndk-r9c ndk
```

Now, to generate build files and build for both linux and Android:

```
EnScrypt $ cmake .
EnScrypt $ make
```

## Building for Linux

```
EnScrypt $ cmake .
EnScrypt $ make
```

The linux build includes a command line utility and a test suite, both located in ```EnScrypt/build/linux```.  To run the tests:

```
EnScrypt $ cd build/linux
EnScrypt/build/linux $ make check
```

Linux builds also include install and uninstall targets.  To install the library and executable (to /usr/local):

```
EnScrypt/build/linux $ sudo make install
```

## Building for Windows
The windows build is a little different.  To get started:

```
C:\...\EnScrypt> generate_win32.bat
```

This will create solution and project files in ```EnScrypt\build\win32```.  Open the solution in Visual Studio and build...
