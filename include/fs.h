#ifndef FS_H
#define FS_H
#define NR_FILES 32
//(512/sizeof(struct dirent))
#pragma pack(0)
struct bitmap{
	char mask[512*256];
};
#pragma pack()

#pragma pack(0)
struct dirent{
	char filename[24];
	unsigned int file_size;
	unsigned int inode_offset;
};//32byte
#pragma pack()

#pragma pack(0)
struct dir{
	struct dirent entries[512/sizeof(struct dirent)];
};//512byte
#pragma pack()

#pragma pack(0)
struct inode{
	unsigned int data_block_offsets[512/sizeof(unsigned int)];
};

#pragma pack()

typedef struct{
	bool opened;
	unsigned int offset;
}Fstate;

enum{SEEK_SET, SEEK_CUR,SEEK_END};

struct bitmap bitmap_d;
struct dir directory_d;

int fs_open(const char *pathname, int flags);
int fs_read(int fd, void *buf, int len);
int fs_write(int fd, void *buf, int len);
int fs_lseek(int fd, int offset, int whence);
int fs_close(int fd);
void fs_rewind(int fd);




#endif
