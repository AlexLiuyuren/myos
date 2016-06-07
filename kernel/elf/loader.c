#include "include/boot.h"
#include "include/string.h"
#include "include/pmap.h"
#include "include/mmu.h"
#include "include/disk.h"
#include "include/fs.h"
#include "include/memlayout.h"
#include "include/env.h"

#define SECTSIZE 512

extern struct PageInfo* page_free_list; 
unsigned char buffer[4096];
void*
loader(struct Env*penv,int diskoff) {
	struct ELFHeader *elf;
	struct ProgramHeader *ph, *eph;
	unsigned char pagebuffer[4096];
	//int i;

	elf = (struct ELFHeader*)buffer;
	/* 读入ELF文件头 */
//	readseg((unsigned char*)elf,4096,0,diskoff);
	int fd=0;
	fs_read(fd,(void*)elf,4096);
	fs_rewind(fd);
	printk("directory_d.entries[0]=%s\n",directory_d.entries[0].filename);
	printk("elfmagic=%x\n",elf->magic);
	printk("elfentry=%x\n",elf->entry);
	/* 把每个program segement依次读入内存 */
	ph=(struct ProgramHeader*)((char*)elf+elf->phoff);
	eph=ph+elf->phnum;
	for(;ph<eph;ph++){
		uint32_t va=ph->vaddr;
		int data_loaded=0;
	if(ph->type==1){
		while(va<ph->vaddr+ph->memsz){
			memset(pagebuffer,0,4096);
			uint32_t offset=va&0xfff;
			va=va&0xfffff000;
			struct PageInfo *page=page_alloc(1);
			page_insert(penv->env_pgdir,page,(void *)va,PTE_U|PTE_W);
			int n=(4096-offset)>ph->memsz?ph->memsz:(4096-offset);
			if(n!=0){
				fs_lseek(fd,ph->off+data_loaded,SEEK_SET);
			//readseg((unsigned char*)(pagebuffer+offset),n,ph->off+data_loaded,diskoff);
				fs_read(fd,(void*)(pagebuffer+offset),n);
			}
			memcpy((void *)page2kva(page),pagebuffer,4096);

			va+=4096;
			data_loaded+=n;
		}	
	}
	}
	/*跳转到程序中*/
	//asm volatile("hlt");
	//boot_map_region(entry_pgdir,0xa0000,320*200,0xa0000,PTE_W|PTE_U);
	
	//return (void*)elf->entry;

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
void
writesect(void *src, int offset) {
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
		out_long(0x1F0,((unsigned int *)src)[i]);
	}
}
/* 将位于磁盘offset位置的count字节数据读入物理地址pa */
void
readseg(unsigned char *va, int count, int offset,int diskoff) {
	unsigned char *eva;
	eva = va + count;
	va -= offset % SECTSIZE;
	//set game at 201 in Makefile
	offset = (offset / SECTSIZE) + diskoff+1;
	for(; va < eva; va += SECTSIZE, offset ++)
		readsect(va, offset);
}

	
