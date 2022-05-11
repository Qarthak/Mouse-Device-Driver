#include <stdio.h>
#include <string.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

int main()
{
	char buffer[10];
	clock_t start=clock();
	int fp = open("/dev/BMDriver", O_RDWR);
	if(fp==-1){
		printf("Error opening file, errno = %d\n",errno);
	}
	printf("File opened succesfully\n");
	while (1)
	{
		read(fp, buffer, 1);
		if( ((double)(clock() - start)/CLOCKS_PER_SEC) > 1 ){
			break;
		}

	}
	return 0;
}
