#include "include/types.h"
#include "include/string.h"
#include "include/video.h"
#include "include/system.h"
/* 绘制屏幕的帧缓冲实现。
 * 在某些版本的qemu-kvm上，由于每次访问显存映射区域都会产生一次VM exit，
 * 更新屏幕的速度可能非常缓慢从而引起游戏跳帧。定义宏SLOW以只重绘屏幕变化
 * 的部分；
 * */
#define SLOW

#ifdef SLOW
	#define PARTIAL_UPDATE
#endif

uint8_t *vmem = VMEM_ADDR;
static uint8_t vbuf[SCR_SIZE];
#ifdef PARTIAL_UPDATE
static uint8_t vref[SCR_SIZE];
#endif

void
prepare_buffer(void) {
#ifdef PARTIAL_UPDATE
	memset(vbuf,0,SCR_SIZE);
	memcpy(vref, vbuf, SCR_SIZE);
#endif
	vmem = vbuf;
	memset(vmem, 0, SCR_SIZE);
}

void
display_buffer(void) {
#ifdef PARTIAL_UPDATE
	int i;
	uint32_t *buf = (uint32_t*)vbuf;
	uint32_t *ref = (uint32_t*)vref;
	//uint32_t *mem = (uint32_t*)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE/4 ; i ++) {
		if (buf[i] != ref[i]) {
			//mem[i] = buf[i];
			//printk("displaybuffer\n");
			system_draw_pixel_off(i,buf[i]);
			ref[i]=buf[i];
		}
	}
#else
	//asm volatile ("cld; rep movsl" : : "c"(SCR_SIZE / 4), "S"(vbuf), "D"(vmem));	
	//vmem = VMEM_ADDR;
	//asm volatile ("cld; rep movsl" : : "c"(SCR_SIZE / 4), "S"(vbuf), "D"(vmem));

	for(i=0;i<SCR_SIZE/4;i++){
		system_draw_pixel_off(i,buf[i]);
			//printk("displaybuffer\n");
	}
#endif
}


/*
 * 蓝屏
 * 
 */
void
blue_screen(){
	//memset((void *)vmem,1,SCR_SIZE);
	system_clear_screen(1);
}
void
white_screen(){
	//memset((void *)vmem,15,SCR_SIZE);
	system_clear_screen(15);
}
void
black_screen(){
	//memset((void *)vmem,0,SCR_SIZE);
	system_clear_screen(0);
}
void
yellow_screen(){
	//memset((void *)vmem,14,SCR_SIZE);
	system_clear_screen(14);
}
int quater_width=SCR_HEIGHT/4;
int quater_height=SCR_HEIGHT/4;
int last_row=SCR_HEIGHT*4/5;
int last_line=SCR_HEIGHT*4/5;
int line_width=4;
int line_num=4;
int row_width=4;
int row_num=4;

void draw_border(void){
	int i,j,k;
	for(i=0;i<row_num;i++){
		for(j=0;j<row_width;j++){
		for(k=0;k<last_line;k++){
		vbuf[i*quater_height*SCR_WIDTH+j*SCR_WIDTH+k]=1;
	}
	}
	}
	for(i=0;i<last_row;i++){
		for(j=0;j<line_num;j++){
			for(k=0;k<line_width;k++)
			vbuf[i*SCR_WIDTH+j*quater_width+k]=1;
		}
	}

}

void draw_block(int index,uint8_t color){
	//assert((index<10)&&(index>0));
	int row=(index-1)/3;
	int line=index%3;
	if(line==0)
		line+=3;
	line-=1;//line和row都从0开始
	int start_place=row*(quater_height)*SCR_WIDTH+row_width*SCR_WIDTH+line_width+line*quater_width;
	int i,j;
	for(i=0;i<quater_height-4;i++){
	for(j=start_place+i*SCR_WIDTH;j<start_place+i*SCR_WIDTH+quater_width-line_width;j++){
	vbuf[j]=color;
	}
	}

}

	
