//#include "include/keyboard.h"
#include "include/types.h"
#include "include/assert.h"
#include "include/keyboard.h"
#include "include/video.h"
typedef struct {
	int num;
	int isempty;//0,1,2
	int num_aph;
}BLOCK;
typedef struct {
	bool ifend;
	int isempty;
}END;
BLOCK block[3][3];
void init_block(){
	int i,j;
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			block[i][j].isempty=0;
			block[i][j].num=i*3+j+1;
			if(block[i][j].num==1)
				block[i][j].num_aph=16;
			else if(block[i][j].num==2)
				block[i][j].num_aph=22;
			else if(block[i][j].num==3)
				block[i][j].num_aph=4;
			else if(block[i][j].num==4)
				block[i][j].num_aph=0;
			else if(block[i][j].num==5)
				block[i][j].num_aph=18;
			else if(block[i][j].num==6)
				block[i][j].num_aph=3;
			else if(block[i][j].num==7)
				block[i][j].num_aph=25;
			else if(block[i][j].num==8)
				block[i][j].num_aph=23;
			else if(block[i][j].num==9)
				block[i][j].num_aph=2;
			else 
				block[i][j].num_aph=30;	
		}
	}	
}

END judge_end(){
	int i,j;
	bool flag=true;
	END temp;
	temp.ifend=false;
	if((block[0][0].isempty!=0)&&(block[0][0].isempty==block[0][1].isempty)&&(block[0][1].isempty==block[0][2].isempty))
	{temp.ifend=true;temp.isempty=block[0][0].isempty;flag=false;}
	else if((block[0][0].isempty!=0)&&(block[0][0].isempty==block[1][0].isempty)&&(block[0][0].isempty==block[2][0].isempty))
	{temp.ifend=true;temp.isempty=block[0][0].isempty;flag=false;}
	else if((block[2][0].isempty!=0)&&(block[2][0].isempty==block[2][1].isempty)&&(block[2][0].isempty==block[2][2].isempty))
	{temp.ifend=true;temp.isempty=block[2][0].isempty;flag=false;}
	else if((block[0][2].isempty!=0)&&(block[0][2].isempty==block[1][2].isempty)&&(block[0][2].isempty==block[2][2].isempty))
	{temp.ifend=true;temp.isempty=block[0][2].isempty;flag=false;}
	else if((block[1][0].isempty!=0)&&(block[1][0].isempty==block[1][1].isempty)&&(block[1][0].isempty==block[1][2].isempty))
	{temp.ifend=true;temp.isempty=block[1][0].isempty;flag=false;}
	else if((block[0][1].isempty!=0)&&(block[0][1].isempty==block[1][1].isempty)&&(block[0][1].isempty==block[2][1].isempty))
	{temp.ifend=true;temp.isempty=block[0][1].isempty;flag=false;}
	else if((block[0][0].isempty!=0)&&(block[0][0].isempty==block[1][1].isempty)&&(block[0][0].isempty==block[2][2].isempty))
	{temp.ifend=true;temp.isempty=block[0][0].isempty;flag=false;}
	else if((block[0][2].isempty!=0)&&(block[0][2].isempty==block[1][1].isempty)&&(block[0][2].isempty==block[2][0].isempty))
	{temp.ifend=true;temp.isempty=block[0][2].isempty;flag=false;}
	else {
		temp.ifend=false;temp.isempty=0;
		for(i=0;i<3;i++){
			for(j=0;j<3;j++){
				if(block[i][j].isempty==0){
					flag=false;

				}
			}
		}
	}
	if(flag==true){
		temp.ifend=true;
		temp.isempty=3;
	}
	return temp;

}

void game_loop(){
	int i,j,k; 
	int count=0;
	while(1){
		for(i=0;i<26;i++){
			if(query_key(i)){;
				for(j=0;j<3;j++){
					for(k=0;k<3;k++){
						if((block[j][k].num_aph==i)&&(block[j][k].isempty==0)){
							if (count%2==0)
							{
								printk("i=%d,j=%d.k=%d\n",i,j,k);
								printk("num=%d",block[j][k].num);
								draw_block(block[j][k].num,15);block[j][k].isempty=1;display_buffer();}
							else
							{draw_block(block[j][k].num,1);block[j][k].isempty=2;display_buffer();}
							count++;
							release_key(i);
						}
					}
				}	
			}
		}
		END judge=judge_end();
		if ((judge.ifend==true)&&((judge.isempty-1)==0)){
			white_screen();
			break;
		}
		else if ((judge.ifend==true)&&((judge.isempty-2)==0)){
			blue_screen();
			break;
		}
		else if((judge.ifend==true)&&(judge.isempty-3)==0){
			yellow_screen();
			break;
		}
	}
}
