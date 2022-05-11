#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<string.h>
#include<errno.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{
	assert(argc >1); const char *str=argv[1];
	// char *str="Hello";
	char buf[100];
	char i=0;
	memset(buf,0,100);
	printf("Input: %s\n",str);
	
	int fp= open("/dev/myDev3",O_RDWR);
	if(fp==-1)
	printf("%d\n",errno);
	write(fp,str,strlen(str));
	while(1){
		int x;
		if((x=read(fp,&buf[i++],1))==0){
			break;
		}
	};
	
	printf("Reversed by the driver: %s", buf);
	return 0;
}
