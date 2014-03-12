md build
cd build
copy ..\CMakeLists.txt .
copy ..\CTestConfig.cmake .
md win32
cd win32
cmake ..
