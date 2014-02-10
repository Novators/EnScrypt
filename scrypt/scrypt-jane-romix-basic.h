#if !defined(SCRYPT_CHOOSE_COMPILETIME)
/* function type returned by scrypt_getROMix, used with cpu detection */
typedef void (FASTCALL *scrypt_ROMixfn)(scrypt_mix_word_t *X/*[chunkWords]*/, scrypt_mix_word_t *Y/*[chunkWords]*/, scrypt_mix_word_t *V/*[chunkWords * N]*/, uint32_t N, uint32_t r);
#endif

/* romix pre/post endian conversion function */
static void asm_calling_convention
scrypt_romix_convert_endian(scrypt_mix_word_t *blocks, size_t nblocks) {
#if !defined(CPU_LE)
	static const union { uint8_t b[2]; uint16_t w; } endian_test = {{1,0}};
	size_t i;
	if (endian_test.w == 0x100) {
		nblocks *= SCRYPT_BLOCK_WORDS;
		for (i = 0; i < nblocks; i++) {
			SCRYPT_WORD_ENDIAN_SWAP(blocks[i]);
		}
	}
#else
	(void)blocks; (void)nblocks;
#endif
}

/* chunkmix test function */
typedef void (asm_calling_convention *chunkmixfn)(scrypt_mix_word_t *Bout/*[chunkWords]*/, scrypt_mix_word_t *Bin/*[chunkWords]*/, scrypt_mix_word_t *Bxor/*[chunkWords]*/, uint32_t r);
typedef void (asm_calling_convention *blockfixfn)(scrypt_mix_word_t *blocks, size_t nblocks);

/* returns a pointer to item i, where item is len scrypt_mix_word_t's long */
static scrypt_mix_word_t *
scrypt_item(scrypt_mix_word_t *base, scrypt_mix_word_t i, scrypt_mix_word_t len) {
	return base + (i * len);
}

/* returns a pointer to block i */
static scrypt_mix_word_t *
scrypt_block(scrypt_mix_word_t *base, scrypt_mix_word_t i) {
	return base + (i * SCRYPT_BLOCK_WORDS);
}
