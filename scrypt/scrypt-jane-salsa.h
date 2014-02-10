#define SCRYPT_MIX_BASE "Salsa20/8"

typedef uint32_t scrypt_mix_word_t;

#define SCRYPT_WORDTO8_LE U32TO8_LE
#define SCRYPT_WORD_ENDIAN_SWAP U32_SWAP

#define SCRYPT_BLOCK_BYTES 64
#define SCRYPT_BLOCK_WORDS (SCRYPT_BLOCK_BYTES / sizeof(scrypt_mix_word_t))

/* must have these here in case block bytes is ever != 64 */
#include "scrypt-jane-romix-basic.h"

#include "scrypt-jane-mix_salsa-xop.h"
#include "scrypt-jane-mix_salsa-avx.h"
#include "scrypt-jane-mix_salsa-sse2.h"
#include "scrypt-jane-mix_salsa.h"
#if defined(NEON)
#include "scrypt-jane-mix_salsa-neon.h"
#endif

#if defined(SCRYPT_SALSA_XOP)
	#define SCRYPT_CHUNKMIX_FN scrypt_ChunkMix_xop
	#define SCRYPT_ROMIX_FN scrypt_ROMix_xop
	#define SCRYPT_ROMIX_TANGLE_FN salsa_core_tangle_sse2
	#define SCRYPT_ROMIX_UNTANGLE_FN salsa_core_tangle_sse2
	#include "scrypt-jane-romix-template.h"
#endif

#if defined(SCRYPT_SALSA_AVX)
	#define SCRYPT_CHUNKMIX_FN scrypt_ChunkMix_avx
	#define SCRYPT_ROMIX_FN scrypt_ROMix_avx
	#define SCRYPT_ROMIX_TANGLE_FN salsa_core_tangle_sse2
	#define SCRYPT_ROMIX_UNTANGLE_FN salsa_core_tangle_sse2
	#include "scrypt-jane-romix-template.h"
#endif

#if defined(SCRYPT_SALSA_SSE2)
	#define SCRYPT_CHUNKMIX_FN scrypt_ChunkMix_sse2
	#define SCRYPT_ROMIX_FN scrypt_ROMix_sse2
	#define SCRYPT_MIX_FN salsa_core_sse2
	#define SCRYPT_ROMIX_TANGLE_FN salsa_core_tangle_sse2
	#define SCRYPT_ROMIX_UNTANGLE_FN salsa_core_tangle_sse2
	#include "scrypt-jane-romix-template.h"
#endif

/* cpu agnostic */
#define SCRYPT_ROMIX_FN scrypt_ROMix_basic
#define SCRYPT_MIX_FN salsa_core_basic
#define SCRYPT_ROMIX_TANGLE_FN scrypt_romix_convert_endian
#define SCRYPT_ROMIX_UNTANGLE_FN scrypt_romix_convert_endian
#include "scrypt-jane-romix-template.h"

#if !defined(SCRYPT_CHOOSE_COMPILETIME)
static scrypt_ROMixfn
scrypt_getROMix(void) {
#if defined(SCRYPT_SALSA_XOP) || defined(SCRYPT_SALSA_AVX) || defined(SCRYPT_SALSA_SSE2) || defined(SCRYPT_SALSA_NEON)
	size_t cpuflags = detect_cpu();
#endif

#if defined(SCRYPT_SALSA_NEON)
	if( cpuflags & cpu_neon )
		return scrypt_ROMix_neon;
	else
#endif
	
#if defined(SCRYPT_SALSA_XOP)
	if (cpuflags & cpu_xop)
		return scrypt_ROMix_xop;
	else
#endif

#if defined(SCRYPT_SALSA_AVX)
	if (cpuflags & cpu_avx)
		return scrypt_ROMix_avx;
	else
#endif

#if defined(SCRYPT_SALSA_SSE2)
	if (cpuflags & cpu_sse2)
		return scrypt_ROMix_sse2;
	else
#endif

	return scrypt_ROMix_basic;
}
#endif


#if defined(SCRYPT_TEST_SPEED)
static size_t
available_implementations(void) {
	size_t cpuflags = detect_cpu();
	size_t flags = 0;

#if defined(SCRYPT_SALSA_XOP)
	if (cpuflags & cpu_xop)
		flags |= cpu_xop;
#endif

#if defined(SCRYPT_SALSA_AVX)
	if (cpuflags & cpu_avx)
		flags |= cpu_avx;
#endif

#if defined(SCRYPT_SALSA_SSE2)
	if (cpuflags & cpu_sse2)
		flags |= cpu_sse2;
#endif

	return flags;
}
#endif

