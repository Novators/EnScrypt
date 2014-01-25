#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "enscrypt.h"

long duration = 0;
long iterations = 0;
uint8_t *salt = NULL;
char *password = NULL;
bool needHelp = false;
bool verbose = true;
int nextProgress = 2;

const char hexTable[32] = "0123456789abcdef0123456789ABCDEF";
const char decTable[10] = "0123456789";

char version[] =
"EnScrypt password hashing utility \n\
Version 1.1 (compatible) \n\
Usage: enscrypt [password] [salt] [iteration count | duration] \n\
For detailed information: enscrypt -h";

char help[] =
"help";

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
	
	for( i = 1; i < argc; i++ ) {
		j = strlen( argv[i] );
		if( j == 64 && strspn( argv[i], hexTable ) == 64 ) {
			if( salt ) free( salt );
			salt = malloc( 32 );
			unhexify( salt, argv[i] );
			continue;
		}
		if( j > 1 ) {
			c = argv[i][j-1];
			switch( c ) {
				case 's':
				case 'S':
					duration = strtol( argv[i], &(argv[i]) + j - 1, 10 );
					continue;
				case 'i':
				case 'I':
					iterations = strtol( argv[i], &(argv[i]) + j - 1, 10 );
					continue;
			}
			if( argv[i][0] == '-' ) {
				if( j == 2 ) {
					switch( argv[i][1] ) {
						case 'h':
						case 'H':
							needHelp = true;
							continue;
						case 'q':
						case 'Q':
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
		printf( "%s\n\n", version );
	}
	
	if( needHelp ) {
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
		enscrypt( result, password, salt, iterations, progress );
	} else if( duration ) {
		iterations = enscrypt_ms( result, password, salt, duration * 1000, progress );
	} else {
		enscrypt( result, password, salt, 1, progress );
	}
	endTime = enscrypt_get_real_time();
	elapsed = endTime - startTime;
	hexify( str, result );
	
	if( verbose ) {
		printf( "\n\nOutput Key: %s\n", str );
	} else {
		printf( "%s\n", str );
	}
	if( verbose ) {
		printf( "Iterations: %ld\n", iterations );
		printf( "   Elapsed: %.3f seconds\n\n", elapsed );
	}
	
	exit(0);
}