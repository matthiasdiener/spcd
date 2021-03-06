#include "spcd.h"

static struct pt_mem_info *mem;
unsigned long pt_addr_conflict;


static inline
unsigned long addr_to_page(unsigned long address)
{
	return (address >> spcd_shift);
}


static inline
struct pt_mem_info* pt_get_mem(unsigned long address)
{
	return &mem[hash_32(addr_to_page(address), spcd_mem_hash_bits)];
}


/* get mem elem, initialize if necessary */
struct pt_mem_info* pt_get_mem_init(unsigned long address)
{
	struct pt_mem_info *elem = pt_get_mem(address);
	unsigned long page = addr_to_page(address);

	if (elem->pg_addr != page) { /* new elem */
		if (elem->pg_addr != 0) { /* delete old elem */
			pt_addr_conflict++;
		}

		elem->sharer[0] = -1;
		elem->sharer[1] = -1;
		elem->pg_addr = page;
	}

	return elem;
}


void pt_mem_clear(void)
{
	int spcd_mem_hash_size = 1UL << spcd_mem_hash_bits;
	if (!mem)
		mem = vmalloc(sizeof(struct pt_mem_info) * spcd_mem_hash_size);
	if (mem)
		memset(mem, 0, sizeof(struct pt_mem_info) * spcd_mem_hash_size);
	else
		printk("SPCD BUG: could not allocate memory for mem hash table\n");
	pt_addr_conflict = 0;
}


void pt_mem_stop(void) 
{
	if (mem)
		vfree(mem);
}
