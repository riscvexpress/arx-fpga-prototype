#include "platform_info.h"
#include "ervp_printf.h"
#include "ervp_assert.h"
#include "ervp_interrupt.h"
#include "ervp_thread.h"
#include "ervp_variable_allocation.h"
#include "ervp_core_id.h"

// Software interrupt example for a manager core with worker cores.
//
// The manager starts one thread on every worker. Each worker registers a
// software interrupt ISR and waits with wfi. The manager then interrupts the
// workers one by one, and each worker answers with a software interrupt back
// to the manager.

static volatile int received[NUM_CORE_PHYSICAL] NOTCACHED_DATA;
static volatile int ready[NUM_CORE_PHYSICAL] NOTCACHED_DATA;
static volatile int manager_id NOTCACHED_DATA;

static void isr_software(void)
{
	received[EXCLUSIVE_ID]++;
}

static void wait_for_software_interrupt(int expected)
{
	disable_interrupt_trap();
	while(received[EXCLUSIVE_ID] < expected)
	{
		wait_for_interrupt();
		enable_interrupt_trap();
		disable_interrupt_trap();
	}
	enable_interrupt_trap();
}

static int worker(void *arg)
{
	const int reply_to = *(volatile int *)arg;
	register_isr_software(isr_software);
	allow_interrupt_software();
	enable_interrupt_trap();
	ready[EXCLUSIVE_ID] = 1;
	wait_for_software_interrupt(1);
	send_software_interrupt(reply_to);
	return EXCLUSIVE_ID;
}

int main()
{
	int i, core_id, ret;
	ervp_thread_t thr[NUM_CORE_WORKER];

	if(EXCLUSIVE_ID==MANAGER_ID)
	{
		manager_id = EXCLUSIVE_ID;
		register_isr_software(isr_software);
		allow_interrupt_software();
		enable_interrupt_trap();

		for(i=0; i<NUM_CORE_WORKER; i++)
			while(thread_create(&thr[i], worker, (void *)&manager_id)!=THREAD_SUCCESS);

		for(i=0; i<NUM_CORE_WORKER; i++)
		{
			core_id = NUM_CORE_USER + thr[i].worker_core_id;
			while(ready[core_id]==0);
			send_software_interrupt(core_id);
			wait_for_software_interrupt(i+1);
			printf("core %d answered\n", core_id);
		}

		for(i=0; i<NUM_CORE_WORKER; i++)
		{
			thread_join(thr[i], &ret);
			core_id = NUM_CORE_USER + thr[i].worker_core_id;
			assert_must(ret==core_id);
			assert_must(received[core_id]==1);
		}
		assert_must(received[manager_id]==NUM_CORE_WORKER);
		printf("PASS\n");
	}
	return 0;
}
