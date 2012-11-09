#ifndef __PT_COMM_H
#define __PT_COMM_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/traps.h>
#include <linux/hash.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <linux/kthread.h>

#include <linux/kprobes.h>
#include <linux/kallsyms.h>

#include <asm-generic/tlb.h>

#define PT_MAXTHREADS 4096

#define PT_MEM_HASH_BITS 26UL
#define PT_MEM_HASH_SIZE (1UL << PT_MEM_HASH_BITS)

#define PT_PID_HASH_BITS 14UL
#define PT_PID_HASH_SIZE (1UL << PT_PID_HASH_BITS)

extern unsigned long pt_pf;
extern unsigned long pt_addr_conflict;
extern unsigned long pt_pf_extra;
extern struct task_struct *pt_task;
extern unsigned long pt_num_walks;

extern struct page* (*vm_normal_page_p)(struct vm_area_struct *vma, unsigned long addr, pte_t pte);

extern int (*walk_page_range_p)(unsigned long addr, unsigned long end,
		    struct mm_walk *walk);

struct pt_mem_info {
	unsigned long pg_addr;
	u8 sharer[2];
};

int pt_get_tid(int pid); 
void pt_add_pid(int pid);
void pt_delete_pid(int pid);
void pt_pid_clear(void);
int spcd_get_num_threads(void);
int spcd_get_active_threads(void);

struct pt_mem_info* pt_get_mem(unsigned long addr);
struct pt_mem_info* pt_get_mem_init(unsigned long addr);
void pt_mem_clear(void);

void pt_share_clear(void);

void pt_print_stats(void);
void pt_print_share(void);

void pt_check_comm(int tid, unsigned long address);

void pt_mark_pte(unsigned long addr, pte_t *pte);
void pt_fix_pte(struct pt_mem_info *elem, unsigned long addr);

int pt_pf_thread_func(void* v);
void spcd_pf_thread_clear(void);

int spcd_check_name(char *name);

#endif
