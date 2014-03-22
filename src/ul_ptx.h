/*
 * ul_ptx.h
 *
 *  Created on: Sep 30, 2011
 *      Author: qwer1
 *  @brief  Protothreads extensions
 */

#ifndef UL_PTX_H_
#define UL_PTX_H_

#include "ul_other.h"
#include "pt.h"
#include "ul_timing.h"

//Execute statement cmd and wait until condition becames true
#define PTX_EXEC_WAIT_UNTIL(cmd, cond) { cmd; }; PT_WAIT_UNTIL(pt, cond)

//delay, ms using millisecond timer
#define PTX_DELAY(tmr, time) PTX_EXEC_WAIT_UNTIL((tmr) = (time), (tmr) == 0);

//delay using thread millisecond timer
#define PTX_DELAY_MS(ms) PTX_DELAY(ptx->tmr, ms)

//delay seconds
#define PTX_DELAY_S(s) PTX_DELAY(ptx->tmr, s * 1000)

//decrement timer if flag is set
#define PTX_TIMER_DEC(flag, tmr) if ((flag) && (tmr)) (tmr)--

//decrement millisecond timer
#define PTX_TIMER_MS_DEC(tmr) PTX_TIMER_DEC(t.ms1, tmr)

//delay for N thread ticks
#define PTX_DELAY_TICKS(N) PTX_EXEC_WAIT_UNTIL(ptx->ticks=(N), (ptx->ticks) == 0)

//thread structure
struct ptx {
	U8 num;       //thread number
	struct pt pt; //thread context
	char (*pf)(struct pt *pt, struct ptx *ptx); //pointer to thread function
	U16  tmr;     //thread timer
	U16  ticks;   //ticks counter
	U8   flags;   //thread flags
	void *data;   //pointer to thread data
};

//thread flags
enum {PTX_STOP=0,PTX_RUN=1,PTX_SYNC_1MS=2,PTX_SYNC_10MS=4,PTX_SYNC_100MS=8,PTX_SYNC_1S=16};
#define PTX_SYNC (PTX_SYNC_1MS | PTX_SYNC_10MS | PTX_SYNC_100MS | PTX_SYNC_1S)
#define PTX_RUN_1MS   (PTX_RUN | PTX_SYNC_1MS)
#define PTX_RUN_10MS  (PTX_RUN | PTX_SYNC_10MS)
#define PTX_RUN_100MS (PTX_RUN | PTX_SYNC_100MS)
#define PTX_RUN_1S    (PTX_RUN | PTX_SYNC_1S)

//threads queue
extern struct ptx ptx_thread[PTX_THREADS];
extern U8 ptx_num;

//add new thread to queue
struct ptx *ptx_add(char (*pf)(struct pt *pt, struct ptx *ptx), void *data, U8 flags);

//run started threads
//void ptx_dispatch( void );
inline void ptx_dispatch( void ) {
	if (ptx_num > 0) {
		struct ptx *ptx = &ptx_thread[0];
		for (U8 cnt=0; cnt<ptx_num; cnt++, ptx++) {
			U8 flags = ptx->flags;
			U8 run = 0;
			if (flags & PTX_RUN) {

				//decrement thread timer
				PTX_TIMER_MS_DEC(ptx->tmr);

				if (flags & PTX_SYNC) {
					if (((flags & PTX_SYNC_1MS) && (t.ms1)) ||
						((flags & PTX_SYNC_10MS) && (t.ms10)) ||
						((flags & PTX_SYNC_100MS) && (t.ms100)) ||
						((flags & PTX_SYNC_1S) && (t.s1))
					) run = 1;
				} else run = 1;
			}
			//call thread function
			if (run) {
				if (ptx->ticks) ptx->ticks--; //update tick counter
				(ptx_thread[cnt].pf)(&ptx->pt, ptx); //call thread function
			}
		}
	}
}

//stop thread
void ptx_stop(struct ptx *ptx);

//start/resume thread
void ptx_start(struct ptx *ptx);

//restart thread
void ptx_restart(struct ptx *ptx);

#endif /* UL_PTX_H_ */

