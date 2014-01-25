project(enscrypt)

cmake_minimum_required(VERSION 2.6)
set(LIBS ${LIBS} rt)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -Wall)
#set(CMAKE_BUILD_TYPE Release)

if(UNIX)
		set(LIBS ${LIBS} m)
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

add_executable(test test.c)
target_link_libraries(test enscrypt)
add_dependencies(test enscrypt)