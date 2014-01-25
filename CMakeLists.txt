project(enscrypt)

cmake_minimum_required(VERSION 2.6)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -Wall)

if(UNIX)
		set(LIBS ${LIBS} m rt)
#		set(CMAKE_INSTALL_PREFIX /usr/)
		if(CMAKE_COMPILER_IS_GNUCC)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
		endif(CMAKE_COMPILER_IS_GNUCC)
endif(UNIX)

configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_SOURCE_DIR}/config.h)

include_directories(code)

set(LIBSRC enscrypt.c realtime.c)

add_library(enscrypt SHARED ${LIBSRC})
target_link_libraries(enscrypt ${LIBS})

add_executable(enscrypt_bin enscrypt_bin.c)
target_link_libraries(enscrypt_bin enscrypt)
add_dependencies(enscrypt_bin enscrypt)
set_target_properties(enscrypt_bin PROPERTIES OUTPUT_NAME enscrypt)

enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
add_executable(enscrypt_test EXCLUDE_FROM_ALL test.c)
target_link_libraries(enscrypt_test enscrypt)
add_dependencies(enscrypt_test enscrypt)
add_test(test_enscrypt enscrypt_test)
add_dependencies(check enscrypt_test)

install(TARGETS enscrypt enscrypt_bin LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(FILES enscrypt.h DESTINATION include)

########### Add uninstall target ###############
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")
