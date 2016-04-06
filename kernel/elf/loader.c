#include "include/boot.h"
#include "include/memlayout.h"
#define SECTSIZE 512

void readseg(unsigned char *, int, int);
extern pte_t entry_pgdir[NPDENTRIES];
extern struct PageInfo* page_free_list; 
void
loader(void) {
	struct ELFHeader *elf;
	struct ProgramHeader *ph, *eph;
	unsigned char* pa, *i;

	/* 因为引导扇区只有512字节，我们设置了堆栈从0x8000向下生长。
	 * 我们需要一块连续的空间来容纳ELF文件头，因此选定了0x8000。 */
	elf = (struct ELFHeader*)0xC0400000;
	/* 读入ELF文件头 */
	//pte_t *pte=pgdir_walk(entry_pgdir,elf,1);
	//page_insert(entry_pgdir,page_free_list,(void *)elf,PTE_W|PTE_U|PTE_P);
	readseg((unsigned char*)elf,4096,0);
	/* 把每个program segement依次读入内存 */
	ph=(struct ProgramHeader*)((char*)elf+elf->phoff);
	eph=ph+elf->phnum;
	for(;ph<eph;ph++){
	pa=(unsigned char*)ph->paddr;
	readseg(pa,ph->filesz,ph->off);
	for(i=pa+ph->filesz;i<pa+ph->memsz;*i++=0);
	}
	/*跳转到程序中*/
	//asm volatile("hlt");
	((void(*)(void))elf->entry)();

}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); /* 等待磁盘完毕 */
}

/* 读磁盘的一个扇区 */
void
readsect(void *dst, int offset) {
	int i;
	waitdisk();
	out_byte(0x1F2, 1);
	out_byte(0x1F3, offset);
	out_byte(0x1F4, offset >> 8);
	out_byte(0x1F5, offset >> 16);
	out_byte(0x1F6, (offset >> 24) | 0xE0);
	out_byte(0x1F7, 0x20);

	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}

/* 将位于磁盘offset位置的count字节数据读入物理地址pa */
void
readseg(unsigned char *pa, int count, int offset) {
	unsigned char *epa;
	epa = pa + count;
	pa -= offset % SECTSIZE;
	offset = (offset / SECTSIZE) + 1;
	for(; pa < epa; pa += SECTSIZE, offset ++)
		readsect(pa, offset);
}

void readseg_va(uint32_t *va,int count,int offset){
	
}
