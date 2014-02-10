#include "cpu-features.h"

#if defined(ARM7)
#include <arm_neon.h>
#define NEON
#endif

#if defined(SCRYPT_TEST_SPEED)
size_t cpu_detect_mask = (size_t)-1;
#endif

#if defined(NEON)
static size_t
detect_cpu(void) {
	size_t cpu_flags = 0;
	
	uint64_t features = android_getCpuFeatures();
	if( features & ANDROID_CPU_ARM_FEATURE_ARMv7 ) {
		cpu_flags |= cpu_armv7;
	}
	if( features & ANDROID_CPU_ARM_FEATURE_VFPv3 ) {
		cpu_flags |= cpu_vfp3;
	}
	if( features & ANDROID_CPU_ARM_FEATURE_NEON ) {
		cpu_flags |= cpu_neon;
	}
	
#if defined(SCRYPT_TEST_SPEED)
	cpu_flags &= cpu_detect_mask;
#endif

	return cpu_flags;
}
#endif

//struct cpu;