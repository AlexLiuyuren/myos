#include "include/types.h"
#define SEG_WRITABLE			0X2
#define SEG_READABLE			0X2
#define SEG_EXECUTABLE			0X8
#define SEG_RW_DATA				0x2 //WRITEBLE
#define SEG_EXE_CODE			0xa	//READABLE|EXECUTABLE
//#define NR_SEGMENTS             512
#define DPL_KERNEL              0
#define DPL_USER                3
#define SEG_KERNEL_CODE         1 
#define SEG_KERNEL_DATA         2
#define SEG_USER_CODE			3
#define SEG_USER_DATA			4
#define SEG_TSS					5
#define SELECTOR_KERNEL(s)		( ((s) << 3) | DPL_KERNEL )
#define SELECTOR_USER(s)		( ((s) << 3) | DPL_USER )

#define SELECTOR_INDEX(s)		(((s) >> 3) - 4)

//#include "types.h"
typedef struct SegmentDescriptor {
	uint32_t limit_15_0          : 16;
	uint32_t base_15_0           : 16;
	uint32_t base_23_16          : 8;
	uint32_t type                : 4;
	uint32_t segment_type        : 1;
	uint32_t privilege_level     : 2;
	uint32_t present             : 1;
	uint32_t limit_19_16         : 4;
	uint32_t soft_use            : 1;
	uint32_t operation_size      : 1;
	uint32_t pad0                : 1;
	uint32_t granularity         : 1;
	uint32_t base_31_24          : 8;
} SegDesc;

//#define SEG_NULL	(struct SegDesc){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
typedef struct {
	uint32_t link;         // old ts selector
	uint32_t esp0;         // Ring 0 Stack pointer and segment selector
	uint32_t ss0;          // after an increase in privilege level
	char dontcare[88];
}TSS;
/*
 * This file contains definitions for the x86 memory management unit (MMU),
 * including paging- and segmentation-related data structures and constants,
 */
