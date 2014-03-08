/* x86 */
#if defined(X86ASM_SSE2) && (!defined(SCRYPT_CHOOSE_COMPILETIME) || !defined(SCRYPT_SALSA_INCLUDED)) && !defined(CPU_X86_FORCE_INTRINSICS)

#define SCRYPT_SALSA_SSE2

asm_naked_fn_proto(void, scrypt_ChunkMix_sse2)(uint32_t *Bout/*[chunkBytes]*/, uint32_t *Bin/*[chunkBytes]*/, uint32_t *Bxor/*[chunkBytes]*/, uint32_t r)
asm_naked_fn(scrypt_ChunkMix_sse2)
	a1(push %ebx)
	a1(push %edi)
	a1(push %esi)
	a1(push %ebp)
	a2(mov %esp, %ebp)
	a2(mov 20(%ebp), %edi)
	a2(mov 24(%ebp), %esi)
	a2(mov 28(%ebp), %eax)
	a2(mov 32(%ebp), %ebx)
	a2(sub $32, %esp)
	a2(and $-192, %esp)
	a2(lea (,%ebx,2), %edx)
	a2(shl $6, %edx)
	a2(lea -64(%edx), %ecx)
	a2(and %eax, %eax)
	a2(movdqa (%ecx,%esi), %xmm0)
	a2(movdqa 16(%ecx,%esi), %xmm1)
	a2(movdqa 32(%ecx,%esi), %xmm2)
	a2(movdqa 48(%ecx,%esi), %xmm3)
	a1(jz scrypt_ChunkMix_sse2_no_xor1)
	a2(pxor (%ecx, %eax), %xmm0)
    a2(pxor 16(%ecx, %eax), %xmm1)
    a2(pxor 32(%ecx, %eax), %xmm2)
    a2(pxor 48(%ecx, %eax), %xmm3)
	a1(scrypt_ChunkMix_sse2_no_xor1:)
	a2(xor %ecx, %ecx)
	a2(xor %ebx, %ebx)
	a1(scrypt_ChunkMix_sse2_loop:)
		a2(and %eax, %eax)
        a2(pxor (%esi,%ecx), %xmm0)
        a2(pxor 16(%esi,%ecx), %xmm1)
        a2(pxor 32(%esi,%ecx), %xmm2)
        a2(pxor 48(%esi,%ecx), %xmm3)
		a1(jz scrypt_ChunkMix_sse2_no_xor2)
		a2(pxor (%eax,%ecx), %xmm0)
        a2(pxor 16(%eax,%ecx), %xmm1)
        a2(pxor 32(%eax,%ecx), %xmm2)
        a2(pxor 48(%eax,%ecx), %xmm3)
		a1(scrypt_ChunkMix_sse2_no_xor2:)
		a2(movdqa %xmm0, (%esp))
		a2(movdqa %xmm1, 16(%esp))
		a2(movdqa %xmm2,%xmm6)
		a2(movdqa %xmm3,%xmm7)
		a2(mov $8, %eax)
		a1(scrypt_salsa_sse2_loop: )
			a2(movdqa %xmm1, %xmm4)
			a2(paddd %xmm0, %xmm4)
			a2(movdqa %xmm4, %xmm5)
			a2(pslld $7, %xmm4)
			a2(psrld $25, %xmm5)
			a2(pxor %xmm4, %xmm3)
			a2(movdqa %xmm0, %xmm4)
			a2(pxor %xmm5, %xmm3)
			a2(paddd %xmm3, %xmm4)
			a2(movdqa %xmm4, %xmm5)
			a2(pslld $9, %xmm4)
			a2(psrld $23, %xmm5)
			a2(pxor %xmm4, %xmm2)
			a2(movdqa %xmm3, %xmm4)
			a2(pxor %xmm5, %xmm2)
			a3(pshufd $0x93, %xmm3, %xmm3)
			a2(paddd %xmm2, %xmm4)
			a2(movdqa %xmm4, %xmm5)
			a2(pslld $13, %xmm4)
			a2(psrld $19, %xmm5)
			a2(pxor %xmm4, %xmm1)
			a2(movdqa %xmm2, %xmm4)
			a2(pxor %xmm5, %xmm1)
			a3(pshufd $0x4e, %xmm2, %xmm2)
			a2(paddd %xmm1, %xmm4)
			a2(movdqa %xmm4, %xmm5)
			a2(pslld $18, %xmm4)
			a2(psrld $14, %xmm5)
			a2(pxor %xmm4, %xmm0)
			a2(movdqa %xmm3, %xmm4)
			a2(pxor %xmm5, %xmm0)
			a3(pshufd $0x39, %xmm1, %xmm1)
			a2(paddd %xmm0, %xmm4)
			a2(movdqa %xmm4, %xmm5)
			a2(pslld $7, %xmm4)
			a2(psrld $25, %xmm5)
			a2(pxor %xmm4, %xmm1)
			a2(movdqa %xmm0, %xmm4)
			a2(pxor %xmm5, %xmm1)
			a2(paddd %xmm1, %xmm4)
			a2(movdqa %xmm4, %xmm5)
			a2(pslld $9, %xmm4)
			a2(psrld $23, %xmm5)
			a2(pxor %xmm4, %xmm2)
			a2(movdqa %xmm1, %xmm4)
			a2(pxor %xmm5, %xmm2)
			a3(pshufd $0x93, %xmm1, %xmm1)
			a2(paddd %xmm2, %xmm4)
			a2(movdqa %xmm4, %xmm5)
			a2(pslld $13, %xmm4)
			a2(psrld $19, %xmm5)
			a2(pxor %xmm4, %xmm3)
			a2(movdqa %xmm2, %xmm4)
			a2(pxor %xmm5, %xmm3)
            a3(pshufd $0x4e, %xmm2, %xmm2)
			a2(paddd %xmm3, %xmm4)
			a2(sub $2, %eax)
			a2(movdqa %xmm4, %xmm5)
			a2(pslld $18, %xmm4)
			a2(psrld $14, %xmm5)
			a2(pxor %xmm4, %xmm0)
			a3(pshufd $0x39, %xmm3, %xmm3)
			a2(pxor %xmm5, %xmm0)
			aj(ja scrypt_salsa_sse2_loop)
		a2(paddd (%esp), %xmm0)
		a2(paddd 16(%esp), %xmm1)
		a2(paddd %xmm6,%xmm2)
		a2(paddd %xmm7,%xmm3)
		a2(lea (%ebx,%ecx), %eax)
		a2(xor %edx, %ebx)
		a2(and $-128, %eax)
		a2(add $64, %ecx)
		a2(shr $1, %eax)
		a2(add %edi, %eax)
		a2(cmp %edx, %ecx)
		a2(movdqa %xmm0, (%eax))
		a2(movdqa %xmm1, 16(%eax))
		a2(movdqa %xmm2, 32(%eax))
		a2(movdqa %xmm3, 48(%eax))
		a2(mov 28(%ebp), %eax)
		a1(jne scrypt_ChunkMix_sse2_loop)
	a2(mov %ebp, %esp)
	a1(pop %ebp)
	a1(pop %esi)
	a1(pop %edi)
	a1(pop %ebx)
	aret($16)
asm_naked_fn_end(scrypt_ChunkMix_sse2)

#endif



/* x64 */
#if defined(X86_64ASM_SSE2) && (!defined(SCRYPT_CHOOSE_COMPILETIME) || !defined(SCRYPT_SALSA_INCLUDED)) && !defined(CPU_X86_FORCE_INTRINSICS)

#define SCRYPT_SALSA_SSE2

asm_naked_fn_proto(void, scrypt_ChunkMix_sse2)(uint32_t *Bout/*[chunkBytes]*/, uint32_t *Bin/*[chunkBytes]*/, uint32_t *Bxor/*[chunkBytes]*/, uint32_t r)
asm_naked_fn(scrypt_ChunkMix_sse2)
    a2(lea (,%ecx,2), %rcx)
	a2(shl $6, %rcx)
    a2(lea -64(%rcx), %r9)
    a2(lea (%rsi,%r9), %rax)
    a2(lea (%rdx,%r9), %r9)
    a2(and %rdx, %rdx)
    a2(movdqa 0(%rax), %xmm0)
    a2(movdqa 16(%rax), %xmm1)
    a2(movdqa 32(%rax), %xmm2)
    a2(movdqa 48(%rax), %xmm3)
    a1(jz scrypt_ChunkMix_sse2_no_xor1)
    a2(pxor (%r9), %xmm0)
    a2(pxor 16(%r9), %xmm1)
    a2(pxor 32(%r9), %xmm2)
    a2(pxor 48(%r9), %xmm3)
    a1(scrypt_ChunkMix_sse2_no_xor1:)
        a2(xor %r9, %r9)
        a2(xor %r8, %r8)
    a1(scrypt_ChunkMix_sse2_loop:)
        a2(and %rdx, %rdx)
        a2(pxor (%rsi,%r9,1), %xmm0)
        a2(pxor 16(%rsi,%r9,1), %xmm1)
        a2(pxor 32(%rsi,%r9,1), %xmm2)
        a2(pxor 48(%rsi,%r9,1), %xmm3)
        a1(jz scrypt_ChunkMix_sse2_no_xor2)
        a2(pxor (%rdx, %r9, 1), %xmm0)
        a2(pxor 16(%rdx, %r9, 1), %xmm1)
        a2(pxor 32(%rdx, %r9, 1), %xmm2)
        a2(pxor 48(%rdx, %r9, 1), %xmm3)
    a1(scrypt_ChunkMix_sse2_no_xor2:)
        a2(movdqa %xmm0, %xmm8)
        a2(movdqa %xmm1, %xmm9)
        a2(movdqa %xmm2, %xmm10)
        a2(movdqa %xmm3, %xmm11)
        a2(mov $8, %rax)
    a1(scrypt_salsa_sse2_loop: )
        a2(movdqa %xmm1, %xmm4)
        a2(paddd %xmm0, %xmm4)
        a2(movdqa %xmm4, %xmm5)
        a2(pslld $7, %xmm4)
        a2(psrld $25, %xmm5)
        a2(pxor %xmm4, %xmm3)
        a2(movdqa %xmm0, %xmm4)
        a2(pxor %xmm5, %xmm3)
        a2(paddd %xmm3, %xmm4)
        a2(movdqa %xmm4, %xmm5)
        a2(pslld $9, %xmm4)
        a2(psrld $23, %xmm5)
        a2(pxor %xmm4, %xmm2)
        a2(movdqa %xmm3, %xmm4)
        a2(pxor %xmm5, %xmm2)
        a3(pshufd $0x93, %xmm3, %xmm3)
        a2(paddd %xmm2, %xmm4)
        a2(movdqa %xmm4, %xmm5)
        a2(pslld $13, %xmm4)
        a2(psrld $19, %xmm5)
        a2(pxor %xmm4, %xmm1)
        a2(movdqa %xmm2, %xmm4)
        a2(pxor %xmm5, %xmm1)
        a3(pshufd $0x4e, %xmm2, %xmm2)
        a2(paddd %xmm1, %xmm4)
        a2(movdqa %xmm4, %xmm5)
        a2(pslld $18, %xmm4)
        a2(psrld $14, %xmm5)
        a2(pxor %xmm4, %xmm0)
        a2(movdqa %xmm3, %xmm4)
        a2(pxor %xmm5, %xmm0)
        a3(pshufd $0x39, %xmm1, %xmm1)
        a2(paddd %xmm0, %xmm4)
        a2(movdqa %xmm4, %xmm5)
        a2(pslld $7, %xmm4)
        a2(psrld $25, %xmm5)
        a2(pxor %xmm4, %xmm1)
        a2(movdqa %xmm0, %xmm4)
        a2(pxor %xmm5, %xmm1)
        a2(paddd %xmm1, %xmm4)
        a2(movdqa %xmm4, %xmm5)
        a2(pslld $9, %xmm4)
        a2(psrld $23, %xmm5)
        a2(pxor %xmm4, %xmm2)
        a2(movdqa %xmm1, %xmm4)
        a2(pxor %xmm5, %xmm2)
        a3(pshufd $0x93, %xmm1, %xmm1)
        a2(paddd %xmm2, %xmm4)
        a2(movdqa %xmm4, %xmm5)
        a2(pslld $13, %xmm4)
        a2(psrld $19, %xmm5)
        a2(pxor %xmm4, %xmm3)
        a2(movdqa %xmm2, %xmm4)
        a2(pxor %xmm5, %xmm3)
        a3(pshufd $0x4e, %xmm2, %xmm2)
        a2(paddd %xmm3, %xmm4)
        a2(sub $2, %rax)
        a2(movdqa %xmm4, %xmm5)
        a2(pslld $18, %xmm4)
        a2(psrld $14, %xmm5)
        a2(pxor %xmm4, %xmm0)
        a3(pshufd $0x39, %xmm3, %xmm3)
        a2(pxor %xmm5, %xmm0)
        a1(ja scrypt_salsa_sse2_loop)

    a2(paddd %xmm8,%xmm0)
    a2(paddd %xmm9,%xmm1)
    a2(paddd %xmm10,%xmm2)
    a2(paddd %xmm11,%xmm3)
    a2(lea (%r8,%r9),%rax)
    a2(xor %rcx, %r8)
    a2(and $-128, %rax)
    a2(add $64,%r9)
    a2(shr $1,%rax)
    a2(add %rdi,%rax)
    a2(cmp %rcx,%r9)
    a2(movdqa %xmm0,(%rax))
    a2(movdqa %xmm1,16(%rax))
    a2(movdqa %xmm2,32(%rax))
    a2(movdqa %xmm3,48(%rax))

    a1(jne scrypt_ChunkMix_sse2_loop)
    a1(ret)
    asm_naked_fn_end(scrypt_ChunkMix_sse2)

#endif


/* intrinsic */
#if defined(X86_INTRINSIC_SSE2) && (!defined(SCRYPT_CHOOSE_COMPILETIME) || !defined(SCRYPT_SALSA_INCLUDED))

#define SCRYPT_SALSA_SSE2

static void NOINLINE asm_calling_convention
scrypt_ChunkMix_sse2(uint32_t *Bout/*[chunkBytes]*/, uint32_t *Bin/*[chunkBytes]*/, uint32_t *Bxor/*[chunkBytes]*/, uint32_t r) {
	uint32_t i, blocksPerChunk = r * 2, half = 0;
	xmmi *xmmp,x0,x1,x2,x3,x4,x5,t0,t1,t2,t3;
	size_t rounds;

	/* 1: X = B_{2r - 1} */
	xmmp = (xmmi *)scrypt_block(Bin, blocksPerChunk - 1);
	x0 = xmmp[0];
	x1 = xmmp[1];
	x2 = xmmp[2];
	x3 = xmmp[3];

	if (Bxor) {
		xmmp = (xmmi *)scrypt_block(Bxor, blocksPerChunk - 1);
		x0 = _mm_xor_si128(x0, xmmp[0]);
		x1 = _mm_xor_si128(x1, xmmp[1]);
		x2 = _mm_xor_si128(x2, xmmp[2]);
		x3 = _mm_xor_si128(x3, xmmp[3]);
	}

	/* 2: for i = 0 to 2r - 1 do */
	for (i = 0; i < blocksPerChunk; i++, half ^= r) {
		/* 3: X = H(X ^ B_i) */
		xmmp = (xmmi *)scrypt_block(Bin, i);
		x0 = _mm_xor_si128(x0, xmmp[0]);
		x1 = _mm_xor_si128(x1, xmmp[1]);
		x2 = _mm_xor_si128(x2, xmmp[2]);
		x3 = _mm_xor_si128(x3, xmmp[3]);

		if (Bxor) {
			xmmp = (xmmi *)scrypt_block(Bxor, i);
			x0 = _mm_xor_si128(x0, xmmp[0]);
			x1 = _mm_xor_si128(x1, xmmp[1]);
			x2 = _mm_xor_si128(x2, xmmp[2]);
			x3 = _mm_xor_si128(x3, xmmp[3]);
		}

		t0 = x0;
		t1 = x1;
		t2 = x2;
		t3 = x3;

		for (rounds = 8; rounds; rounds -= 2) {
			x4 = x1;
			x4 = _mm_add_epi32(x4, x0);
			x5 = x4;
			x4 = _mm_slli_epi32(x4, 7);
			x5 = _mm_srli_epi32(x5, 25);
			x3 = _mm_xor_si128(x3, x4);
			x4 = x0;
			x3 = _mm_xor_si128(x3, x5);
			x4 = _mm_add_epi32(x4, x3);
			x5 = x4;
			x4 = _mm_slli_epi32(x4, 9);
			x5 = _mm_srli_epi32(x5, 23);
			x2 = _mm_xor_si128(x2, x4);
			x4 = x3;
			x2 = _mm_xor_si128(x2, x5);
			x3 = _mm_shuffle_epi32(x3, 0x93);
			x4 = _mm_add_epi32(x4, x2);
			x5 = x4;
			x4 = _mm_slli_epi32(x4, 13);
			x5 = _mm_srli_epi32(x5, 19);
			x1 = _mm_xor_si128(x1, x4);
			x4 = x2;
			x1 = _mm_xor_si128(x1, x5);
			x2 = _mm_shuffle_epi32(x2, 0x4e);
			x4 = _mm_add_epi32(x4, x1);
			x5 = x4;
			x4 = _mm_slli_epi32(x4, 18);
			x5 = _mm_srli_epi32(x5, 14);
			x0 = _mm_xor_si128(x0, x4);
			x4 = x3;
			x0 = _mm_xor_si128(x0, x5);
			x1 = _mm_shuffle_epi32(x1, 0x39);
			x4 = _mm_add_epi32(x4, x0);
			x5 = x4;
			x4 = _mm_slli_epi32(x4, 7);
			x5 = _mm_srli_epi32(x5, 25);
			x1 = _mm_xor_si128(x1, x4);
			x4 = x0;
			x1 = _mm_xor_si128(x1, x5);
			x4 = _mm_add_epi32(x4, x1);
			x5 = x4;
			x4 = _mm_slli_epi32(x4, 9);
			x5 = _mm_srli_epi32(x5, 23);
			x2 = _mm_xor_si128(x2, x4);
			x4 = x1;
			x2 = _mm_xor_si128(x2, x5);
			x1 = _mm_shuffle_epi32(x1, 0x93);
			x4 = _mm_add_epi32(x4, x2);
			x5 = x4;
			x4 = _mm_slli_epi32(x4, 13);
			x5 = _mm_srli_epi32(x5, 19);
			x3 = _mm_xor_si128(x3, x4);
			x4 = x2;
			x3 = _mm_xor_si128(x3, x5);
			x2 = _mm_shuffle_epi32(x2, 0x4e);
			x4 = _mm_add_epi32(x4, x3);
			x5 = x4;
			x4 = _mm_slli_epi32(x4, 18);
			x5 = _mm_srli_epi32(x5, 14);
			x0 = _mm_xor_si128(x0, x4);
			x3 = _mm_shuffle_epi32(x3, 0x39);
			x0 = _mm_xor_si128(x0, x5);
		}

		x0 = _mm_add_epi32(x0, t0);
		x1 = _mm_add_epi32(x1, t1);
		x2 = _mm_add_epi32(x2, t2);
		x3 = _mm_add_epi32(x3, t3);

		/* 4: Y_i = X */
		/* 6: B'[0..r-1] = Y_even */
		/* 6: B'[r..2r-1] = Y_odd */
		xmmp = (xmmi *)scrypt_block(Bout, (i / 2) + half);
		xmmp[0] = x0;
		xmmp[1] = x1;
		xmmp[2] = x2;
		xmmp[3] = x3;
	}
}

#endif

#if defined(SCRYPT_SALSA_SSE2)
	#undef SCRYPT_MIX
	#define SCRYPT_MIX "Salsa/8-SSE2"
	#undef SCRYPT_SALSA_INCLUDED
	#define SCRYPT_SALSA_INCLUDED
#endif

/* used by avx,etc as well */
#if defined(SCRYPT_SALSA_SSE2) || defined(SCRYPT_SALSA_AVX) || defined(SCRYPT_SALSA_XOP)
/*
	Default layout:
		0  1  2  3
		4  5  6  7
		8  9 10 11
	12 13 14 15

	SSE2 layout:
		0  5 10 15
	12  1  6 11
		8 13  2  7
		4  9 14  3
*/

static void asm_calling_convention
salsa_core_tangle_sse2(uint32_t *blocks, size_t count) {
	uint32_t t;
	while (count--) {
		t = blocks[1]; blocks[1] = blocks[5]; blocks[5] = t;
		t = blocks[2]; blocks[2] = blocks[10]; blocks[10] = t;
		t = blocks[3]; blocks[3] = blocks[15]; blocks[15] = t;
		t = blocks[4]; blocks[4] = blocks[12]; blocks[12] = t;
		t = blocks[7]; blocks[7] = blocks[11]; blocks[11] = t;
		t = blocks[9]; blocks[9] = blocks[13]; blocks[13] = t;
		blocks += 16;
	}
}
#endif

