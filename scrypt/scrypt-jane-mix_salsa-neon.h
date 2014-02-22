#if !defined(SCRYPT_CHOOSE_COMPILETIME) || !defined(SCRYPT_SALSA_INCLUDED)

/*
	* version 20110505
	* D. J. Bernstein
	* Public domain.
	*
	* Based on crypto_core/salsa208/armneon/core.c from SUPERCOP 20130419
	*/
#include <errno.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define ROUNDS 8
static void
salsa_core_neon(uint32x4_t *state)
{
	int i;
	
	const uint32x4_t abab = {-1,0,-1,0};
	
	/*
	 * This is modified since we only have one argument. Usually you'd rearrange
	 * the constant, key, and input bytes, but we just have one linear array to
	 * rearrange which is a bit easier.
	 */
	
	/*
	 * Change the input to be diagonals as if it's a 4x4 matrix of 32-bit values.
	 */
	uint32x4_t x0x5x10x15;
	uint32x4_t x12x1x6x11;
	uint32x4_t x8x13x2x7;
	uint32x4_t x4x9x14x3;
	
	uint32x4_t x0x1x10x11;
	uint32x4_t x12x13x6x7;
	uint32x4_t x8x9x2x3;
	uint32x4_t x4x5x14x15;
	
	x0x1x10x11 = vcombine_u32(vget_low_u32(state[0]), vget_high_u32(state[2]));
	x4x5x14x15 = vcombine_u32(vget_low_u32(state[1]), vget_high_u32(state[3]));
	x8x9x2x3 = vcombine_u32(vget_low_u32(state[2]), vget_high_u32(state[0]));
	x12x13x6x7 = vcombine_u32(vget_low_u32(state[3]), vget_high_u32(state[1]));
	
	x0x5x10x15 = vbslq_u32(abab,x0x1x10x11,x4x5x14x15);
	x8x13x2x7 = vbslq_u32(abab,x8x9x2x3,x12x13x6x7);
	x4x9x14x3 = vbslq_u32(abab,x4x5x14x15,x8x9x2x3);
	x12x1x6x11 = vbslq_u32(abab,x12x13x6x7,x0x1x10x11);
	
	uint32x4_t start0 = x0x5x10x15;
	uint32x4_t start1 = x12x1x6x11;
	uint32x4_t start3 = x4x9x14x3;
	uint32x4_t start2 = x8x13x2x7;
	
	/* From here on this should be the same as the SUPERCOP version. */
	
	uint32x4_t diag0 = start0;
	uint32x4_t diag1 = start1;
	uint32x4_t diag2 = start2;
	uint32x4_t diag3 = start3;
	
	uint32x4_t a0;
	uint32x4_t a1;
	uint32x4_t a2;
	uint32x4_t a3;
	
	for (i = ROUNDS;i > 0;i -= 2) {
		a0 = diag1 + diag0;
		diag3 ^= vsriq_n_u32(vshlq_n_u32(a0,7),a0,25);
		a1 = diag0 + diag3;
		diag2 ^= vsriq_n_u32(vshlq_n_u32(a1,9),a1,23);
		a2 = diag3 + diag2;
		diag1 ^= vsriq_n_u32(vshlq_n_u32(a2,13),a2,19);
		a3 = diag2 + diag1;
		diag0 ^= vsriq_n_u32(vshlq_n_u32(a3,18),a3,14);
		
		diag3 = vextq_u32(diag3,diag3,3);
		diag2 = vextq_u32(diag2,diag2,2);
		diag1 = vextq_u32(diag1,diag1,1);
		
		a0 = diag3 + diag0;
		diag1 ^= vsriq_n_u32(vshlq_n_u32(a0,7),a0,25);
		a1 = diag0 + diag1;
		diag2 ^= vsriq_n_u32(vshlq_n_u32(a1,9),a1,23);
		a2 = diag1 + diag2;
		diag3 ^= vsriq_n_u32(vshlq_n_u32(a2,13),a2,19);
		a3 = diag2 + diag3;
		diag0 ^= vsriq_n_u32(vshlq_n_u32(a3,18),a3,14);
		
		diag1 = vextq_u32(diag1,diag1,3);
		diag2 = vextq_u32(diag2,diag2,2);
		diag3 = vextq_u32(diag3,diag3,1);
	}
	
	x0x5x10x15 = diag0 + start0;
	x12x1x6x11 = diag1 + start1;
	x8x13x2x7 = diag2 + start2;
	x4x9x14x3 = diag3 + start3;
	
	x0x1x10x11 = vbslq_u32(abab,x0x5x10x15,x12x1x6x11);
	x12x13x6x7 = vbslq_u32(abab,x12x1x6x11,x8x13x2x7);
	x8x9x2x3 = vbslq_u32(abab,x8x13x2x7,x4x9x14x3);
	x4x5x14x15 = vbslq_u32(abab,x4x9x14x3,x0x5x10x15);
	
	state[0] = vcombine_u32(vget_low_u32(x0x1x10x11),vget_high_u32(x8x9x2x3));
	state[1] = vcombine_u32(vget_low_u32(x4x5x14x15),vget_high_u32(x12x13x6x7));
	state[2] = vcombine_u32(vget_low_u32(x8x9x2x3),vget_high_u32(x0x1x10x11));
	state[3] = vcombine_u32(vget_low_u32(x12x13x6x7),vget_high_u32(x4x5x14x15));
}

static void
blkcpy(void *dest, void *src, size_t len)
{
	uint32x4_t *D = dest;
	uint32x4_t *S = src;
	for( len = (len / 16) - 1; len; len-- ) {
		D[len] = S[len];
	}
}

static void
blkxor(void * dest, void * src, size_t len)
{
	uint32x4_t * D = dest;
	uint32x4_t * S = src;
	len /= 16;
	size_t i;
	
	for (i = 0; i < len; i++)
		D[i] = veorq_u32(D[i], S[i]);
}

/*
 *	Bout = ChunkMix(Bin)
 * 
 *	2*r: number of blocks in the chunk
 */
static void 
scrypt_ChunkMix_neon(uint32x4_t * Bin, uint32x4_t * Bout, uint32x4_t * X, size_t r) {
	size_t i;
	
	/* 1: X <-- B_{2r - 1} */
	memcpy(X, &Bin[8 * r - 4], 64);
	
	/* 2: for i = 0 to 2r - 1 do */
	for (i = 0; i < r; i++) {
		/* 3: X <-- H(X \xor B_i) */
		blkxor(X, &Bin[i * 8], 64);
		salsa_core_neon((void *) X);
		
		/* 4: Y_i <-- X */
		/* 6: B' <-- (Y_0, Y_2 ... Y_{2r-2}, Y_1, Y_3 ... Y_{2r-1}) */
		memcpy(&Bout[i * 4], X, 64);
		
		/* 3: X <-- H(X \xor B_i) */
		blkxor(X, &Bin[i * 8 + 4], 64);
		salsa_core_neon((void *) X);
		
		/* 4: Y_i <-- X */
		/* 6: B' <-- (Y_0, Y_2 ... Y_{2r-2}, Y_1, Y_3 ... Y_{2r-1}) */
		memcpy(&Bout[(r + i) * 4], X, 64);
	}
}

/**
 * integerify(B, r):
 * Return the result of parsing B_{2r-1} as a little-endian integer.
 */
static uint64_t
integerify(void * B, size_t r)
{
	uint8_t * X = (void*)((uintptr_t)(B) + (2 * r - 1) * 64);
	
	return (le64dec(X));
}

#define ROMIX_CONST 32
#define ROMIX_CONST_4 128

static void NOINLINE FASTCALL
scrypt_ROMix_neon(scrypt_mix_word_t *aX/*[chunkWords]*/, scrypt_mix_word_t *aY/*[chunkWords]*/, scrypt_mix_word_t *V/*[N * chunkWords]*/, uint32_t N, uint32_t r) {
	uint32x4_t Z[4];
	uint32x4_t *X = (uint32x4_t*)aX;
	uint32x4_t *Y = (uint32x4_t*)aY;
	uint64_t i, j = 0;
	
	/* 2: for i = 0 to N - 1 do */
	for (i = 0; i < N; i += 2) {
		
		/* 3: V_i <-- X */
		memcpy((void*)(V + i * ROMIX_CONST * r), X, ROMIX_CONST_4 * r);
		
		/* 4: X <-- H(X) */
		scrypt_ChunkMix_neon(X, Y, Z, r);
		
		/* 3: V_i <-- X */
		memcpy((void *)(V + (i + 1) * ROMIX_CONST * r),
			   Y, ROMIX_CONST_4 * r);
		
		/* 4: X <-- H(X) */
		scrypt_ChunkMix_neon(Y, X, Z, r);
	}
	
	/* 6: for i = 0 to N - 1 do */
	for (i = 0; i < N; i += 2) {
		/* 7: j <-- Integerify(X) mod N */
		j = integerify(X, r) & (N - 1);
		
		/* 8: X <-- H(X \xor V_j) */
		blkxor(X, (void *)(V + j * ROMIX_CONST * r), ROMIX_CONST_4 * r);
		scrypt_ChunkMix_neon(X, Y, Z, r);
		
		/* 7: j <-- Integerify(X) mod N */
		j = integerify(Y, r) & (N - 1);
		
		/* 8: X <-- H(X \xor V_j) */
		blkxor(Y, (void *)(V + j * ROMIX_CONST * r), ROMIX_CONST_4 * r);
		scrypt_ChunkMix_neon(Y, X, Z, r);
	}
}

#undef ROMIX_CONST
#undef ROMIX_CONST_4

#define SCRYPT_SALSA_INCLUDED
#define SCRYPT_SALSA_NEON

#endif
