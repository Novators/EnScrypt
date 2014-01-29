#!/bin/bash

VALID=("armeabi" "armeabi-v7a" "armeabi-v7a with NEON" "armeabi-v7a with VFPV3" "armeabi-v6 with VFP")
WD=${PWD##*/}
echo "-- BOOTSTRAPPING ${WD}..."
MATCH=$(echo "${VALID[@]:0}" | grep -o $WD)
if [ ! -f Makefile ]; then
	if [[ -z $MATCH ]]; then
		cmake ..
	else
		if [ $# -gt 0 ]; then
			cmake -DANDROID_STANDALONE_TOOLCHAIN="${1}" -DANDROID_ABI="${WD}" -DCMAKE_TOOLCHAIN_FILE=../../cmake/android.toolchain.cmake ..
		else
			echo "-- Android Toolchain not Specified!"
		fi
	fi
else
	echo "-- Makefile present... Skipping cmake."
fi
