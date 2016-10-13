/*
 * ul_timing.h
 *
 *  Created on: Sep 29, 2011
 *      Author: qwer1
 */

#ifndef UL_T_H_
#define UL_T_H_

#include "ul_other.h"

struct t_struct {
	u8 tick,ms1,ms10,ms100,ms500,s1;  //1,10,100,500ms,1000ms flags
	struct {
		u8  tick,ms10,ms100,ms500,s1; //10,100,1000ms counters
		u32 ms;                  //ms counter
	} cnt;
};

extern volatile struct t_struct t;

inline void t_sync() { if (t.ms1) t.ms1 = t.ms10 = t.ms100 = t.ms500 = t.s1 = 0; }

inline void t_update() {
	t.ms1 = 1;
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
u32 t_ms(void);

#ifdef __AVR
	#include <util/delay.h>
	#define delay_us( US ) { for (u8 us=((US) / 48.0); us; us--) _delay_us(48); _delay_us( (US) % 48 ); }
	#define delay_ms( MS ) { for (u8 ms=((MS) / 16.0); ms; ms--) _delay_ms(16); _delay_ms( (MS) % 16 ); }
	#define delay_s ( S )  { for (u8 s=(S); s; s--) for (u8 ms=100; ms; ms--) delay_ms(10); }
#endif

#ifdef __STM32
	#define HSE_DIV (F_HSE / F_BASE)
	#define PLL_MUL (F_CPU / F_BASE)

	// delay with loop
	#if defined( __ARM_ARCH_6M__ ) || defined( __ARM_ARCH_7M__ ) || defined( __ARM_ARCH_7EM__ )
		void delay_loops(u32 loops);
		#ifndef DELAY_LOOP_CYCLES
			#define DELAY_LOOP_CYCLES 3.0
		#endif
		#define DELAY_US_LOOPS(US) ((u32)((double)(US) * F_CPU / DELAY_LOOP_CYCLES / 1000000.0))
		#define DELAY_MS_LOOPS(MS) ((u32)((double)(MS) * F_CPU / DELAY_LOOP_CYCLES / 1000.0))
		#define DELAY_S_LOOPS (S)  ((u32)((double)(S)  * F_CPU / DELAY_LOOP_CYCLES))
		#define delay_us( US ) delay_loops( DELAY_US_LOOPS(US) )
		#define delay_ms( MS ) delay_loops( DELAY_MS_LOOPS(MS) )
		#define delay_s ( S )  delay_loops( DELAY_S_LOOPS(S) )
	#endif
#endif

#endif /* UL_T_H_ */
