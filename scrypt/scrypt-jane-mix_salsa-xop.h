/* x86 */
#if defined(X86ASM_XOP) && (!defined(SCRYPT_CHOOSE_COMPILETIME) || !defined(SCRYPT_SALSA_INCLUDED)) && !defined(CPU_X86_FORCE_INTRINSICS)

#define SCRYPT_SALSA_XOP

asm_naked_fn_proto(void, scrypt_ChunkMix_xop)(uint32_t *Bout/*[chunkBytes]*/, uint32_t *Bin/*[chunkBytes]*/, uint32_t *Bxor/*[chunkBytes]*/, uint32_t r)
asm_naked_fn(scrypt_ChunkMix_xop)
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
	aj(jz scrypt_ChunkMix_xop_no_xor1)
	a3(vpxor (%ecx, %eax), %xmm0,%xmm0)
	a3(vpxor 16(%ecx, %eax), %xmm1,%xmm1)
	a3(vpxor 32(%ecx, %eax), %xmm2,%xmm2)
	a3(vpxor 48(%ecx, %eax), %xmm3,%xmm3)
	a1(scrypt_ChunkMix_xop_no_xor1:)
	a2(xor %ecx,%ecx)
	a2(xor %ebx,%ebx)
	a1(scrypt_ChunkMix_xop_loop:)
		a2(and %eax, %eax)
		a3(vpxor (%esi,%ecx), %xmm0,%xmm0)
		a3(vpxor 16(%esi,%ecx), %xmm1,%xmm1)
		a3(vpxor 32(%esi,%ecx), %xmm2,%xmm2)
		a3(vpxor 48(%esi,%ecx), %xmm3,%xmm3)
		aj(jz scrypt_ChunkMix_xop_no_xor2)
		a3(vpxor (%eax, %ecx), %xmm0,%xmm0)
		a3(vpxor 16(%eax, %ecx), %xmm1,%xmm1)
		a3(vpxor 32(%eax, %ecx), %xmm2,%xmm2)
		a3(vpxor 48(%eax, %ecx), %xmm3,%xmm3)
		a1(scrypt_ChunkMix_xop_no_xor2:)
		a2(vmovdqa %xmm0, (%esp))
		a2(vmovdqa %xmm1, 16(%esp))
		a2(vmovdqa %xmm2,%xmm6)
		a2(vmovdqa %xmm3,%xmm7)
		a2(mov $8, %eax)
		a1(scrypt_salsa_xop_loop: )
			a3(vpaddd %xmm0, %xmm4, %xmm1)
			a3(vprotd $7, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm3, %xmm3)
			a3(vpaddd %xmm3, %xmm4, %xmm0)
			a3(vprotd $9, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm2, %xmm2)
			a3(vpaddd %xmm2, %xmm4, %xmm3)
			a3(vprotd $13, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm1, %xmm1)
			a3(vpaddd %xmm1, %xmm4, %xmm2)
			a3(pshufd $0x93, %xmm3, %xmm3)
			a3(vprotd $18, %xmm4, %xmm4)
			a3(pshufd $0x4e, %xmm2, %xmm2)
			a3(vpxor %xmm4, %xmm0, %xmm0)
			a3(pshufd $0x39, %xmm1, %xmm1)
			a3(vpaddd %xmm0, %xmm4, %xmm3)
			a3(vprotd $7, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm1, %xmm1)
			a3(vpaddd %xmm1, %xmm4, %xmm0)
			a3(vprotd $9, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm2, %xmm2)
			a3(vpaddd %xmm2, %xmm4, %xmm1)
			a3(vprotd $13, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm3, %xmm3)
			a3(pshufd $0x93, %xmm1, %xmm1)
			a3(vpaddd %xmm3, %xmm4, %xmm2)
			a3(pshufd $0x4e, %xmm2, %xmm2)
			a3(vprotd $18, %xmm4, %xmm4)
			a3(pshufd $0x39, %xmm3, %xmm3)
			a3(vpxor %xmm4, %xmm0, %xmm0)
			a2(sub $2, %eax)
			aj(ja scrypt_salsa_xop_loop)
		a3(vpaddd (%esp), %xmm0,%xmm0)
		a3(vpaddd 16(%esp), %xmm1,%xmm1)
		a3(vpaddd %xmm6, %xmm2,%xmm2)
		a3(vpaddd %xmm7, %xmm3,%xmm3)
		a2(lea (%ebx, %ecx), %eax)
		a2(xor %edx, %ebx)
		a2(and $-128, %eax)
		a2(add $64, %ecx)
		a2(shr $1, %eax)
		a2(add %edi, %eax)
		a2(cmp %edx, %ecx)
		a2(vmovdqa %xmm0, (%eax))
		a2(vmovdqa %xmm1, 16(%eax))
		a2(vmovdqa %xmm2, 32(%eax))
		a2(vmovdqa %xmm3, 48(%eax))
		a2(mov 28(%ebp), %eax)
		aj(jne scrypt_ChunkMix_xop_loop)
	a2(mov %ebp, %esp)
	a1(pop %ebp)
	a1(pop %esi)
	a1(pop %edi)
	a1(pop %ebx)
	aret($16)
asm_naked_fn_end(scrypt_ChunkMix_xop)

#endif



/* x64 */
#if defined(X86_64ASM_XOP) && (!defined(SCRYPT_CHOOSE_COMPILETIME) || !defined(SCRYPT_SALSA_INCLUDED)) && !defined(CPU_X86_FORCE_INTRINSICS)

#define SCRYPT_SALSA_XOP

asm_naked_fn_proto(void, scrypt_ChunkMix_xop)(uint32_t *Bout/*[chunkBytes]*/, uint32_t *Bin/*[chunkBytes]*/, uint32_t *Bxor/*[chunkBytes]*/, uint32_t r)
asm_naked_fn(scrypt_ChunkMix_xop)
	a2(lea (,%ecx,2), %rcx) /* zero extend uint32_t by using %ecx, win64 can leave garbage in the top half */
	a2(shl $6, %rcx)
	a2(lea -64(%rcx), %r9)
	a2(lea (%rsi,%r9), %rax)
	a2(lea (%rdx,%r9), %r9)
	a2(and %rdx, %rdx)
	a2(vmovdqa (%rax), %xmm0)
	a2(vmovdqa 16(%rax), %xmm1)
	a2(vmovdqa 32(%rax), %xmm2)
	a2(vmovdqa 48(%rax), %xmm3)
	aj(jz scrypt_ChunkMix_xop_no_xor1)
	a3(vpxor (%r9), %xmm0, %xmm0)
	a3(vpxor 16(%r9), %xmm1, %xmm1)
	a3(vpxor 32(%r9), %xmm2, %xmm2)
	a3(vpxor 48(%r9), %xmm3, %xmm3)
	a1(scrypt_ChunkMix_xop_no_xor1:)
	a2(xor %r9,%r9)
	a2(xor %r8,%r8)
	a1(scrypt_ChunkMix_xop_loop:)
		a2(and %rdx, %rdx)
		a3(vpxor (%rsi,%r9), %xmm0, %xmm0)
		a3(vpxor 16(%rsi,%r9), %xmm1,%xmm1)
		a3(vpxor 32(%rsi,%r9), %xmm2,%xmm2)
		a3(vpxor 48(%rsi,%r9), %xmm3,%xmm3)
		aj(jz scrypt_ChunkMix_xop_no_xor2)
		a3(vpxor (%rdx,%r9),%xmm0,%xmm0)
		a3(vpxor 16(%rdx,%r9),%xmm1,%xmm1)
		a3(vpxor 32(%rdx,%r9),%xmm2,%xmm2)
		a3(vpxor 48(%rdx,%r9),%xmm3,%xmm3)
		a1(scrypt_ChunkMix_xop_no_xor2:)
		a2(vmovdqa %xmm0,%xmm8)
		a2(vmovdqa %xmm1,%xmm9)
		a2(vmovdqa %xmm2,%xmm10)
		a2(vmovdqa %xmm3,%xmm11)
		a2(mov $8, %rax)
		a1(scrypt_salsa_xop_loop: )
			a3(vpaddd %xmm0, %xmm1, %xmm4)
			a3(vprotd $7, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm3, %xmm3)
			a3(vpaddd %xmm3, %xmm0, %xmm4)
			a3(vprotd $9, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm2, %xmm2)
			a3(vpaddd %xmm2, %xmm3, %xmm4)
			a3(vprotd $13, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm1, %xmm1)
			a3(vpaddd %xmm1, %xmm2, %xmm4)
			a3(pshufd $0x93, %xmm3, %xmm3)
			a3(vprotd $18, %xmm4, %xmm4)
			a3(pshufd $0x4e, %xmm2, %xmm2)
			a3(vpxor %xmm4, %xmm0, %xmm0)
			a3(pshufd $0x39, %xmm1, %xmm1)
			a3(vpaddd %xmm0, %xmm3, %xmm4)
			a3(vprotd $7, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm1, %xmm1)
			a3(vpaddd %xmm1, %xmm0, %xmm4)
			a3(vprotd $9, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm2, %xmm2)
			a3(vpaddd %xmm2, %xmm1, %xmm4)
			a3(vprotd $13, %xmm4, %xmm4)
			a3(vpxor %xmm4, %xmm3, %xmm3)
			a3(pshufd $0x93, %xmm1, %xmm1)
			a3(vpaddd %xmm3, %xmm2, %xmm4)
			a3(pshufd $0x4e, %xmm2, %xmm2)
			a3(vprotd $18, %xmm4, %xmm4)
			a3(pshufd $0x39, %xmm3, %xmm3)
			a3(vpxor %xmm4, %xmm0, %xmm0)
			a2(sub $2, %rax)
			aj(ja scrypt_salsa_xop_loop)
		a3(vpaddd %xmm8,%xmm0,%xmm0)
		a3(vpaddd %xmm9,%xmm1,%xmm1)
		a3(vpaddd %xmm10,%xmm2,%xmm2)
		a3(vpaddd %xmm11,%xmm3,%xmm3)
		a2(lea (%r8,%r9), %rax)
		a2(xor %rcx, %r8)
		a2(and $-128, %rax)
		a2(add $64, %r9)
		a2(shr $1, %rax)
		a2(add %rdi, %rax)
		a2(cmp %rcx, %r9)
		a2(vmovdqa %xmm0, (%rax))
		a2(vmovdqa %xmm1, 16(%rax))
		a2(vmovdqa %xmm2, 32(%rax))
		a2(vmovdqa %xmm3, 48(%rax))
		aj(jne scrypt_ChunkMix_xop_loop)
	a1(ret)
asm_naked_fn_end(scrypt_ChunkMix_xop)

#endif


/* intrinsic */
#if defined(X86_INTRINSIC_XOP) && (!defined(SCRYPT_CHOOSE_COMPILETIME) || !defined(SCRYPT_SALSA_INCLUDED))

#define SCRYPT_SALSA_XOP

static void asm_calling_convention NOINLINE
scrypt_ChunkMix_xop(uint32_t *Bout/*[chunkBytes]*/, uint32_t *Bin/*[chunkBytes]*/, uint32_t *Bxor/*[chunkBytes]*/, uint32_t r) {
	uint32_t i, blocksPerChunk = r * 2, half = 0;
	%xmmi *%xmmp,x0,x1,x2,x3,x4,x5,t0,t1,t2,t3;
	size_t rounds;

	/* 1: X = B_{2r - 1} */
	%xmmp = (%xmmi *)scrypt_block(Bin, blocksPerChunk - 1);
	x0 = %xmmp[0];
	x1 = %xmmp[1];
	x2 = %xmmp[2];
	x3 = %xmmp[3];

	if (Bxor) {
		%xmmp = (%xmmi *)scrypt_block(Bxor, blocksPerChunk - 1);
		x0 = _mm_xor_si128(x0, %xmmp[0]);
		x1 = _mm_xor_si128(x1, %xmmp[1]);
		x2 = _mm_xor_si128(x2, %xmmp[2]);
		x3 = _mm_xor_si128(x3, %xmmp[3]);
	}

	/* 2: for i = 0 to 2r - 1 do */
	for (i = 0; i < blocksPerChunk; i++, half ^= r) {
		/* 3: X = H(X ^ B_i) */
		%xmmp = (%xmmi *)scrypt_block(Bin, i);
		x0 = _mm_xor_si128(x0, %xmmp[0]);
		x1 = _mm_xor_si128(x1, %xmmp[1]);
		x2 = _mm_xor_si128(x2, %xmmp[2]);
		x3 = _mm_xor_si128(x3, %xmmp[3]);

		if (Bxor) {
			%xmmp = (%xmmi *)scrypt_block(Bxor, i);
			x0 = _mm_xor_si128(x0, %xmmp[0]);
			x1 = _mm_xor_si128(x1, %xmmp[1]);
			x2 = _mm_xor_si128(x2, %xmmp[2]);
			x3 = _mm_xor_si128(x3, %xmmp[3]);
		}

		t0 = x0;
		t1 = x1;
		t2 = x2;
		t3 = x3;

		for (rounds = 8; rounds; rounds -= 2) {
			x4 = _mm_add_epi32(x1, x0);
			x4 = _mm_roti_epi32(x4, 7);
			x3 = _mm_xor_si128(x3, x4);
			x4 = _mm_add_epi32(x0, x3);
			x4 = _mm_roti_epi32(x4, 9);
			x2 = _mm_xor_si128(x2, x4);
			x4 = _mm_add_epi32(x3, x2);
			x4 = _mm_roti_epi32(x4, 13);
			x1 = _mm_xor_si128(x1, x4);
			x4 = _mm_add_epi32(x2, x1);
			x4 = _mm_roti_epi32(x4, 18);
			x0 = _mm_xor_si128(x0, x4);
			x3 = _mm_shuffle_epi32(x3, 0x93);
			x2 = _mm_shuffle_epi32(x2, 0x4e);
			x1 = _mm_shuffle_epi32(x1, 0x39);
			x4 = _mm_add_epi32(x3, x0);
			x4 = _mm_roti_epi32(x4, 7);
			x1 = _mm_xor_si128(x1, x4);
			x4 = _mm_add_epi32(x0, x1);
			x4 = _mm_roti_epi32(x4, 9);
			x2 = _mm_xor_si128(x2, x4);
			x4 = _mm_add_epi32(x1, x2);
			x4 = _mm_roti_epi32(x4, 13);
			x3 = _mm_xor_si128(x3, x4);
			x4 = _mm_add_epi32(x2, x3);
			x4 = _mm_roti_epi32(x4, 18);
			x0 = _mm_xor_si128(x0, x4);
			x1 = _mm_shuffle_epi32(x1, 0x93);
			x2 = _mm_shuffle_epi32(x2, 0x4e);
			x3 = _mm_shuffle_epi32(x3, 0x39);
		}

		x0 = _mm_add_epi32(x0, t0);
		x1 = _mm_add_epi32(x1, t1);
		x2 = _mm_add_epi32(x2, t2);
		x3 = _mm_add_epi32(x3, t3);

		/* 4: Y_i = X */
		/* 6: B'[0..r-1] = Y_even */
		/* 6: B'[r..2r-1] = Y_odd */
		%xmmp = (%xmmi *)scrypt_block(Bout, (i / 2) + half);
		%xmmp[0] = x0;
		%xmmp[1] = x1;
		%xmmp[2] = x2;
		%xmmp[3] = x3;
	}
}

#endif

#if defined(SCRYPT_SALSA_XOP)
	/* uses salsa_core_tangle_sse2 */

	#undef SCRYPT_MIX
	#define SCRYPT_MIX "Salsa/8-XOP"
	#undef SCRYPT_SALSA_INCLUDED
	#define SCRYPT_SALSA_INCLUDED
#endif
