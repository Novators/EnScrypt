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
#include <stdio.h>

#include "config.h"
#include "enscrypt.h"

#define SCRYPT_BLOCK_BYTES 64
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

#include "../scrypt/scrypt-jane-portable.h"
#include "../scrypt/scrypt-jane-hash.h"
#include "../scrypt/scrypt-jane-romix.h"

#include <stdio.h>
//#include <malloc.h>

static void //NORETURN
scrypt_fatal_error_default(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

static enscrypt_fatal_errorfn scrypt_fatal_error = scrypt_fatal_error_default;

DLL_PUBLIC
void enscrypt_set_fatal_error(enscrypt_fatal_errorfn fn) {
	scrypt_fatal_error = fn;
}

typedef struct scrypt_aligned_alloc_t {
	uint8_t *mem, *ptr;
} scrypt_aligned_alloc;

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

#define ENSCRYPT_R 256
#define ENSCRYPT_CHUNK_BYTES 32768
#define ENSCRYPT_YX_BYTES 65536
#define ENSCRYPT_V_BYTES 16777216

DLL_PUBLIC
int enscrypt(uint8_t *buf, const char *password, const uint8_t *saltIn, int iterations, int n_factor, enscrypt_progress_fn cb_ptr, void *cb_data )
{
	scrypt_ROMixfn scrypt_ROMix = scrypt_getROMix();
	uint8_t *salt = NULL;
	size_t password_len = password ? strlen(password) : 0;
	size_t salt_len = salt ? 32 : 0;
	
	double i, startTime, endTime;
	int p = 0, lp = -1, go = 1;
	
	if( !buf ) return -1;
	if( iterations < 1 ) return -1;
	if( n_factor < 0 || n_factor > scrypt_maxNfactor ) return -1;

	if( saltIn ) {
		salt = malloc( 32 );
		if( salt ) {
			memcpy( salt, saltIn, 32 );
			salt_len = 32;
		} else {
			return -1;
		}
	} else {
		salt = NULL;
		salt_len = 0;
	}
	
	
	scrypt_aligned_alloc V = scrypt_alloc( ENSCRYPT_V_BYTES );
	scrypt_aligned_alloc YX = scrypt_alloc( ENSCRYPT_YX_BYTES );
	uint8_t *Y = YX.ptr;
	uint8_t *X = Y + ENSCRYPT_CHUNK_BYTES;

	memset( buf, 0, 32 );
	startTime = enscrypt_get_real_time();
	for( i = 0; i < iterations; i++ ) {
		if( cb_ptr ) {
			if( lp != (p= (int)(i / iterations * 100))) {
				go = (*cb_ptr)( p, cb_data );
				lp = p;
			}
			if( go == 0 ) break;
		}
		/* 1: X = PBKDF2(password, salt) */
		scrypt_pbkdf2((uint8_t*)password, password_len, salt, salt_len, 1, X, ENSCRYPT_CHUNK_BYTES);
		
		/* 2: X = ROMix(X) */
		scrypt_ROMix((scrypt_mix_word_t *)X, (scrypt_mix_word_t *)Y, (scrypt_mix_word_t *)V.ptr, (1 << n_factor), ENSCRYPT_R);
		
		if( !salt ) {
			salt = malloc( 32 );
			salt_len = 32;
		}
		
		/* 3: Out = PBKDF2(password, X) */
		scrypt_pbkdf2((uint8_t*)password, password_len, X, ENSCRYPT_CHUNK_BYTES, 1, salt, 32);
		
		((uint64_t*)buf)[0] ^= ((uint64_t*)salt)[0];
		((uint64_t*)buf)[1] ^= ((uint64_t*)salt)[1];
		((uint64_t*)buf)[2] ^= ((uint64_t*)salt)[2];
		((uint64_t*)buf)[3] ^= ((uint64_t*)salt)[3];
	}

	if( go && cb_ptr ) (*cb_ptr)( 100, cb_data );
	endTime = enscrypt_get_real_time() - startTime;
	if( salt && salt != saltIn ) {
		free( salt );
	}
	if( YX.mem ) {
		scrypt_ensure_zero( YX.ptr, ENSCRYPT_YX_BYTES );
		scrypt_free( &YX );
	}
	if( V.mem ) {
		scrypt_free( &V );
	}
	if( go ) return (int)(endTime * 1000);
	return -1;
}

DLL_PUBLIC
int enscrypt_ms(uint8_t *buf, const char *password, const uint8_t *saltIn, int millis, int n_factor, enscrypt_progress_fn cb_ptr, void *cb_data )
{
	#if !defined(SCRYPT_CHOOSE_COMPILETIME)
	scrypt_ROMixfn scrypt_ROMix = scrypt_getROMix();
	#else
	scrypt_ROMixfn scrypt_ROMix = scrypt_ROMix;
	#endif
	uint8_t *salt = NULL;
	size_t password_len = password ? strlen(password) : 0;
	size_t salt_len = salt ? 32 : 0;
	
	double startTime, elapsed = 0.0;
	int i = 0;
	int p = 0, lp = -1, go = 1;
	
	if( !buf ) return -1;
	if( millis < 1 ) return -1;
	if( n_factor < 0 || n_factor > scrypt_maxNfactor ) return -1;
	
	if( saltIn ) {
		salt = malloc( 32 );
		if( salt ) {
			memcpy( salt, saltIn, 32 );
			salt_len = 32;
		} else {
			return -1;
		}
	} else {
		salt = NULL;
		salt_len = 0;
	}
	
	
	scrypt_aligned_alloc V = scrypt_alloc( ENSCRYPT_V_BYTES );
	scrypt_aligned_alloc YX = scrypt_alloc( ENSCRYPT_YX_BYTES );
	uint8_t *Y = YX.ptr;
	uint8_t *X = Y + ENSCRYPT_CHUNK_BYTES;
	
	memset( buf, 0, 32 );
	startTime = enscrypt_get_real_time();
	while( elapsed < millis ) {
		if( cb_ptr ) {
			if( lp != (p= (int)(elapsed / millis * 100))) {
				go = (*cb_ptr)( p, cb_data );
				lp = p;
			}
			if( go == 0 ) break;
		}
		/* 1: X = PBKDF2(password, salt) */
		scrypt_pbkdf2((uint8_t*)password, password_len, salt, salt_len, 1, X, ENSCRYPT_CHUNK_BYTES);
		
		/* 2: X = ROMix(X) */
		scrypt_ROMix((scrypt_mix_word_t *)X, (scrypt_mix_word_t *)Y, (scrypt_mix_word_t *)V.ptr, (1 << n_factor), ENSCRYPT_R);
		
		if( !salt ) {
			salt = malloc( 32 );
			salt_len = 32;
		}
		
		/* 3: Out = PBKDF2(password, X) */
		scrypt_pbkdf2((uint8_t*)password, password_len, X, ENSCRYPT_CHUNK_BYTES, 1, salt, 32);
		
		((uint64_t*)buf)[0] ^= ((uint64_t*)salt)[0];
		((uint64_t*)buf)[1] ^= ((uint64_t*)salt)[1];
		((uint64_t*)buf)[2] ^= ((uint64_t*)salt)[2];
		((uint64_t*)buf)[3] ^= ((uint64_t*)salt)[3];
		
		elapsed = (enscrypt_get_real_time() - startTime) * 1000;
		i++;
	}
	
	if( go && cb_ptr ) (*cb_ptr)( 100, cb_data );
	if( salt && salt != saltIn ) {
		free( salt );
	}
	if( YX.mem ) {
		scrypt_ensure_zero( YX.ptr, ENSCRYPT_YX_BYTES );
		scrypt_free( &YX );
	}
	if( V.mem ) {
		scrypt_free( &V );
	}
	if( go ) return i;
	return -1;
}
