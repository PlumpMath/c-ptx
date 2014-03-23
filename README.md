ptx
===

Simple scheduler for Protothreads

- threads and events synchronization to 1/10/100/1000 ms intervals;
- 1 16-bit timer per thread;
- 1 user data pointer per thread;


```c
#include "ul_other.h"
#include "ul_timing.h"

//maximum number of threads
#define  PTX_THREADS 2
#include "ul_ptx.h"

//threads function prototypes
PT_THREAD( led1_pt(struct pt *pt, struct ptx *ptx) );
PT_THREAD( led2_pt(struct pt *pt, struct ptx *ptx) );

int main() {
	// setup timing module
	t_setup();
	
	//start threads
	ptx_add(led1_pt, NULL, PTX_RUN);
	ptx_add(led2_pt, NULL, PTX_RUN);
	
	//main loop
	while (1) {
		//synchronize timing flags
		t_sync();

		// 1 ms flag set ? Update other flags
		if (t.tick) {
			t.tick = 0;
			t_update();
		}
		
		//run threads
		ptx_dispatch();
	}
	
	return 0;
}

PT_THREAD(led1_pt(struct pt *pt, struct ptx *ptx)) {
	PT_BEGIN(pt);

	while (1) {
		led1_set(1);
		//delay 500ms
		PTX_DELAY_MS(500);

		led1_set(0);
		//delay 800ms
		PTX_DELAY_MS(800);
	}

	PT_END(pt);
	return 0;
}

PT_THREAD(led2_pt(struct pt *pt, struct ptx *ptx)) {
	PT_BEGIN(pt);

	while (1) {
		led2_set(1);
		//delay 300ms
		PTX_DELAY_MS(300);

		led2_set(0);
		//delay 700ms
		PTX_DELAY_MS(700);
	}

	PT_END(pt);
	return 0;
}

```

