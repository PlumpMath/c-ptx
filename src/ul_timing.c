/*
 * ul_timing.c
 *
 *  Created on: Sep 29, 2011
 *      Author: qwer1
 */

#include "ul_timing.h"

volatile struct t_struct t;

//inline void _delay_loops(U32 loops) __attribute__ ((always_inline, naked));
#if defined( __ARM_ARCH_7M__ ) || defined( __ARM_ARCH_6M__ )
	void delay_loops(U32 loops) {
		asm(".syntax unified");
		asm volatile (
			"1: SUBS %[loops], %[loops], #1 \n"
			"   BNE 1b \n"
			: [loops] "+r"(loops)
		);
		asm(".syntax divided");
	}
#endif

void t_setup(void) {
}

U32 t_ms(void) {
	cli();
	U32 ms = t.cnt.ms;
	sei();
	return ms;
}
