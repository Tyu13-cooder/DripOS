#ifndef PMM_H
#define PMM_H
#include <stdint.h>
#include "stivale.h"

#define SIZE_OFFSET 8
#define PTR_AND_ADDR_SIZE 16

typedef void *symbol[];

extern symbol __kernel_end;
extern symbol __kernel_start;
extern symbol __kernel_code_start;
extern symbol __kernel_code_end;

void pmm_memory_setup(stivale_info_t *bootloader_info);
void *pmm_alloc(uint64_t size);
void pmm_unalloc(void *addr, uint64_t size);
uint64_t pmm_get_used_mem();
uint64_t pmm_get_free_mem();
uint64_t pmm_get_total_mem();

extern uint64_t cur_pain;

#endif