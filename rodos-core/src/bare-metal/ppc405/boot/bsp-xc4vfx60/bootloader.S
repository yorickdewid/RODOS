/**
 * @author Tobias Mikschl
 */

#define SRC_ADDR 0x07000000
#define SRC_SIZE 200000
#define DST_START 0x0000

	.section .bootload0,"ax"
	.global _bootload0

_bootload0:	
	/*Reset Stackpointer*/
	/*addis   1,0,__stack@h      
        ori     1,1,__stack@l
	addis   14,0,0x0000
	stw     14,0*/
	lis     0,0x00
	lis     3,0x00  
	lis     4,SRC_ADDR@h  
	ori     4,4,SRC_ADDR@l
	lis     5,SRC_SIZE@h
	ori     5,5,SRC_SIZE@l
	bl      _memcpy
	/*Jump to _start*/
	lis     0, DST_START@h
	ori     0, 0, DST_START@l
	mtlr    0
	blr


/* Optimized memcpy implementation for PowerPC. */

_memcpy:
/* void * [r3] memcpy(void *dest [r3], const void *src [r4], size_t n [r5]) */

/* Registers used:
   r0:  temporary
   r3:	saved `dest'
   r4:	pointer to previous word in src
   r5:	
   r6:	pointer to previous word in dest
   r7:  temporary
   r8:  temporary (used to move words)
*/
	srwi.	7,5,3       /* r0 = r5 >> 3 */
	addi	6,3,-4
	addi	4,4,-4
	beq     2f          /* if less than 8 bytes to do */
	andi.	0,6,3       /* get dest word aligned */
	mtctr	7
	bne     5f
1:	lwz     7,4(4)
	lwzu	8,8(4)
	stw     7,4(6)
	stwu	8,8(6)
	bdnz	1b
	andi.	5,5,7
2:	cmplwi	0,5,4
	blt     3f
	lwzu	0,4(4)
	addi	5,5,-4
	stwu	0,4(6)
3:	cmpwi	0,5,0
	beqlr
	mtctr	5
	addi	4,4,3
	addi	6,6,3
4:	lbzu	0,1(4)
	stbu	0,1(6)
	bdnz	4b
	blr
5:	subfic	0,0,4
	mtctr	0
6:	lbz     7,4(4)
	addi	4,4,1
	stb     7,4(6)
	addi	6,6,1
	bdnz	6b
	subf	5,0,5
	srwi.	7,5,3
	beq     2b
	mtctr	7
	b       1b

	.section .bootload,"ax"
	.global _bootload

_bootload:	
	b       _bootload0

