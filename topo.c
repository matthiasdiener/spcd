#include "spcd.h"
#include <linux/cpu.h>

#define for_each_sibling(s, cpu) for_each_cpu(s, cpu_sibling_mask(cpu))
 
int num_nodes = 0, num_cores = 0, num_threads = 0;


void topo_start(void)
{
	unsigned long node, cpu, sibling;
	int curCPU = 0;

	printk("SPCD: detected hardware topology:\n");

	for_each_online_node(node) {
		int cpus[64] = {};
		int cores[64] = {};
		num_nodes++;

		printk("  node: %lu\n", node);

		for_each_online_cpu(cpu) {
			int physID = cpu_data(cpu).phys_proc_id;
			if (cpu_to_node(cpu) != node)
				continue;
			if (!cpus[physID]) {
				printk("    processor: %d\n", physID);
				cpus[physID] = 1;
				curCPU = physID;
			}
			num_threads++;
			if (physID == curCPU && !cores[cpu]) {
				printk("      cores: %2lu", cpu);
				cores[cpu] = 1;
				for_each_sibling(sibling, cpu) {
					if (!cores[sibling]) {
						printk(", %2lu", sibling);
						cores[sibling] = 1;
						num_cores++;
					}
				}
				printk("\n");
			}
		}
	}
	printk("SPCD: %d nodes/processors, %d cores, %d threads\n", num_nodes, num_cores, num_threads);
}

void topo_stop(void)
{
}
