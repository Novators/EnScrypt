#include "config.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "enscrypt.h"

const char testNames[5][20] = 
{ "1i", "100i", "password 123i", "password {0} 123i", "1000i" };
const int testIterations[5] = {1,100,123,123,1000};
const char *testPasswords[5] = { NULL, NULL, "password", "password", NULL };
const bool testSalts[5] = { false, false, false, true, false };
const char testResults[5][65] = 
{
	"a8ea62a6e1bfd20e4275011595307aa302645c1801600ef5cd79bf9d884d911c",
	"45a42a01709a0012a37b7b6874cf16623543409d19e7740ed96741d2e99aab67",
	"129d96d1e735618517259416a605be7094c2856a53c14ef7d4e4ba8e4ea36aeb",
	"2f30b9d4e5c48056177ff90a6cc9da04b648a7e8451dfa60da56c148187f6a7d",
	"3f671adf47d2b1744b1bf9b50248cc71f2a58e8d2b43c76edb1d2a2c200907f5"
};

void hexify( char *result, const uint8_t *buf )
{
	const char tab[16] = "0123456789abcdef";
	int i;
	for( i = 0; i < 32; i++ ) {
		result[i*2] = (char)tab[buf[i]>>4];
		result[i*2+1] = (char)tab[buf[i]&0x0F];
	}
}

bool test_enscrypt()
{
	double startTime, endTime;
	int totalTime;
	char result[65] = {0};
	int elapsed = 0;
	int iter = 0;
	int i;
	uint8_t buf[32], buf2[32];
	uint8_t nullsalt[32] = {0};
	
	startTime = enscrypt_get_real_time();
	printf( "Repeatability (2 x 2s): " );
	fflush( stdout );
	iter = enscrypt_ms( buf, NULL, NULL, 2000, NULL, NULL );
	elapsed = enscrypt( buf2, NULL, NULL, iter, NULL, NULL ) * 2;
	if( memcmp( buf, buf2, 32 ) == 0 ) {
		printf( "PASSED (~%2d.%03d seconds)\n", elapsed / 1000, elapsed % 1000 );
	} else {
		printf( "FAILED (~%2d.%03d seconds)\n", elapsed / 1000, elapsed % 1000 );
		hexify( result, buf );
		printf( "%s\n", result );
		hexify( result, buf2 );
		printf( "%s\n", result );
		return false;
	}
	
	for( i = 0; i < 5; i++ ) {
		printf( "%22s: ", testNames[i] );
		fflush( stdout );
		elapsed = enscrypt( buf, testPasswords[i], testSalts[i] ? nullsalt : NULL, testIterations[i], NULL, NULL );
		hexify( result, buf );
		if( memcmp( result, testResults[i], 64 ) == 0 ) {
			printf( "PASSED (%3d.%03d seconds)\n", elapsed / 1000, elapsed % 1000 );
		} else {
			printf( "FAILED (%3d.%03d seconds)\n", elapsed / 1000, elapsed % 1000 );
			printf( "EXPECTED: %s\n", testResults[i] );
			printf( "RECEIVED: %s\n", result );
			return false;
		}
	}
	endTime = enscrypt_get_real_time();
	totalTime = (endTime - startTime) * 1000;
	printf( "%22s: PASSED (%3d.%3d seconds)\n", "All Tests", totalTime / 1000, totalTime % 1000 );
	
	return true;
}

int main(void)
{
	srand(time(NULL));
	if( test_enscrypt() ) {
		exit(0);
	}
	exit(1);
}
