/** @file enscrypt_bin.c
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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "config.h"
#include "enscrypt.h"


long duration = 0;
long iterations = 0;
uint8_t *salt = NULL;
char *password = NULL;
bool showHelp = false;
bool verbose = true;
int nextProgress = 2;

const char hexTable[32] = "0123456789abcdef0123456789ABCDEF";
const char decTable[10] = "0123456789";

#define VERSION_TEMPLATE "\
  EnScrypt: %s build %d\n"

char help[] = "\
     Usage: enscrypt [-q] [password] [salt] [iteration count] [duration] \n\
            enscrypt -h\n\
\n\
Option            Description\n\
----------------  -------------------------------------------------------\n\
duration          An integer (1..9999) + 's' or 'S'.  \n\
                  For example, \"7s\" == 7 seconds.\n\
iteration_count   An integer (1..9999) + 'i' or 'I'.  \n\
                  For example, \"100i\" == 100 iterations.\n\
                  If both are included, iteration_count\n\
                  takes precedence over duration.\n\
salt              A 64 character hex string representing a 32 byte salt.  \n\
                  Allowed characters are 0-9, a-f, and A-F.\n\
-q                Quiet.  Suppresses all output except the key.\n\
-h                Help.  Displays usage information.\n\
password          Any string not matching the above arguments.\n\n\
All arguments are optional, and order is arbitrary.";

void progress( int p )
{
	const char sym[] = "|****";
	while( p >= nextProgress ) {
		if( verbose ) {
			if( nextProgress == 100 ) {
				printf( "]" );
			} else {
				printf( "%c", sym[nextProgress%5] );
			}
		}
		nextProgress += 2;
	}
	fflush( stdout );
}

void hexify( char *out, const uint8_t *in )
{
	int i;
	for( i = 0; i < 32; i++ ) {
		out[i*2] = (char)hexTable[in[i]>>4];
		out[i*2+1] = (char)hexTable[in[i]&0x0F];
	}
	out[i*2] = 0;
}

void unhexify( uint8_t *out, const char *in )
{
	int i, a;
	char *p;
	memset( out, 0, 32 );
	for( i = 0; i < 64; i++ ) {
		p = strchr( hexTable, in[i] );
		if( p ) {
			a = (p - hexTable) % 16;
			out[i/2] |= (i%2) ? a : a<<4;
		}
	}
}

int main( int argc, char *argv[] )
{
	int i, j;
	char c;
	char str[65];
	uint8_t result[32];
	double startTime, endTime, elapsed;
	int retVal;
	
	for( i = 1; i < argc; i++ ) {
		j = strlen( argv[i] );
		if( j == 64 && strspn( argv[i], hexTable ) == 64 ) {
			if( salt ) free( salt );
			salt = malloc( 32 );
			unhexify( salt, argv[i] );
			continue;
		} else if( j > 1 ) {
			if( strspn( argv[i], decTable ) == j - 1 ) {
				c = argv[i][j-1];
				if( c == 's' || c == 'S' ) {
					if( iterations == 0 ) {
						duration = ( strtol( argv[i], NULL, 10 )) % 10000;
					}
					continue;
				} else if( c == 'i' || c == 'I' ) {
					iterations = ( strtol( argv[i], NULL, 10 )) % 10000;
					if( duration ) duration = 0;
					continue;
				}
			}
			if( argv[i][0] == '-' ) {
				c = argv[i][1];
				if( j == 2 ) {
					if( c == 'h' || c == 'H' ) {
						showHelp = true;
						continue;
					} else if( c == 'q' || c == 'Q' ) {
						verbose = false;
						continue;
					}
				}
			}
		}
		if( password ) free( password );
		password = malloc( strlen( argv[i] ) + 1 );
		strcpy( password, argv[i] );
	}

	if( verbose ) {
		printf( VERSION_TEMPLATE, ENSCRYPT_VERSION, ENSCRYPT_BUILD );
	}
	
	if( showHelp ) {
		printf( "%s\n", help );
		exit(0);
	}
	
	if( verbose ) {
		if( password ) {
			printf( "  Password: %s\n", password );
		} else {
			printf( "  Password: <null>\n" );
		}
		if( salt ) {
			hexify( str, salt );
			//str[65] = 0;
			printf( "      Salt: %s\n", str );
		} else {
			printf( "      Salt: <null>\n" );
		}
		if( iterations ) {
			printf( "Iterations: %ld\n", iterations );
		} else if( duration ) {
			printf( "  Duration: %ld seconds\n", duration );
		}
		printf( "   Percent: [   10%%  20%%  30%%  40%%  50%%  60%%  70%%  80%%  90%%   ]\n" );
		printf( "  Progress: [" );
		fflush( stdout );
	}

	startTime = enscrypt_get_real_time();
	if( iterations ) {
		retVal = enscrypt( result, password, salt, iterations, progress );
	} else if( duration ) {
		iterations = retVal = enscrypt_ms( result, password, salt, duration * 1000, progress );
	} else {
		iterations = 1;
		retVal = enscrypt( result, password, salt, iterations, progress );
	}
	endTime = enscrypt_get_real_time();
	elapsed = endTime - startTime;
	hexify( str, result );
	
	if( retVal > -1 ) {
		if( verbose ) {
			printf( "\n\nOutput Key: %s\n", str );
		} else {
			printf( "%s\n", str );
		}
		if( verbose ) {
			printf( "Iterations: %ld\n", iterations );
			printf( "   Elapsed: %.3f seconds\n\n", elapsed );
		}
	} else {
		if( verbose ) {
			printf( "Error running EnScrypt!\n" );
		}
		exit(1);
	}
	exit(0);
}