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
 **/
typedef void (*enscrypt_progress_fn)(int percent);


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
void enscrypt_set_fatal_error(enscrypt_fatal_errorfn fn);


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
double enscrypt_get_real_time();

/**
 * @brief The original Scrypt PBKDF
 *
 * @param password string containing the password.
 * @param password_len length of password.
 * @param salt pointer to unsigned char array to use as salt.
 * @param salt_len length of salt.
 * @param Nfactor N = (1 << (Nfactor + 1))
 * @param rfactor r = (1 << rfactor)
 * @param pfactor p = (1 << pfactor)
 * @param out pointer to buffer receive result.
 * @param bytes Number of bytes to write to out.
 **/
void enscrypt_scrypt(const unsigned char *password, size_t password_len, const unsigned char *salt, size_t salt_len, unsigned char Nfactor, unsigned char rfactor, unsigned char pfactor, unsigned char *out, size_t bytes);

/**
 * @brief Iteration based enscrypt
 *
 * @param buf A 32 byte buffer to receive the result.
 * @param passwd null-terminated password, or NULL.
 * @param salt 32 byte buffer containing the salt, or NULL.
 * @param iterations Number of iterations; must be > 0.
 * @param cb_ptr Pointer to an enscrypt_progress_fn, or NULL.
 * @return Execution time (in milliseconds) or -1 on error.
 **/
int enscrypt(uint8_t *buf, const char *passwd, const uint8_t *salt, int iterations, enscrypt_progress_fn cb_ptr );

/**
 * @brief Time based enscrypt
 *
 * @param buf A 32 byte buffer to receive the result.
 * @param passwd null-terminated password, or NULL.
 * @param salt 32 byte buffer containing the salt, or NULL.
 * @param millis Milliseconds to compute.  Must be > 0.
 * @param cb_ptr Pointer to an enscrypt_progress_fn, or NULL.
 * @return Number of iterations performed or -1 on error.
 **/
int enscrypt_ms( uint8_t *buf, const char *passwd, const uint8_t *salt, int millis, enscrypt_progress_fn cb_ptr );

#ifdef __cplusplus
}
#endif

#endif /* ENSCRYPT_H */
