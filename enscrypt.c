/** @file enscrypt.c
 * 
 * EnScrypt, a Scrypt based password hashing library.
 * 
 * This work is released under the MIT License as follows:
 * 
 * Copyright (c) 2014 Adam Comley
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 **/

#include <string.h>
#include <stdbool.h>

#include "config.h"
#include "enscrypt.h"
#include "scrypt/scrypt-jane-portable.h"
#include "scrypt/scrypt-jane-hash.h"
#include "scrypt/scrypt-jane-romix.h"
#include "scrypt/scrypt-jane-test-vectors.h"

#define scrypt_maxNfactor 30  /* (1 << (30 + 1)) = ~2 billion */
#if (SCRYPT_BLOCK_BYTES == 64)
#define scrypt_r_32kb 8 /* (1 << 8) = 256 * 2 blocks in a chunk * 64 bytes = Max of 32kb in a chunk */
#elif (SCRYPT_BLOCK_BYTES == 128)
#define scrypt_r_32kb 7 /* (1 << 7) = 128 * 2 blocks in a chunk * 128 bytes = Max of 32kb in a chunk */
#elif (SCRYPT_BLOCK_BYTES == 256)
#define scrypt_r_32kb 6 /* (1 << 6) = 64 * 2 blocks in a chunk * 256 bytes = Max of 32kb in a chunk */
#elif (SCRYPT_BLOCK_BYTES == 512)
#define scrypt_r_32kb 5 /* (1 << 5) = 32 * 2 blocks in a chunk * 512 bytes = Max of 32kb in a chunk */
#endif
#define scrypt_maxrfactor scrypt_r_32kb /* 32kb */
#define scrypt_maxpfactor 25  /* (1 << 25) = ~33 million */

#include <stdio.h>
//#include <malloc.h>

typedef struct scrypt_aligned_alloc_t {
	uint8_t *mem, *ptr;
} scrypt_aligned_alloc;

static void NORETURN
scrypt_fatal_error_default(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

static enscrypt_fatal_errorfn scrypt_fatal_error = scrypt_fatal_error_default;

DLL_PUBLIC
void enscrypt_set_fatal_error(enscrypt_fatal_errorfn fn) {
	scrypt_fatal_error = fn;
}

static int
scrypt_power_on_self_test(void) {
	const scrypt_test_setting *t;
	uint8_t test_digest[64];
	uint32_t i;
	int res = 7, scrypt_valid;

	if (!scrypt_test_mix()) {
#if !defined(SCRYPT_TEST)
		scrypt_fatal_error("scrypt: mix function power-on-self-test failed");
#endif
		res &= ~1;
	}

	if (!scrypt_test_hash()) {
#if !defined(SCRYPT_TEST)
		scrypt_fatal_error("scrypt: hash function power-on-self-test failed");
#endif
		res &= ~2;
	}

	for (i = 0, scrypt_valid = 1; post_settings[i].pw; i++) {
		t = post_settings + i;
		enscrypt_scrypt((uint8_t *)t->pw, strlen(t->pw), (uint8_t *)t->salt, strlen(t->salt), t->Nfactor, t->rfactor, t->pfactor, test_digest, sizeof(test_digest));
		scrypt_valid &= scrypt_verify(post_vectors[i], test_digest, sizeof(test_digest));
	}
	
	if (!scrypt_valid) {
#if !defined(SCRYPT_TEST)
		scrypt_fatal_error("scrypt: scrypt power-on-self-test failed");
#endif
		res &= ~4;
	}

	return res;
}

#if defined(SCRYPT_TEST_SPEED)
static uint8_t *mem_base = (uint8_t *)0;
static size_t mem_bump = 0;

/* allocations are assumed to be multiples of 64 bytes and total allocations not to exceed ~1.01gb */
static scrypt_aligned_alloc
scrypt_alloc(uint64_t size) {
	scrypt_aligned_alloc aa;
	if (!mem_base) {
		mem_base = (uint8_t *)malloc((1024 * 1024 * 1024) + (1024 * 1024) + (SCRYPT_BLOCK_BYTES - 1));
		if (!mem_base)
			scrypt_fatal_error("scrypt: out of memory");
		mem_base = (uint8_t *)(((size_t)mem_base + (SCRYPT_BLOCK_BYTES - 1)) & ~(SCRYPT_BLOCK_BYTES - 1));
	}
	aa.mem = mem_base + mem_bump;
	aa.ptr = aa.mem;
	mem_bump += (size_t)size;
	return aa;
}

static void
scrypt_free(scrypt_aligned_alloc *aa) {
	mem_bump = 0;
}
#else
static scrypt_aligned_alloc
scrypt_alloc(uint64_t size) {
	static const size_t max_alloc = (size_t)-1;
	scrypt_aligned_alloc aa;
	size += (SCRYPT_BLOCK_BYTES - 1);
	if (size > max_alloc)
		scrypt_fatal_error("scrypt: not enough address space on this CPU to allocate required memory");
	aa.mem = (uint8_t *)malloc((size_t)size);
	aa.ptr = (uint8_t *)(((size_t)aa.mem + (SCRYPT_BLOCK_BYTES - 1)) & ~(SCRYPT_BLOCK_BYTES - 1));
	if (!aa.mem)
		scrypt_fatal_error("scrypt: out of memory");
	return aa;
}

static void
scrypt_free(scrypt_aligned_alloc *aa) {
	free(aa->mem);
}
#endif

DLL_PUBLIC
void enscrypt_scrypt(const uint8_t *password, size_t password_len, const uint8_t *salt, size_t salt_len, uint8_t Nfactor, uint8_t rfactor, uint8_t pfactor, uint8_t *out, size_t bytes) {
	scrypt_aligned_alloc YX, V;
	uint8_t *X, *Y;
	uint32_t N, r, p, chunk_bytes, i;

#if !defined(SCRYPT_CHOOSE_COMPILETIME)
	scrypt_ROMixfn scrypt_ROMix = scrypt_getROMix();
#endif

#if !defined(SCRYPT_TEST)
	static int power_on_self_test = 0;
	if (!power_on_self_test) {
		power_on_self_test = 1;
		if (!scrypt_power_on_self_test())
			scrypt_fatal_error("scrypt: power on self test failed");
	}
#endif

	if (Nfactor > scrypt_maxNfactor)
		scrypt_fatal_error("scrypt: N out of range");
	if (rfactor > scrypt_maxrfactor)
		scrypt_fatal_error("scrypt: r out of range");
	if (pfactor > scrypt_maxpfactor)
		scrypt_fatal_error("scrypt: p out of range");

	N = (1 << (Nfactor + 1));
	r = (1 << rfactor);
	p = (1 << pfactor);

	chunk_bytes = SCRYPT_BLOCK_BYTES * r * 2;
	V = scrypt_alloc((uint64_t)N * chunk_bytes);
	YX = scrypt_alloc((p + 1) * chunk_bytes);

	/* 1: X = PBKDF2(password, salt) */
	Y = YX.ptr;
	X = Y + chunk_bytes;
	scrypt_pbkdf2(password, password_len, salt, salt_len, 1, X, chunk_bytes * p);

	/* 2: X = ROMix(X) */
	for (i = 0; i < p; i++)
		scrypt_ROMix((scrypt_mix_word_t *)(X + (chunk_bytes * i)), (scrypt_mix_word_t *)Y, (scrypt_mix_word_t *)V.ptr, N, r);

	/* 3: Out = PBKDF2(password, X) */
	scrypt_pbkdf2(password, password_len, X, chunk_bytes * p, 1, out, bytes);

	scrypt_ensure_zero(YX.ptr, (p + 1) * chunk_bytes);

	scrypt_free(&V);
	scrypt_free(&YX);
}

#define ENSCRYPT_N 512
#define ENSCRYPT_R 256
#define ENSCRYPT_CHUNK_BYTES 32768
#define ENSCRYPT_YX_BYTES 65536
#define ENSCRYPT_V_BYTES 16777216

typedef struct enscrypt_context
{
	scrypt_aligned_alloc V;
	scrypt_aligned_alloc YX;
	uint8_t *Y, *X;
	scrypt_ROMixfn scrypt_ROMix;
	uint8_t *password;
	uint8_t *salt;
	uint8_t *out;
	size_t password_len;
	size_t salt_len;
} enscrypt_context;

void
enscrypt_end( enscrypt_context *ctx, uint8_t *buf )
{
	if( ctx ) {
		if( ctx->password ) {
			scrypt_ensure_zero( ctx->password, ctx->password_len );
			free( ctx->password );
			ctx->password = NULL;
			ctx->password_len = 0;
		}
		if( ctx->salt ) {
			free( ctx->salt );
			ctx->salt = NULL;
			ctx->salt_len = 0;
		}
		if( ctx->YX.mem ) {
			scrypt_ensure_zero( ctx->YX.ptr, ENSCRYPT_YX_BYTES );
			scrypt_free( &(ctx->YX) );
		}
		if( ctx->V.mem ) {
			scrypt_free( &(ctx->V) );
		}
		if( ctx->out ) {
			if( buf ) {
				memcpy( buf, ctx->out, 32 );
			}
			scrypt_ensure_zero( ctx->out, 32 );
			free( ctx->out );
			ctx->out = NULL;
		}
	}
}

bool enscrypt_begin( enscrypt_context *ctx, const char *password, const uint8_t *salt )
{
	if( !ctx ) return false;
	bool success = true;
	
	#if !defined(SCRYPT_CHOOSE_COMPILETIME)
	ctx->scrypt_ROMix = scrypt_getROMix();
	#else
	ctx->scrypt_ROMix = scrypt_ROMix;
	#endif
	
	#if !defined(SCRYPT_TEST)
	static int power_on_self_test = 0;
	if (!power_on_self_test) {
		power_on_self_test = 1;
		if (!scrypt_power_on_self_test())
			scrypt_fatal_error("scrypt: power on self test failed");
	}
	#endif

	ctx->V = scrypt_alloc(ENSCRYPT_V_BYTES);
	ctx->YX = scrypt_alloc(ENSCRYPT_YX_BYTES);
	ctx->Y = ctx->YX.ptr;
	ctx->X = ctx->Y + ENSCRYPT_CHUNK_BYTES;
	
	if( ctx->V.mem == NULL || ctx->YX.mem == NULL ) success = false;
	
	if( password && strlen( password ) > 0 ) {
		ctx->password_len = strlen( password );
		ctx->password = malloc( ctx->password_len + 1 );
		if( ctx->password ) {
			strcpy( (char*)ctx->password, password );
		} else {
			success = false;
		}
	} else {
		ctx->password = NULL;
		ctx->password_len = 0;
	}
	
	if( salt ) {
		ctx->salt = malloc( 32 );
		if( ctx->salt ) {
			memcpy( ctx->salt, salt, 32 );
			ctx->salt_len = 32;
		} else {
			success = false;
		}
	} else {
		ctx->salt = NULL;
		ctx->salt_len = 0;
	}

	ctx->out = calloc( 32, 1 );
	if( ctx->out == NULL ) {
		success = false;
	}
	if( success ) {
		return true;
	} else {
		enscrypt_end( ctx, NULL );
		return false;
	}
}

void
enscrypt_iterate( enscrypt_context *ctx ) {
	/* 1: X = PBKDF2(password, salt) */
	scrypt_pbkdf2(ctx->password, ctx->password_len, ctx->salt, ctx->salt_len, 1, ctx->X, ENSCRYPT_CHUNK_BYTES);
	
	/* 2: X = ROMix(X) */
	ctx->scrypt_ROMix((scrypt_mix_word_t *)ctx->X, (scrypt_mix_word_t *)ctx->Y, (scrypt_mix_word_t *)ctx->V.ptr, ENSCRYPT_N, ENSCRYPT_R);
	
	if( !ctx->salt ) {
		ctx->salt = malloc( 32 );
		ctx->salt_len = 32;
	}
	
	/* 3: Out = PBKDF2(password, X) */
	scrypt_pbkdf2(ctx->password, ctx->password_len, ctx->X, ENSCRYPT_CHUNK_BYTES, 1, ctx->salt, 32);
	
	((uint64_t*)ctx->out)[0] ^= ((uint64_t*)ctx->salt)[0];
	((uint64_t*)ctx->out)[1] ^= ((uint64_t*)ctx->salt)[1];
	((uint64_t*)ctx->out)[2] ^= ((uint64_t*)ctx->salt)[2];
	((uint64_t*)ctx->out)[3] ^= ((uint64_t*)ctx->salt)[3];
}

DLL_PUBLIC
int enscrypt_ms( uint8_t *buf, const char *passwd, const uint8_t *salt, int millis, enscrypt_progress_fn cb_ptr )
{
	enscrypt_context ctx;
	double startTime, elapsed = 0.0;
	int i = 0, p = 0, lp = -1;
	
	memset( &ctx, 0, sizeof( enscrypt_context ));
	if( !buf ) return 0;
	if( millis < 1 ) millis = 1;
	
	if( enscrypt_begin( &ctx, passwd, salt )) {
		startTime = enscrypt_get_real_time();
		while( elapsed < millis ) {
			if( cb_ptr ) {
				if( lp != ( p = elapsed / millis * 100 ) ) {
					(*cb_ptr)( p );
					lp = p;
				}
			}
			enscrypt_iterate( &ctx );
			elapsed = (enscrypt_get_real_time() - startTime) * 1000;
			i++;
		}
		if( cb_ptr ) (*cb_ptr)( 100 );
		enscrypt_end( &ctx, buf );
		return i;
	}
	return -1;
}

DLL_PUBLIC
int enscrypt(uint8_t *buf, const char *passwd, const uint8_t *salt, int iterations, enscrypt_progress_fn cb_ptr)
{
	enscrypt_context ctx;
	double i, startTime, endTime;
	int p = 0, lp = -1;
	
	memset( &ctx, 0, sizeof( enscrypt_context ));
	if( !buf ) return 0;
	if( iterations < 1 ) iterations = 1;
	
	if( enscrypt_begin( &ctx, passwd, salt )) {
		startTime = enscrypt_get_real_time();
		for( i = 0; i < iterations; i++ ) {
			if( cb_ptr ) {
				if( lp != (p= i / iterations * 100) ) {
					(*cb_ptr)( p );
					lp = p;
				}
			}
			enscrypt_iterate( &ctx );
		}
		if( cb_ptr ) (*cb_ptr)( 100 );
		endTime = enscrypt_get_real_time() - startTime;
		enscrypt_end( &ctx, buf );
		return (int)(endTime * 1000);
	}
	return -1;
}