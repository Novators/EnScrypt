#include "config.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "enscrypt.h"

void hexify( char *result, const uint8_t *buf )
{
	const char tab[16] = "0123456789abcdef";
	int i;
	for( i = 0; i < 32; i++ ) {
		result[i*2] = (char)tab[buf[i]>>4];
		result[i*2+1] = (char)tab[buf[i]&0x0F];
	}
}

void progress( int percent )
{
	printf( "%02d ", percent );
	fflush( stdout );
}

bool test_enscrypt()
{
	char result[65] = {0};
	int elapsed = 0;
	int iter = 0;
	uint8_t buf[32], buf2[32];
	uint8_t nullsalt[32] = {0};
	
	iter = enscrypt_millis( buf, NULL, NULL, 2000, progress );
	hexify( result, buf );
	printf( "\nNULL, NULL, 2000ms (%d iterations)\n%s\n\n", iter, result );
	elapsed = enscrypt( buf2, NULL, NULL, iter, progress );
	hexify( result, buf2 );
	printf( "\nNULL, NULL, %d (%d ms)\n%s\n\n", iter, elapsed, result );
	if( memcmp( buf, buf2, 32 ) != 0 ) {
		return false;
	}
	
	elapsed = enscrypt( buf, NULL, NULL, 1, progress );
	hexify( result, buf );
	printf( "\nNULL, NULL, 1 (%d ms)\n%s\n\n", elapsed, result );
	if( memcmp( result, "a8ea62a6e1bfd20e4275011595307aa302645c1801600ef5cd79bf9d884d911c", 64 ) != 0 ) {
		return false;
	}
	return true;
	elapsed = enscrypt( buf, "", NULL, 100, progress );
	hexify( result, buf );
	printf( "\n\"\", NULL, 100 (%d ms)\n%s\n\n", elapsed, result );
	if( memcmp( result, "45a42a01709a0012a37b7b6874cf16623543409d19e7740ed96741d2e99aab67", 64 ) != 0 ) {
		return false;
	}
	elapsed = enscrypt( buf, "password", NULL, 123, progress );
	hexify( result, buf );
	printf( "\n\"password\", NULL, 123 (%d ms)\n%s\n\n", elapsed, result );
	if( memcmp( result, "129d96d1e735618517259416a605be7094c2856a53c14ef7d4e4ba8e4ea36aeb", 64 ) != 0 ) {
		return false;
	}
	elapsed = enscrypt( buf, "password", nullsalt, 123, progress );
	hexify( result, buf );
	printf( "\n\"password\", {0}, 123 (%d ms)\n%s\n\n", elapsed, result );
	if( memcmp( result, "2f30b9d4e5c48056177ff90a6cc9da04b648a7e8451dfa60da56c148187f6a7d", 64 ) != 0 ) {
		return false;
	}
	elapsed = enscrypt( buf, "", NULL, 1000, progress );
	hexify( result, buf );
	printf( "\n\"\", NULL, 1000 (%d ms)\n%s\n", elapsed, result );
	if( memcmp( result, "3f671adf47d2b1744b1bf9b50248cc71f2a58e8d2b43c76edb1d2a2c200907f5", 64 ) != 0 ) {
		return false;
	}
	return true;
}

int main(void)
{
	srand(time(NULL));
	if( test_enscrypt() ) {
		fprintf( stdout, "Passed!\n\n" );
		exit(0);
	}
	exit(1);
}
