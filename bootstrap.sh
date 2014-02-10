#!/bin/bash

VALID=("armeabi" "armeabi-v7a" "x86" "mips")
WD=${PWD##*/}
WD=${WD//_/ }
NDK=${1}/ndk
ARCH=${WD:0:3}
if [ ${ARCH} = "mip" ]; then
	ARCH=mips
fi
TC=${1}/toolchain/${ARCH}

echo "-- BOOTSTRAPPING ${WD}"
MATCH=$(echo "${VALID[@]:0}" | grep -o "${WD}")
if [[ ! -z $MATCH ]]; then
	if [ ! -f ${TC}/SOURCES ]; then
		echo -- Creating Toolchain at ${TC}
		${NDK}/build/tools/make-standalone-toolchain.sh --arch=${ARCH} --platform=android-9 --install-dir=${TC}
	else
		echo -- Found Toolchain at ${TC}
	fi
fi

if [ ! -f Makefile ]; then
	if [[ -z $MATCH ]]; then
		cmake ..
	else
		if [ $# -gt 0 ]; then
			cmake -DANDROID_STANDALONE_TOOLCHAIN="${TC}" -DANDROID_ABI="${WD}" -DCMAKE_TOOLCHAIN_FILE=../../cmake/android.toolchain.cmake ..
		else
			echo "-- Android Toolchain not Specified!"
		fi
	fi
else
	echo "-- Makefile present... Skipping cmake."
fi

