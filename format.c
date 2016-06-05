#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
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

struct bitmap bitmap_d;
struct dir directory_d;
struct inode inode_d;

#define SET_BIT(n) (bitmap_d.mask[n/8]|=(0x1<<(n%8)))
#define CLEAR_BIT(n) (bitmap_d.mask[n/8]&=(~(0x1<<(n%8))))
#define USED(n) (bitmap_d.mask[n/8]&(0x1<<(n%8)))

unsigned int alloc_block(){
	int i;
	for(i=0;i<512*1024*1024;i++){
		if(!USED(i)){
			SET_BIT(i);
			return i;
		}
			
	}
	return 0;
}

int main(int argc,char *argv[]){
	int i;
 	FILE * disk=fopen("data.disk","wb");	
	memset(bitmap_d.mask,0,sizeof(bitmap_d.mask));
	memset(directory_d.entries,0,sizeof(directory_d.entries));
	for(i=0;i<257;i++)
		SET_BIT(i);
	for(i=1;i<argc;i++){
		FILE *fp=fopen(argv[i],"rb");
		if(!fp) {
			system("pwd");
			perror("failed:");
		}
		printf("argv[%d]=%s\n",i,argv[i]);
		assert(fp);
		fseek(fp,0,SEEK_END);
		unsigned int filesz=ftell(fp);
		rewind(fp);
		strcpy(directory_d.entries[i-1].filename,argv[i]);
		directory_d.entries[i-1].file_size=filesz;
		unsigned int inode_index=alloc_block();
		directory_d.entries[i-1].inode_offset=inode_index;

		//write in file data
		struct inode node;
		char buf[512];
		int count=0;
		while(fread(buf,1,512,fp)){
			unsigned int bias=alloc_block();
			node.data_block_offsets[count]=bias;
			fseek(disk,bias*512,SEEK_SET);
			fwrite(buf,1,512,disk);
			rewind(disk);
			count++;
		}
		//write in inode
		fseek(disk,inode_index*512,SEEK_SET);
		fwrite(&node,1,512,disk);
		rewind(disk);
		fclose(fp);
	}
	fseek(disk,512*216,SEEK_SET);
	fwrite(directory_d.entries,1,512,disk);
	rewind(disk);
	fseek(disk,0,SEEK_SET);
	fwrite(bitmap_d.mask,1,512*256,disk);
	rewind(disk);
	fclose(disk);
	return 0;
}
