#ifndef ENSCRYPT_H
#define ENSCRYPT_H

/*
	Nfactor: Increases CPU & Memory Hardness
	N = (1 << (Nfactor + 1)): How many times to mix a chunk and how many temporary chunks are used

	rfactor: Increases Memory Hardness
	r = (1 << rfactor): How large a chunk is

	pfactor: Increases CPU Hardness
	p = (1 << pfactor): Number of times to mix the main chunk

	A block is the basic mixing unit (salsa/chacha block = 64 bytes)
	A chunk is (2 * r) blocks

	~Memory used = (N + 2) * ((2 * r) * block size)
*/

#include <stdlib.h>
#include <stdint.h>
#include "config.h"

typedef void (*sqrl_progress_fn)(int);

typedef void (*scrypt_fatal_errorfn)(const char *msg);
void scrypt_set_fatal_error(scrypt_fatal_errorfn fn);

double getRealTime();
void scrypt(const unsigned char *password, size_t password_len, const unsigned char *salt, size_t salt_len, unsigned char Nfactor, unsigned char rfactor, unsigned char pfactor, unsigned char *out, size_t bytes);
int enscrypt(uint8_t *, const char *, const uint8_t *, int, sqrl_progress_fn);
int enscrypt_millis( uint8_t *, const char *, const uint8_t *, int, sqrl_progress_fn );

#endif /* ENSCRYPT_H */
