/*
 * ul_timing.h
 *
 *  Created on: Sep 29, 2011
 *      Author: qwer1
 */

#ifndef UL_T_H_
#define UL_T_H_

#include "ul_other.h"

/** delay using simple loop */
#if defined( __ARM_ARCH_7M__ ) || defined( __ARM_ARCH_6M__ )
	void delay_loops(U32 loops);
	#ifndef DELAY_LOOP_CYCLES
		#define DELAY_LOOP_CYCLES 3.0
	#endif
	#define DELAY_US_LOOPS(US) ((U32)((double)US * F_CPU / DELAY_LOOP_CYCLES / 1000000.0))
	#define DELAY_MS_LOOPS(MS) ((U32)((double)MS * F_CPU / DELAY_LOOP_CYCLES / 1000.0))
	#define DELAY_S_LOOPS(S)   ((U32)((double)S  * F_CPU / DELAY_LOOP_CYCLES))
	#define delay_us( US ) delay_loops( DELAY_US_LOOPS(US) )
	#define delay_ms( MS ) delay_loops( DELAY_MS_LOOPS(MS) )
	#define delay_s( S )   delay_loops( DELAY_S_LOOPS(S) )
#endif

#ifdef __AVR
	#include <util/delay.h>
	#define delay_us( US ) { for (U8 us=(US / 30UL); us; us--) _delay_us(30); _delay_us( US % 30UL ); }
	#define delay_ms( MS ) { for (U8 ms=(MS / 10UL); ms; ms--) _delay_ms(10); _delay_ms( MS % 10UL ); }
	#define delay_s( S )   { for (U8 s=S; s; s--) for (U8 ms=100; ms; ms--) delay_ms(10); }
#endif


struct t_struct {
	U8 tick;
	U8 ms1,ms10,ms100,ms500,s1;  //1,10,100,500ms,1000ms flags
	struct {
		U8  ms10,ms100,ms500,s1; //10,100,1000ms counters
		U32 ms;                  //ms counter
	} cnt;
};

extern volatile struct t_struct t;

inline void t_sync() {
	t.ms1 = t.ms10 = t.ms100 = t.ms500 = t.s1 = 0;
}

inline void t_update() {
	t.ms1 = 1;
	t.ms10 = t.ms100 = t.ms500 = t.s1 = 0;

	if ((++t.cnt.ms10) >= 10) {
		t.cnt.ms10 = 0;
		t.ms10     = 1;

		if ((++t.cnt.ms100) >= 10) {
			t.cnt.ms100 = 0;
			t.ms100     = 1;

			if ((++t.cnt.ms500) >= 5) {
				t.cnt.ms500 = 0;
				t.ms500     = 1;
			}

			if ((++t.cnt.s1) >= 10) {
				t.cnt.s1 = 0;
				t.s1     = 1;
			}
		}
	}
	t.cnt.ms++;
}

// setup timing module
void t_setup(void);

//get milliseconds counter
U32 t_ms(void);

#endif /* UL_T_H_ */
