#include "include/stdio.h"
#include "include/string.h"
#include "include/fs.h"
#include "include/disk.h"
//partly copy from ics2015
static int min(int a,int b)
{
	return (a<b)?a:b;
}

int fs_open(const char *pathname, int flags) 
{
	int i;
	for(i=0;i<(NR_FILES);i++)
	{
		if(strcmp(directory_d.entries[i].filename,pathname)==0)
		{
			curenv->file[i].opened=true;
			curenv->file[i].offset=0;//set to the beginning of the file
			return i;
		}
	}
	printk("No such file!\n");
	return -1;
}


int fs_read(int fd, void *buf, int len) 
{
	if((fd>=0)&&(fd<NR_FILES)&&(curenv->file[fd].opened==true))
	{
		int i;
		//first we should read inode
		struct inode node;
		unsigned int ioff=directory_d.entries[fd].inode_offset;
		readsect((void*)node.data_block_offsets,ioff+201);
		int length=min(len,directory_d.entries[fd].file_size-curenv->file[fd].offset);
		unsigned int begin=curenv->file[fd].offset/512;
		unsigned int begin_off=curenv->file[fd].offset%512;
		unsigned int end=(curenv->file[fd].offset+len)/512;
		unsigned int end_off=(curenv->file[fd].offset+len)%512;
		
		if(begin==end){//the reading range is in one block
			char sect[512];
			readsect((void*)sect,node.data_block_offsets[begin]+201);
			for(i=0;i<len;i++){
				((char*)buf)[i]=sect[begin_off+i];
			}
		}
		else{//reading range covers several block
			char sect[512];
			readsect((void*)sect,node.data_block_offsets[begin]+201);
			for(i=0;i<512-begin_off;i++){
				((char*)buf)[i]=sect[begin_off+i];
			}
			buf=((char*)buf)+512-begin_off;
			for(i=begin+1;i<end;i++){
				readsect(buf,node.data_block_offsets[i]+201);
				buf=((char*)buf)+512;
			}
			readsect((void*)sect,node.data_block_offsets[end]+201);
			for(i=0;i<end_off;i++){
				((char*)buf)[i]=sect[i];
			}
		}
		curenv->file[fd].offset+=length;
		return length;
	}
	else	
	{
		printk("fs_read,fd is not valid\n");
		return -1;
	}
}

int fs_write(int fd, void *buf, int len) 
{
	if((fd>=0)&&(fd<NR_FILES)&&(curenv->file[fd].opened==true))
	{
		int i;
		//first we should read inode
		struct inode node;
		unsigned int ioff=directory_d.entries[fd].inode_offset;
		readsect((void*)node.data_block_offsets,ioff+201);
		//int length=min(len,directory_d[fd].file_size-curenv->file[fd].offset);
		int length=len;
		unsigned int begin=curenv->file[fd].offset/512;
		unsigned int begin_off=curenv->file[fd].offset%512;
		unsigned int end=(curenv->file[fd].offset+len)/512;
		unsigned int end_off=(curenv->file[fd].offset+len)%512;
		
		if(begin==end){//the reading range is in one block
			char sect[512];
			readsect((void*)sect,node.data_block_offsets[begin]+201);
			for(i=0;i<len;i++){
				sect[begin_off+i]=((char*)buf)[i];
			}
			writesect((void*)sect,node.data_block_offsets[begin]);
		}
		else{//reading range covers several block
			char sect[512];
			readsect((void*)sect,node.data_block_offsets[begin]+201);
			for(i=0;i<512-begin_off;i++){
				sect[begin_off+i]=((char*)buf)[i];
			}
			writesect((void*)sect,node.data_block_offsets[begin]);
			buf=((char*)buf)+512-begin_off;
			for(i=begin+1;i<end;i++){
				writesect(buf,node.data_block_offsets[i]+201);
				buf=((char*)buf)+512;
			}
			readsect((void*)sect,node.data_block_offsets[end]+201);
			for(i=0;i<end_off;i++){
				sect[i]=((char*)buf)[i];
			}
			writesect((void*)sect,node.data_block_offsets[end]+201);
		}
		curenv->file[fd].offset+=length;
		return length;
	}
	else	
	{
		printk("fs_read,fd is not valid\n");
		return -1;
	}
}




int fs_lseek(int fd, int offset, int whence) 
{
	if((fd>=0)&&(fd<NR_FILES)&&(curenv->file[fd].opened==true))
	{
		int temp_offset=0;
		switch(whence)
		{
		case SEEK_SET:
			temp_offset=offset;
			break;
		case SEEK_CUR:
			temp_offset=curenv->file[fd].offset+offset;
			break;
		case SEEK_END:
			temp_offset=directory_d.entries[fd].file_size+offset;
			break;
		default:
			printk("fs_lseek,default");
			break;
		}
		if((temp_offset>=0)&&(temp_offset<=directory_d.entries[fd].file_size))
		{
			curenv->file[fd].offset=temp_offset;
			return temp_offset;		
		}
		else
		{
			printk("offset overstep the boundary\n");
			return -1;
		}
	}
	else
	{
		printk("fs_lseek,fd error\n");
		return -1;
	}

}


int fs_close(int fd) 
{

	if((fd>=0)&&(fd<NR_FILES)&&curenv->file[fd].opened)
	{
		curenv->file[fd].opened=false;
		curenv->file[fd].offset=0;
		return 0;
	}
	else
	{
		printk("cannot close because this file is not opened\n");
		return -1;
	}
}

void fs_rewind(int fd){
	fs_lseek(fd,0,SEEK_SET);
}
