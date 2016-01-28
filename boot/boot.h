/* ELF32二进制文件头 */
struct ELFHeader {
	unsigned int   magic;
	unsigned char  elf[12];
	unsigned short type;
	unsigned short machine;
	unsigned int   version;
	unsigned int   entry;
	unsigned int   phoff;
	unsigned int   shoff;
	unsigned int   flags;
	unsigned short ehsize;
	unsigned short phentsize;
	unsigned short phnum;
	unsigned short shentsize;
	unsigned short shnum;
	unsigned short shstrndx;
};

/* ELF32 Program header */
struct ProgramHeader {
	unsigned int type;
	unsigned int off;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int filesz;
	unsigned int memsz;
	unsigned int flags;
	unsigned int align;
};

/* I/O处理函数 */
static inline char
in_byte(short port) {
	char data;
	__asm__ __volatile__(
	"in %1, %%al"
	:"=a"(data)
	:"g"(port)
	:"memory"
	);
	/*请自行加入内联汇编代码*/
	return data;
}
static inline int 
in_long(short port) {
	int data;
	__asm__ __volatile__(
	"in %1, %%eax"
	:"=a"(data)
	:"g"(port)
	:"memory"
	);
	return data;
}
static inline void
out_byte(short port, char data) {
	/*请自行加入内联汇编代码*/
	__asm__ __volatile__(
	"move %1, %%al\n\t"\
	"out %%al, %0"
	:
	:"g"(port),"g"(data)
	:"memory"	
	);

}
