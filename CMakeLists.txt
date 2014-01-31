project(EnScrypt)
cmake_minimum_required(VERSION 2.6)
set(enscrypt_version_major 1)
set(enscrypt_version_minor 1)
set(enscrypt_build 6)
set(enscrypt_version "${enscrypt_version_major}.${enscrypt_version_minor} build ${enscrypt_build}")
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
endif()
set(SRCDIR ${CMAKE_SOURCE_DIR}/src)

macro(find_substring needle haystack found)
	set(${found} FALSE)
	foreach(delegate ${${haystack}})
		if("${delegate}" STREQUAL "${${needle}}")
			set(${found} TRUE)
		endif()
	endforeach()
endmacro()

########## Platform Tests ##########
find_program(ANT_PROGRAM ant)
find_program(ANDROID_PROGRAM android)
find_program(JAR_PROGRAM jar)
find_program(ZIP_PROGRAM zip)

set(ARM_ARCHS armeabi armeabi-v7a "armeabi-v7a_with_NEON" "armeabi-v7a_with_VFPV3" "armeabi-v6_with_VFP")
set(ARCHS ${ARM_ARCHS})
if(UNIX)
	set(ARCHS ${ARCHS} "linux")
endif(UNIX)

set(HAVE_ARM_TOOLCHAIN FALSE)
if(EXISTS ${CMAKE_SOURCE_DIR}/toolchain/arm)
	set(HAVE_ARM_TOOLCHAIN TRUE)
	set(arm_toolchain ${CMAKE_SOURCE_DIR}/toolchain/arm)
endif()
if(EXISTS ${CMAKE_SOURCE_DIR}/../toolchain/arm)
	set(HAVE_ARM_TOOLCHAIN TRUE)
	set(arm_toolchain ${CMAKE_SOURCE_DIR}/../toolchain/arm)
endif()
message("-- Detecting ARM Toolchain - " ${HAVE_ARM_TOOLCHAIN})

if(HAVE_ARM_TOOLCHAIN)
	if(ANT_PROGRAM)
		if(ANDROID_PROGRAM)
			set(ARCHS ${ARCHS} android)
		endif(ANDROID_PROGRAM)
	endif(ANT_PROGRAM)
endif(HAVE_ARM_TOOLCHAIN)



if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
	set(BUILD_meta 1)
else()
	foreach(arch ${ARCHS})
		if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}/${arch}")
			set(BUILD_${arch} 1)
			set(ARCH "${arch}")
		endif()
	endforeach()
endif()

find_substring(ARCH ARM_ARCHS substring_found)
if(substring_found)
	set(ARM_PLATFORM TRUE)
endif(substring_found)

########## Meta Configuration ##########
if(BUILD_meta)
	configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${SRCDIR}/config.h)
	configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_SOURCE_DIR}/Doxyfile)
	configure_file(${CMAKE_SOURCE_DIR}/README.md.in ${CMAKE_SOURCE_DIR}/README.md)
	configure_file(${CMAKE_SOURCE_DIR}/mainpage.dox.in ${CMAKE_SOURCE_DIR}/mainpage.dox)

	find_package(Doxygen QUIET)
	if(DOXYGEN_FOUND)
		add_custom_target(doc
			${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
	endif(DOXYGEN_FOUND)
	
	configure_file(${CMAKE_SOURCE_DIR}/CMakeLists.txt ${CMAKE_SOURCE_DIR}/build/CMakeLists.txt COPYONLY)
	file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
	foreach(arch ${ARCHS})
		find_substring(arch ARM_ARCHS substring_found)
		set(select_arch FALSE)
		if(substring_found)
			if(HAVE_ARM_TOOLCHAIN)
				set(select_arch TRUE)
			endif(HAVE_ARM_TOOLCHAIN)
		else(substring_found)
			set(select_arch TRUE)
		endif(substring_found)
		if(select_arch)
			message( "-- Selecting: " ${arch} )
			set(pr_dir "${CMAKE_SOURCE_DIR}/build/${arch}")
			file(MAKE_DIRECTORY "${pr_dir}")
			if("${arch}" STREQUAL "android")
				add_custom_target(${arch} ALL DEPENDS ${pr_dir}/Makefile armeabi armeabi-v7a)
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
				COMMAND ${CMAKE_SOURCE_DIR}/bootstrap.sh ${arm_toolchain}
			)
		endif(select_arch)
	endforeach(arch)
	unset(pr_dir)
	
endif(BUILD_meta)

########## Linux Configuration ##########
if(BUILD_linux)
	project(EnScrypt-linux)
	set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/..)
	set(SRCDIR ${CMAKE_SOURCE_DIR}/src)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set(LIBS ${LIBS} rt)
#	set(CMAKE_INSTALL_PREFIX /usr/)
	if(CMAKE_COMPILER_IS_GNUCC)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
	endif(CMAKE_COMPILER_IS_GNUCC)
	configure_file(${SRCDIR}/enscrypt.h ${CMAKE_CURRENT_BINARY_DIR}/include/enscrypt.h COPYONLY)

	########## Library Target ##########
	set(LIBSRC ${SRCDIR}/enscrypt.c ${SRCDIR}/realtime.c)
	add_library(enscrypt SHARED ${LIBSRC})
	target_link_libraries(enscrypt ${LIBS})
	add_library(enscrypt_static STATIC ${LIBSRC})
	target_link_libraries(enscrypt_static ${LIBS})
	set_target_properties(enscrypt_static PROPERTIES OUTPUT_NAME enscrypt)

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
	install(TARGETS enscrypt enscrypt_bin enscrypt_static 
		LIBRARY DESTINATION lib 
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin)
	install(FILES enscrypt.h DESTINATION include)

	########## Uninstall Target ##########
	CONFIGURE_FILE(
		"${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)
	ADD_CUSTOM_TARGET(uninstall
		"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")
endif(BUILD_linux)

if(BUILD_android)
	project(EnScrypt-${ARCH})
	set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/..)
	set(SRCDIR ${CMAKE_CURRENT_BINARY_DIR}/EnScrypt)
	
	set(ANDROID_TARGET "android-19")
	configure_file("${CMAKE_CURRENT_BINARY_DIR}/../armeabi/libenscrypt.so" ${SRCDIR}/libs/armeabi/libenscrypt.so COPYONLY)
	configure_file("${CMAKE_CURRENT_BINARY_DIR}/../armeabi-v7a/libenscrypt.so" ${SRCDIR}/libs/armeabi-v7a/libenscrypt.so COPYONLY)
	configure_file("${CMAKE_SOURCE_DIR}/src/android/Enscrypt.java" ${SRCDIR}/src/com/sqrlauth/enscrypt/Enscrypt.java COPYONLY)
	configure_file("${CMAKE_SOURCE_DIR}/src/android/AndroidManifest.xml" ${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml COPYONLY)
	#configure_file("${CMAKE_SOURCE_DIR}/src/android/Android.mk" "${SRCDIR}/jni/Android.mk" COPYONLY)
	
	message("ANDROID: " ${ANDROID_PROGRAM})
	message("ANT: " ${ANT_PROGRAM})
	
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
endif(BUILD_android)

########## Android-ARM Configuration ##########
if(ARM_PLATFORM)
	project(EnScrypt-${ARCH})
	set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/..)
	set(SRCDIR ${CMAKE_SOURCE_DIR}/src)
	
	set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/toolchain/arm/sysroot/usr/include)
	
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	
	if(CMAKE_COMPILER_IS_GNUCC)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
	endif(CMAKE_COMPILER_IS_GNUCC)
	configure_file(${SRCDIR}/enscrypt.h ${CMAKE_CURRENT_BINARY_DIR}/include/enscrypt.h COPYONLY)

	########## Library Target ##########
	set(LIBSRC ${SRCDIR}/enscrypt.c ${SRCDIR}/realtime.c ${SRCDIR}/com_sqrlauth_enscrypt_Enscrypt.c)
	add_library(enscrypt SHARED ${LIBSRC})
	#add_library(enscrypt_static STATIC ${LIBSRC})
	#set_target_properties(enscrypt_static PROPERTIES OUTPUT_NAME enscrypt)
endif(ARM_PLATFORM)