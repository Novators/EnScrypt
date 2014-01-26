project(enscrypt)
cmake_minimum_required(VERSION 2.6)
set(enscrypt_version 1.1)
set(enscrypt_build 2)

########## Configuration ##########
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

if(UNIX)
		set(LIBS ${LIBS} m rt)
#		set(CMAKE_INSTALL_PREFIX /usr/)
		if(CMAKE_COMPILER_IS_GNUCC)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -Wall")
		endif(CMAKE_COMPILER_IS_GNUCC)
endif(UNIX)

configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_SOURCE_DIR}/config.h)
configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_SOURCE_DIR}/Doxyfile)
configure_file(${CMAKE_SOURCE_DIR}/README.md.in ${CMAKE_SOURCE_DIR}/README.md)
configure_file(${CMAKE_SOURCE_DIR}/mainpage.dox.in ${CMAKE_SOURCE_DIR}/mainpage.dox)

########## Library Target ##########
set(LIBSRC enscrypt.c realtime.c)

add_library(enscrypt SHARED ${LIBSRC})
target_link_libraries(enscrypt ${LIBS})

########## Binary Target ##########
add_executable(enscrypt_bin enscrypt_bin.c)
target_link_libraries(enscrypt_bin enscrypt)
add_dependencies(enscrypt_bin enscrypt)
set_target_properties(enscrypt_bin PROPERTIES OUTPUT_NAME enscrypt)

########## Test Target ##########
enable_testing()

add_executable(enscrypt_test EXCLUDE_FROM_ALL test.c)
target_link_libraries(enscrypt_test enscrypt)
add_dependencies(enscrypt_test enscrypt)

set(CMAKE_CTEST_COMMAND ctest -V)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
add_test(test_enscrypt enscrypt_test)
add_dependencies(check enscrypt_test)

########## Install Target ##########
install(TARGETS enscrypt enscrypt_bin LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(FILES enscrypt.h DESTINATION include)

########## Uninstall Target ##############
CONFIGURE_FILE(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

########## Documentation ##########
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)
