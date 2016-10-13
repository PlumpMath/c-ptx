/*
 * ul_ptx.c
 *
 *  Created on: Oct 25, 2011
 *      Author: qwer1
 */

#include <stddef.h>
#include "ul_ptx.h"
#include "ul_timing.h"

struct ptx ptx_thread[PTX_THREADS];
u8 ptx_num;

struct ptx *ptx_add( char (*pf)(struct pt *pt, struct ptx *ptx), void *data, u8 flags ) {
	if (ptx_num < (PTX_THREADS)) {
		struct ptx *ptx = &ptx_thread[ptx_num];
		ptx->num   = ptx_num;
		ptx->pf    = pf;
		ptx->data  = data;
		PT_INIT( &ptx->pt );
		ptx->tmr   = 0;
		ptx->flags = flags;
		ptx_num++;
		return ptx;
	}
	return NULL;
}


void ptx_stop(struct ptx *ptx){
	ptx->flags &= ~PTX_RUN;
	PT_INIT(&ptx->pt);
}

void ptx_start(struct ptx *ptx) {
	ptx->flags |= PTX_RUN;
}

void ptx_restart(struct ptx *ptx) {
	PT_INIT(&(ptx->pt));
	ptx->flags |= PTX_RUN;
}
