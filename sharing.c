#include "spcd.h"

unsigned *share = NULL;

static inline int get_num_sharers(struct pt_mem_info *elem)
{
	if (elem->sharer[0] == -1 && elem->sharer[1] == -1)
		return 0;
	if (elem->sharer[0] != -1 && elem->sharer[1] != -1)
		return 2;
	return 1;
}


static inline void maybe_inc(int first, int second, unsigned old_tsc, unsigned long new_tsc)
{
	// if (new_tsc-old_tsc <= TSC_DELTA) {
	if (first > second)
		share[first * max_threads + second] ++;
	else
		share[second * max_threads + first] ++;
	// }
}

void pt_check_comm(int tid, unsigned long address)
{
	struct pt_mem_info *elem = pt_get_mem_init(address);
	int sh = get_num_sharers(elem);
	unsigned new_tsc = get_cycles();

	switch (sh) {
		case 0: // no one accessed page before, store accessing thread in pos 0
			elem->sharer[0] = tid;
			break;

		case 1: // one previous access => needs to be in pos 0
			/*if (elem->sharer[0] == -1) {
				if (elem->sharer[1] != tid) {
					elem->sharer[0] = tid;
					maybe_inc(tid, elem->sharer[1], elem->tsc, new_tsc);
				}
			} else */
			if (elem->sharer[0] != tid) {
				maybe_inc(tid, elem->sharer[0], elem->tsc, new_tsc);
				elem->sharer[1] = elem->sharer[0];
				elem->sharer[0] = tid;
			}
			break;

		case 2: // two previous accesses
			if (elem->sharer[0] != tid && elem->sharer[1] != tid) {
				maybe_inc(tid, elem->sharer[0], elem->tsc, new_tsc);
				maybe_inc(tid, elem->sharer[1], elem->tsc, new_tsc);
				elem->sharer[1] = elem->sharer[0];
				elem->sharer[0] = tid;
			} else if (elem->sharer[0] == tid) {
				maybe_inc(tid, elem->sharer[1], elem->tsc, new_tsc);
			} else if (elem->sharer[1] == tid) {
				maybe_inc(tid, elem->sharer[0], elem->tsc, new_tsc);
				elem->sharer[1] = elem->sharer[0];
				elem->sharer[0] = tid;
			}

			break;
	}

	elem->tsc = new_tsc;
}


// static int avg(unsigned share[PT_MAXTHREADS][PT_MAXTHREADS], int nt)
// {
// 	int i, j;
// 	int sum = 0;
// 	for (i = nt-1; i >= 0; i--) {
// 		for (j = 0; j < nt; j++) {
// 			sum += share[i][j] + share[j][i];
// 		}
// 	}
// 	return sum / nt / nt;
// }


// static int var(unsigned share[PT_MAXTHREADS][PT_MAXTHREADS], int nt, int avg)
// {
// 	int i, j;
// 	int sum = 0;

// 	for (i = nt-1; i >= 0; i--) {
// 		for (j = 0; j < nt; j++) {
// 			int diff = avg - (share[i][j] + share[j][i]);
// 			sum += diff*diff;
// 		}
// 	}
// 	return sum / nt / nt;
// }

static inline unsigned get_share(int i, int j)
{
	if (i>j)
		return share[i*max_threads + j];
	else
		return share[j*max_threads + i];
}

void pt_print_share(void)
{
	int i, j;
	int nt = spcd_get_num_threads();
	int sum = 0, sum_sqr = 0;
	int av, va;

	for (i = nt-1; i >= 0; i--) {
		for (j = 0; j < nt; j++) {
			int s = get_share(i,j);
			sum += s;
			sum_sqr += s*s;
			printk ("%u", s);
			if (j != nt-1)
				printk (",");
		}
		printk("\n");
	}
	
	av = sum / nt / nt;
	va = (sum_sqr - ((sum*sum)/nt))/(nt-1);

	printk ("avg: %d, var: %d, hf: %d\n", av, va, av ? va/av : 0);
}




void pt_share_clear(void)
{
	if (!share)
		share = (unsigned*) kmalloc (sizeof(unsigned) * max_threads * max_threads, GFP_KERNEL);
	memset(share, 0, sizeof(unsigned) * max_threads * max_threads);

}
