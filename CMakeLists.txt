project(EnScrypt)
cmake_minimum_required(VERSION 2.6)

set(enscrypt_version_major 1)
set(enscrypt_version_minor 1)
set(enscrypt_build 8)
set(enscrypt_version "${enscrypt_version_major}.${enscrypt_version_minor} build ${enscrypt_build}")

set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

set(SRCDIR ${CMAKE_SOURCE_DIR}/src)

macro(find_substring needle haystack found)
	set(${found} FALSE)
	foreach(delegate ${${haystack}})
		if("${delegate}" STREQUAL "${${needle}}")
			set(${found} TRUE)
		endif()
	endforeach()
endmacro()

if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
endif()

########## Platform Tests ##########
set(ALL_ARCHS "mac" "win32" "linux" "armeabi" "armeabi-v7a" "x86" "mips" "android")
set(ANDROID_ARCHS "armeabi" "armeabi-v7a" "x86" "mips")

foreach(arch ${ALL_ARCHS})
	if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}/${arch}")
		set(ARCH "${arch}")
	endif()
endforeach()

if(WIN32)
	if(NOT ARCH STREQUAL "win32")
		message(FATAL_ERROR "To build on a Windows platform, execute the generate_win32.bat file first.  Your project files will be created in the build directory.")
	endif()
endif(WIN32)

find_program(ANT_PROGRAM ant)
find_program(ANDROID_PROGRAM android)
find_program(JAR_PROGRAM jar)
find_program(ZIP_PROGRAM zip)

find_substring(ARCH ANDROID_ARCHS IS_ANDROID_ARCH)

######################### WIN32 BUILD #########################
if(ARCH STREQUAL "win32")
	project(EnScrypt-win32)
	set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/..)
	set(SRCDIR ${CMAKE_SOURCE_DIR}/src)
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
	configure_file(${SRCDIR}/enscrypt.h ${CMAKE_CURRENT_BINARY_DIR}/include/enscrypt.h COPYONLY)

	########## Library Target ##########
	set(LIBSRC ${SRCDIR}/enscrypt.c ${SRCDIR}/realtime.c)
	add_library(enscrypt SHARED ${LIBSRC})

	########## Binary Target ##########
	add_executable(enscrypt_bin ${SRCDIR}/enscrypt_bin.c)
	target_link_libraries(enscrypt_bin enscrypt)
	add_dependencies(enscrypt_bin enscrypt)
	set_target_properties(enscrypt_bin PROPERTIES OUTPUT_NAME enscrypt)

	########## Test Target ##########
	add_executable(enscrypt_test EXCLUDE_FROM_ALL ${SRCDIR}/test.c)
	target_link_libraries(enscrypt_test enscrypt)
	add_dependencies(enscrypt_test enscrypt)

######################### LINUX BUILD #########################
elseif(ARCH STREQUAL "linux")
	project(EnScrypt-linux)
	set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/..)
	set(SRCDIR ${CMAKE_SOURCE_DIR}/src)
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set(LIBS ${LIBS} rt)
#	set(CMAKE_INSTALL_PREFIX /usr/)
	if(CMAKE_COMPILER_IS_GNUCC)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
	endif(CMAKE_COMPILER_IS_GNUCC)

	configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
	configure_file(${SRCDIR}/enscrypt.h ${CMAKE_CURRENT_BINARY_DIR}/include/enscrypt.h COPYONLY)

	########## Library Target ##########
	set(LIBSRC ${SRCDIR}/enscrypt.c ${SRCDIR}/realtime.c)
	add_library(enscrypt SHARED ${LIBSRC})
	target_link_libraries(enscrypt ${LIBS})

	########## Binary Target ##########
	add_executable(enscrypt_bin ${SRCDIR}/enscrypt_bin.c)
	target_link_libraries(enscrypt_bin enscrypt)
	add_dependencies(enscrypt_bin enscrypt)
	set_target_properties(enscrypt_bin PROPERTIES OUTPUT_NAME enscrypt)

	########## Test Target ##########
	enable_testing()
	add_executable(enscrypt_test EXCLUDE_FROM_ALL ${SRCDIR}/test.c)
	target_link_libraries(enscrypt_test enscrypt)
	add_dependencies(enscrypt_test enscrypt)
	set(CMAKE_CTEST_COMMAND ctest -V)
	add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
	add_test(test_enscrypt enscrypt_test)
	add_dependencies(check enscrypt_test)

	########## Install Target ##########
	install(TARGETS enscrypt enscrypt_bin
		LIBRARY DESTINATION lib 
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/enscrypt.h DESTINATION include)

	########## Uninstall Target ##########
	CONFIGURE_FILE(
		"${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)
	ADD_CUSTOM_TARGET(uninstall
		"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

######################### MAC BUILD #########################
elseif(ARCH STREQUAL "mac")
		project(EnScrypt-mac)
		set(MACOS_X true)
		unset(UNIX)
		if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
			set(CLANG true)
		endif()
		set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/..)
		set(SRCDIR ${CMAKE_SOURCE_DIR}/src)
		include_directories(${CMAKE_CURRENT_BINARY_DIR})
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
		if(CMAKE_COMPILER_IS_GNUCC)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
		endif(CMAKE_COMPILER_IS_GNUCC)

		configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
		configure_file(${SRCDIR}/enscrypt.h ${CMAKE_CURRENT_BINARY_DIR}/include/enscrypt.h COPYONLY)

		########## Library Target ##########
		set(LIBSRC ${SRCDIR}/enscrypt.c ${SRCDIR}/realtime.c)
		add_library(enscrypt SHARED ${LIBSRC})
		target_link_libraries(enscrypt ${LIBS})

		########## Binary Target ##########
		add_executable(enscrypt_bin ${SRCDIR}/enscrypt_bin.c)
		target_link_libraries(enscrypt_bin enscrypt)
		add_dependencies(enscrypt_bin enscrypt)
		set_target_properties(enscrypt_bin PROPERTIES OUTPUT_NAME enscrypt)

		########## Test Target ##########
		enable_testing()
		add_executable(enscrypt_test EXCLUDE_FROM_ALL ${SRCDIR}/test.c)
		target_link_libraries(enscrypt_test enscrypt)
		add_dependencies(enscrypt_test enscrypt)
		set(CMAKE_CTEST_COMMAND ctest -V)
		add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
		add_test(test_enscrypt enscrypt_test)
		add_dependencies(check enscrypt_test)

		########## Install Target ##########
		install(TARGETS enscrypt enscrypt_bin
			LIBRARY DESTINATION lib 
			ARCHIVE DESTINATION lib
			RUNTIME DESTINATION bin)
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/enscrypt.h DESTINATION include)
		########## Uninstall Target ##########
		CONFIGURE_FILE(
			"${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
			"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
			IMMEDIATE @ONLY)
		ADD_CUSTOM_TARGET(uninstall
			"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

######################### ANDROID BUILD #########################
elseif(ARCH STREQUAL "android")
	project(EnScrypt-${ARCH})
	set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/..)
	set(SRCDIR ${CMAKE_CURRENT_BINARY_DIR}/EnScrypt)

	configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

	set(ANDROID_TARGET "android-19")
	
	configure_file("${CMAKE_SOURCE_DIR}/src/android/Enscrypt.java" ${SRCDIR}/src/com/sqrlauth/enscrypt/Enscrypt.java COPYONLY)
	configure_file("${CMAKE_SOURCE_DIR}/src/android/AndroidManifest.xml" ${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml COPYONLY)
	foreach(ldir ${ANDROID_ARCHS})
		configure_file("${CMAKE_CURRENT_BINARY_DIR}/../${ldir}/libenscrypt.so" "${SRCDIR}/libs/${ldir}/libenscrypt.so" COPYONLY)
	endforeach(ldir)
	unset(ldir)
	
	add_custom_command(
		OUTPUT "${SRCDIR}/build.xml"
		WORKING_DIRECTORY "${SRCDIR}"
		COMMAND ${ANDROID_PROGRAM} create lib-project --target ${ANDROID_TARGET} --name EnScrypt --path . --package com.sqrlauth.enscrypt 
		COMMAND cp ../AndroidManifest.xml .
	)

	add_custom_target(
		android-lib ALL
		COMMAND ${ANT_PROGRAM} debug
		DEPENDS "${SRCDIR}/build.xml"
		WORKING_DIRECTORY "${SRCDIR}"
	)
	
	if(JAR_PROGRAM)
		add_custom_command(
			TARGET android-lib
			POST_BUILD
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/EnScrypt
			COMMAND ${JAR_PROGRAM} cf com.sqrlauth.enscrypt.jar -C bin/classes com
			COMMAND mv com.sqrlauth.enscrypt.jar libs
		)
		if(ZIP_PROGRAM)
			add_custom_command(
				TARGET android-lib
				POST_BUILD
				WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/EnScrypt
				COMMAND ${ZIP_PROGRAM} -r ../EnScrypt.zip libs 
			)
		endif(ZIP_PROGRAM)
	endif(JAR_PROGRAM)

######################### ANDROID LIBRARY #########################
elseif(IS_ANDROID_ARCH)
	project(EnScrypt-${ARCH})
	set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/..)
	set(SRCDIR ${CMAKE_SOURCE_DIR}/src)
	
	if(${ARCH} STREQUAL "armeabi-v7a")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp -mfpu=neon -DARM7")
	endif()

	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	include_directories(${SRCDIR})
	configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
	
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	
	if(CMAKE_COMPILER_IS_GNUCC)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
	endif(CMAKE_COMPILER_IS_GNUCC)
	configure_file(${SRCDIR}/enscrypt.h ${CMAKE_CURRENT_BINARY_DIR}/include/enscrypt.h COPYONLY)

	########## Library Target ##########
	set(LIBSRC ${SRCDIR}/enscrypt.c ${SRCDIR}/realtime.c ${SRCDIR}/com_sqrlauth_enscrypt_Enscrypt.c ${SRCDIR}/cpu-features.c)
	add_library(enscrypt SHARED ${LIBSRC})

######################### META BUILD #########################
else()
	configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_SOURCE_DIR}/Doxyfile)
	configure_file(${CMAKE_SOURCE_DIR}/README.md.in ${CMAKE_SOURCE_DIR}/README.md)
	configure_file(${CMAKE_SOURCE_DIR}/mainpage.dox.in ${CMAKE_SOURCE_DIR}/mainpage.dox)
	configure_file(${CMAKE_SOURCE_DIR}/CMakeLists.txt ${CMAKE_SOURCE_DIR}/build/CMakeLists.txt COPYONLY)
	
	find_package(Doxygen QUIET)
	if(DOXYGEN_FOUND)
		add_custom_target(doc
			${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
	endif(DOXYGEN_FOUND)
	
	unset(ARCHS)
	if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(ARCHS "mac")
	elseif(UNIX)
		set(ARCHS "linux")
	endif()

	if(EXISTS "${CMAKE_SOURCE_DIR}/ndk")
		if(ANT_PROGRAM)
			if(ZIP_PROGRAM)
				if(ANDROID_PROGRAM)
					set(ARCHS "${ARCHS}" ${ANDROID_ARCHS} "android")
				endif(ANDROID_PROGRAM)
			endif(ZIP_PROGRAM)
		endif(ANT_PROGRAM)
	else()
		message( "-- Android build requires 'android', 'ant', 'zip', and an ndk at ./ndk" )
		message( "-- See README.md for build instructions." )
	endif()
	
	foreach(arch ${ARCHS})
		message( "-- Selecting: " ${arch} )
		set(pr_dir "${CMAKE_SOURCE_DIR}/build/${arch}")
		file(MAKE_DIRECTORY "${pr_dir}")
		if("${arch}" STREQUAL "android")
			add_custom_target(${arch} ALL DEPENDS ${pr_dir}/Makefile ${ANDROID_ARCHS})
		else()
			add_custom_target(${arch} ALL DEPENDS ${pr_dir}/Makefile)
		endif()
		add_custom_command(
			TARGET ${arch}
			PRE_BUILD
			WORKING_DIRECTORY ${pr_dir}
			COMMAND "${CMAKE_MAKE_PROGRAM}"
		)
		add_custom_command(
			OUTPUT "${pr_dir}/Makefile" "${pr_dir}/CMakeCache.txt" "${pr_dir}/CMakeFiles/"
			WORKING_DIRECTORY ${pr_dir}
			COMMAND ${CMAKE_SOURCE_DIR}/bootstrap.sh ${CMAKE_SOURCE_DIR}
		)
	endforeach(arch)
	unset(pr_dir)
	unset(toolchain)
endif()