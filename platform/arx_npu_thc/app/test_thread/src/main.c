#include "platform_info.h"
#include "ervp_printf.h"
#include "ervp_assert.h"
#include "ervp_multicore_synch.h"
#include "ervp_thread.h"
#include "ervp_variable_allocation.h"
#include "ervp_core_id.h"

int worker(void *arg)
{
	int num = *(int *)arg;
	for (int i = 0; i < 3; i++)
		printf("\nHi %d %d", EXCLUSIVE_ID, num);
	return 0;
}

volatile int num1 NOTCACHED_DATA;

int main()
{
	if (MANAGER_ID == 0)
	{
		printf("\nHello");
		ervp_thread_t thr1;
		num1 = 9;
		assert_must(thread_create(&thr1, worker, &num1) == THREAD_SUCCESS);

		// fallback only when a NOTCACHED_DATA global is not possible: flush a local before passing it
		ervp_thread_t thr2;
		int num2 = 7;
		flush_cache();
		assert_must(thread_create(&thr2, worker, &num2) == THREAD_SUCCESS);

		thread_join(thr1, NULL);
		thread_join(thr2, NULL);
	}

	return 0;
}
