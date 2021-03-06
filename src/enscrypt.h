/** @file enscrypt.h 
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
 **/

#ifndef ENSCRYPT_H
#define ENSCRYPT_H

#if defined (_WIN32) 
	#if defined(enscrypt_EXPORTS)
		#define  ENSCRYPT_EXPORT __declspec(dllexport)
	#else
		#define  ENSCRYPT_EXPORT __declspec(dllimport)
	#endif /* enscrypt_EXPORTS */
#else /* defined (_WIN32) */
	#define ENSCRYPT_EXPORT
#endif


#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

/**
 * @brief Progress callback prototype
 * 
 * After each Scrypt iteration, if the progress of the enscrypt operation 
 * has increased by 1% or more, the progress callback will be called.
 * 
 * A long operation may result in 101 callbacks, while a single iteration
 * will only result in 2 (0% and 100%).
 * 
 * Note that the execution time of the callback will affect the iteration
 * count of the time based enscrypt function (enscrypt_ms), so treat this
 * as an interrupt function and return as quickly as possible.  You may,
 * for instance, pass off UI updates to another thread so that enscrypt
 * can continue immediately.
 *
 * @param  percent an integer from 0..100 inclusive, indicating percent of operation complete.
 * @param  data pointer to data for application use (or NULL).
 * @return Integer -- 0 to cancel operation, positive to continue.
 **/
typedef int (*enscrypt_progress_fn)(int percent, void* data);


/**
 * @brief Fatal Error function prototype
 *
 * @param msg string describing the error encountered.
 **/
typedef void (*enscrypt_fatal_errorfn)(const char *msg);

/**
 * @brief Set a function to call in the event of an un-recoverable error.
 * 
 * By default: print an error message to stderr, and exit(1).
 * 
 * Use this function to override the default.
 *
 * @param fn pointer to function of type enscrypt_fatal_errorfn
 **/
ENSCRYPT_EXPORT void enscrypt_set_fatal_error(enscrypt_fatal_errorfn fn);


/**
 * @brief  Get a value representing execution time
 *
 * Time is measured since an arbitrary and OS-dependent start time.
 * The returned real time is only useful for computing an elapsed time
 * between two calls to this function.
 * 
 *
 * @return time in seconds, or -1.0 on error
 **/
ENSCRYPT_EXPORT double enscrypt_get_real_time();

/**
 * @brief Iteration based enscrypt
 *
 * @param buf A 32 byte buffer to receive the result.
 * @param passwd buffer containing password, or NULL.
 * @param passwd_len length of password.
 * @param salt buffer containing the salt, or NULL.
 * @param salt_len length of salt.
 * @param iterations Number of iterations; must be > 0.
 * @param n_factor Scrypt n factor (recommend 9)
 * @param cb_ptr Pointer to an enscrypt_progress_fn, or NULL.
 * @param cb_data Pointer to data to send to callback function.
 * @return Execution time (in milliseconds) or -1 on error.
 **/
ENSCRYPT_EXPORT int enscrypt(
	uint8_t *buf, 
	const char *passwd,
	size_t passwd_len, 
	const uint8_t *salt, 
	size_t salt_len,
	int iterations, 
	int n_factor, 
	enscrypt_progress_fn cb_ptr, 
	void* cb_data );

/**
 * @brief Time based enscrypt
 *
 * @param buf A 32 byte buffer to receive the result.
 * @param passwd null-terminated password, or NULL.
 * @param salt 32 byte buffer containing the salt, or NULL.
 * @param millis Milliseconds to compute.  Must be > 0.
 * @param n_factor Scrypt n factor (recommend 9)
 * @param cb_ptr Pointer to an enscrypt_progress_fn, or NULL.
 * @param cb_data Pointer to data to send to callback function.
 * @return Number of iterations performed or -1 on error.
 **/
ENSCRYPT_EXPORT int enscrypt_ms( 
	uint8_t *buf,
	const char *passwd,
	size_t passwd_len,
	const uint8_t *salt,
	size_t salt_len,
	int millis,
	int n_factor,
	enscrypt_progress_fn cb_ptr,
	void* cb_data );

#ifdef __cplusplus
}
#endif

#endif /* ENSCRYPT_H */
