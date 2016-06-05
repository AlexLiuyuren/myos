#include <string.h>
#include <stdio.h>

int main(){
	FILE *fp1;
	FILE *fp2;
	char poem[10][30];
	int i;
	memset(poem,0,sizeof(poem));
	strcpy(poem[0],"When day is done");
	strcpy(poem[1],"if day is done");
	strcpy(poem[2],"if birds sings no more");
	strcpy(poem[3],"If the wind has flagged tired");
	fp1=fopen("poem1.txt","wb");
	for(i=0;i<4;i++){
		fwrite(poem[i],1,30,fp1);
	}
	fclose(fp1);
	return 0;
}
